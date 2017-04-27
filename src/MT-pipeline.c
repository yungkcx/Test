#include "all.h"

typedef struct stage_tag {
    pthread_mutex_t   mutex;
    pthread_cond_t    avail;
    pthread_cond_t    ready;
    int               data_ready;
    long              data;
    pthread_t         thread;
    struct stage_tag* next;
} stage_t;

typedef struct pipe_tag {
    pthread_mutex_t mutex;
    stage_t*        head;
    stage_t*        tail;
    int             stages;
    int             active;
} pipe_t;

int pipe_send(stage_t* stage, long data);
void* pipe_stage(void* arg);
int pipe_create(pipe_t* pipe, int stages);
int pipe_start(pipe_t* pipe, long value);
int pipe_result(pipe_t* pipe, long* result);

int main() {
    pipe_t  pipe;
    long    value, result;
    char    line[128];

    pipe_create(&pipe, 10);
    printf("Input a number or '='\n");

    while (1) {
        printf("Input>> ");
        if (fgets(line, sizeof(line), stdin) == NULL)
            exit(0);
        if (strlen(line) <= 1)
            continue;
        if (strlen(line) <= 2 && line[0] == '=') {
            if (pipe_result(&pipe, &result))
                printf("The result is: %ld\n", result);
            else
                printf("The pipe is empty\n");
        } else {
            if (sscanf(line, "%ld", &value) < 1)
                emsg("Input integer value.");
            else
                pipe_start(&pipe, value);
        }
    }
}

int pipe_send(stage_t* stage, long data) {
    int s;

    if ((s = pthread_mutex_lock(&stage->mutex)) != 0)
        return s;

    while (stage->data_ready) {
        if ((s = pthread_cond_wait(&stage->ready, &stage->mutex)) != 0) {
            pthread_mutex_unlock(&stage->mutex);
            return s;
        }
    }

    stage->data = data;
    stage->data_ready = 1;
    if ((s = pthread_cond_signal(&stage->avail)) != 0) {
        pthread_mutex_unlock(&stage->mutex);
        return s;
    }
    s = pthread_mutex_unlock(&stage->mutex);
    return s;
}

void* pipe_stage(void* arg) {
    stage_t* stage = (stage_t*) arg;
    stage_t* next_stage = stage->next;
    int s;

    s = pthread_mutex_lock(&stage->mutex);
    if (s != 0)
        handle_err(s, "Lock pipe stage");
    while (1) {
        while (stage->data_ready != 1) {
            s = pthread_cond_wait(&stage->avail, &stage->mutex);
            if (s != 0)
                handle_err(s, "Wait for previous stage");
        }

        pipe_send(next_stage, stage->data + 1);
        stage->data_ready = 0;
        s = pthread_cond_signal(&stage->ready);
        if (s != 0)
            handle_err(s, "Wake next stage");
    }
}

int pipe_create(pipe_t* pipe, int stages) {
    stage_t**   link = &pipe->head;
    stage_t*    new_stage;
    stage_t*    stage; 
    int s;

    s = pthread_mutex_init(&pipe->mutex, NULL);
    if (s != 0)
        handle_err(s, "Init pipe mutex");
    pipe->stages = stages;
    pipe->active = 0;

    for (int pipe_index = 0; pipe_index <= stages; pipe_index++) {
        new_stage = malloc(sizeof(stage_t));
        if ((s = pthread_mutex_init(&new_stage->mutex, NULL)) != 0)
            handle_err(s, "Init stage mutex");
        if ((s = pthread_cond_init(&new_stage->avail, NULL)) != 0)
            handle_err(s, "Init avail cond");
        if ((s = pthread_cond_init(&new_stage->ready, NULL)) != 0)
            handle_err(s, "Init ready cond");
        new_stage->data_ready = 0;
        *link = new_stage;
        link = &new_stage->next;
    }

    *link = (stage_t*) NULL;
    pipe->tail = new_stage;

    for (stage = pipe->head; stage->next != NULL; stage = stage->next) {
        if ((s = pthread_create(&stage->thread, NULL, pipe_stage, (void*) stage)) != 0)
            handle_err(s, "Create pipe stage");
    }
    return 0;
}

int pipe_start(pipe_t* pipe, long value) {
    int s;

    if ((s = pthread_mutex_lock(&pipe->mutex)) != 0)
        handle_err(s, "Lock pipe mutex");
    pipe->active++;
    if ((s = pthread_mutex_unlock(&pipe->mutex)) != 0)
        handle_err(s, "Unlock pipe mutex");
    pipe_send(pipe->head, value);
    return 0;
}

int pipe_result(pipe_t* pipe, long* result) {
    stage_t*    tail = pipe->tail;
    int         empty = 0;
    int         s;

    if ((s = pthread_mutex_lock(&pipe->mutex)) != 0)
        handle_err(s, "Lock pipe mutex");
    if (pipe->active <= 0)
        empty = 1;
    else
        pipe->active--;
    if ((s = pthread_mutex_unlock(&pipe->mutex)) != 0)
        handle_err(s, "Unlock pipe mutex");
    if (empty)
        return 0;

    if ((s = pthread_mutex_lock(&tail->mutex)) != 0)
        handle_err(s, "Lock tail mutex");
    while (! tail->data_ready)
        pthread_cond_wait(&tail->ready, &tail->mutex);
    *result = tail->data;
    tail->data_ready = 0;
    pthread_cond_signal(&tail->ready);
    pthread_mutex_unlock(&tail->mutex);

    return 1;
}
