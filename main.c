#include "all.h"

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int ready = 0;

void* client_routine(void* arg) {
    int s;
    UNUSED(arg);
    if ((s = pthread_mutex_lock(&mutex)) != 0)
        handle_err(s, "pthread_mutex_lock");
    pthread_cleanup_push((void (*)(void *)) pthread_mutex_unlock, &mutex);
    pthread_exit(NULL);
    pthread_cleanup_pop(0);
    return NULL;
}

int main() {
    pthread_t thread;
    int s;

    ready = 1;
    if ((s = pthread_create(&thread, NULL, client_routine, NULL)) != 0)
        handle_err(s, "Create client thread");
    if ((s = pthread_join(thread, NULL)) != 0)
        handle_err(s, "pthread_join");
    if ((s = pthread_mutex_lock(&mutex)) != 0)
        handle_err(s, "pthread_mutex_lock");
    printf("mutex locked\n");
    if ((s = pthread_mutex_unlock(&mutex)) != 0)
        handle_err(s, "pthread_mutex_unlock");
    printf("mutex unlocked\n");

    exit(EXIT_SUCCESS);
}
