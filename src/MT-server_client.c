#include "../all.h"

#define CLIENT_THREADS  4
#define REQ_READ        1
#define REQ_WRITE       2
#define REQ_QUIT        3

typedef struct request_tag {
    struct request_tag* next;
    int                 operation;
    int                 synchronous;
    int                 done_flag;
    pthread_cond_t      done;
    char                prompt[32];
    char                text[128];
} request_t;

typedef struct tty_server_tag {
    pthread_mutex_t mutex;
    pthread_cond_t  request;
    request_t*      first;
    request_t*      last;
    int             running;
} tty_server_t;

tty_server_t tty_server = {
    PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER,
    NULL, NULL, 0
};

int client_threads;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t client_done = PTHREAD_COND_INITIALIZER;

void* tty_server_routine(void* arg);
void tty_server_request(int operation, int sync, const char* prompt, char* string);
void* client_routine(void* arg);

int main() {
    pthread_t thread;
    int s;

    client_threads = CLIENT_THREADS;
    for (intptr_t count = 0; count < client_threads; ++count)
        if ((s = pthread_create(&thread, NULL, client_routine, (void*) count)) != 0)
            handle_err(s, "Create client thread");
    if ((s = pthread_mutex_lock(&client_mutex)) != 0)
        handle_err(s, "Lock client mutex");
    while (client_threads > 0)
        if ((s = pthread_cond_wait(&client_done, &client_mutex)) != 0)
            handle_err(s, "Wait for clients to finish");
    if ((s = pthread_mutex_unlock(&client_mutex)) != 0)
        handle_err(s, "Unlock client mutex");
    printf("All clients done\n");
    tty_server_request(REQ_QUIT, 1, NULL, NULL);
    
    exit(EXIT_SUCCESS);
}

void* tty_server_routine(void* arg) {
    request_t* request;
    int operation;
    size_t len;
    int s;

    UNUSED(arg);
    while (1) {
        if ((s = pthread_mutex_lock(&tty_server.mutex)) != 0)
            handle_err(s, "Lock server mutex");

        while (tty_server.first == NULL)
            if ((s = pthread_cond_wait(&tty_server.request, &tty_server.mutex)) != 0)
                handle_err(s, "Wait for request");
        request = tty_server.first;
        tty_server.first = request->next;
        if (tty_server.first == NULL)
            tty_server.last = NULL;
        if ((s = pthread_mutex_unlock(&tty_server.mutex)) != 0)
            handle_err(s, "Unlock server mutex");

        operation = request->operation;
        switch (operation) {
            case REQ_QUIT:
                break;
            case REQ_READ:
                if (strlen(request->prompt) > 0)
                    printf("%s", request->prompt);
                if (fgets(request->text, 128, stdin) == NULL)
                    request->text[0] = '\0';
                len = strlen(request->text);
                if (len > 0 && request->text[len - 1] == '\n')
                    request->text[len - 1] = '\0';
                break;
            case REQ_WRITE:
                puts(request->text);
                break;
            default:
                break;
        }
        if (request->synchronous) {
            if ((s = pthread_mutex_lock(&tty_server.mutex)) != 0)
                handle_err(s, "Lock server mutex");
            request->done_flag = 1;
            if ((s = pthread_cond_signal(&request->done)) != 0)
                handle_err(s, "Signal server condition");
            if ((s = pthread_mutex_unlock(&tty_server.mutex)) != 0)
                handle_err(s, "Unlock server mutex");
        } else {
            free(request);
        }
        if (operation == REQ_QUIT)
            break;
    }
    return NULL;
}

void tty_server_request(int operation, int sync, const char* prompt, char* string) {
    request_t* request;
    int s;

    if ((s = pthread_mutex_lock(&tty_server.mutex)) != 0)
        handle_err(s, "Lock server mutex");
    if (! tty_server.running) {
        pthread_t thread;
        pthread_attr_t detached_attr;

        if ((s = pthread_attr_init(&detached_attr)) != 0)
            handle_err(s, "Init attributes object");
        if ((s = pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED)) != 0)
            handle_err(s, "Set detach state");
        tty_server.running = 1;
        if ((s = pthread_create(&thread, NULL, tty_server_routine, NULL)) != 0)
            handle_err(s, "Create server");
        pthread_attr_destroy(&detached_attr);
    }

    if ((request = malloc(sizeof(request_t))) == NULL)
        handle_err(errno, "Allocate request");
    request->next = NULL;
    request->operation = operation;
    request->synchronous = sync;
    if (sync) {
        request->done_flag = 0;
        if ((s = pthread_cond_init(&request->done, NULL)) != 0)
            handle_err(s, "Init request condition");
    }
    if (prompt != NULL)
        strncpy(request->prompt, prompt, 32);
    else
        request->prompt[0] = '\0';
    if (operation == REQ_WRITE && string != NULL)
        strncpy(request->text, string, 128);
    else
        request->text[0] = '\0';

    if (tty_server.first == NULL) {
        tty_server.first = tty_server.last = request;
    } else {
        (tty_server.last)->next = request;
        tty_server.last = request;
    }

    if ((s = pthread_cond_signal(&tty_server.request)) != 0)
        handle_err(s, "Wake server");
    if (sync) {
        while (! request->done_flag)
            if ((s = pthread_cond_wait(&request->done, &tty_server.mutex)) != 0)
                handle_err(s, "Wait for sync request");
        if (operation == REQ_READ) {
            if (strlen(request->text) > 0)
                strcpy(string, request->text);
            else
                string[0] = '\0';
        }
        if ((s = pthread_cond_destroy(&request->done)) != 0)
            handle_err(s, "Destroy request condition");
        free(request);
    }
    if ((s = pthread_mutex_unlock(&tty_server.mutex)) != 0)
        handle_err(s, "Unlock mutex");
}

void* client_routine(void* arg) {
    int my_number = (int) arg;
    char prompt[32];
    char string[128], formatted[128];
    int s;

    sprintf(prompt, "Client %d> ", my_number);
    while (1) {
        tty_server_request(REQ_READ, 1, prompt, string);
        if (strlen(string) == 0)
            break;
        for (int i = 0; i < 4; ++i) {
            sprintf(formatted, "(%d#%d) %s", my_number, i, string);
            tty_server_request(REQ_WRITE, 0, NULL, formatted);
            sleep(1);
        }
    }
    if ((s = pthread_mutex_lock(&client_mutex)) != 0)
        handle_err(s, "Lock client mutex");
    client_threads--;
    if (client_threads <= 0)
        if ((s = pthread_cond_signal(&client_done)) != 0)
            handle_err(s, "Signal clients done");
    if ((s = pthread_mutex_unlock(&client_mutex)) != 0)
        handle_err(s, "Unlock clients mutex");

    return NULL;
}
