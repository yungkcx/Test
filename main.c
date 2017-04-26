#include "all.h"

#define handle_error_en(en, msg)\
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define MAXJOB 10

typedef struct {
    pthread_mutex_t mutex;
    int arr[MAXJOB];
    int closed;
    int cur;
} jobs_t;

typedef struct {
    pthread_mutex_t mutex;
    int arr[MAXJOB];
    int cur;
} results_t;

typedef struct {
    jobs_t* jobs;
    results_t* res;
    int wn;
} args_t;

void jobs_add(jobs_t* jobs, int w) {
    int s;
    if ((s = pthread_mutex_lock(&jobs->mutex)) != 0)
        handle_error_en(s, "pthread_mutex_lock");
    if (jobs->cur < MAXJOB)
        jobs->arr[jobs->cur++] = w;
    if ((s = pthread_mutex_unlock(&jobs->mutex)) != 0)
        handle_error_en(s, "pthread_mutex_lock");
}

int jobs_get(jobs_t* jobs) {
    int s, w;
    if ((s = pthread_mutex_lock(&jobs->mutex)) != 0)
        handle_error_en(s, "pthread_mutex_lock");
    if (jobs->cur > 0) {
        w = jobs->arr[--jobs->cur];
    } else {
        w = -1;
    }
    if ((s = pthread_mutex_unlock(&jobs->mutex)) != 0)
        handle_error_en(s, "pthread_mutex_lock");
    return w;
}

void res_add(results_t* res, int r) {
    int s;
    if ((s = pthread_mutex_lock(&res->mutex)) != 0)
        handle_error_en(s, "pthread_mutex_lock");
    if (res->cur < MAXJOB)
        res->arr[res->cur++] = r;
    if ((s = pthread_mutex_unlock(&res->mutex)) != 0)
        handle_error_en(s, "pthread_mutex_lock");
}

void* thread_func(void* arg) {
    int job;
    args_t* args = (args_t*) arg;

    while (args->jobs->closed == 0 || args->jobs->cur != 0) {
        job = jobs_get(args->jobs);
        if (job == -1) {
            continue;
        }
        printf("worker %d start job %d\n", args->wn, job);
        sleep(1);
        res_add(args->res, job*2);
        printf("worker %d finished job %d\n", args->wn, job);
    }

    return NULL;
}

int main() {
    int s;
    pthread_t ths[5];
    jobs_t jobs = { PTHREAD_MUTEX_INITIALIZER, {0}, 0, 0 };
    results_t res = { PTHREAD_MUTEX_INITIALIZER, {0}, 0 };
    args_t args[5];

    for (int i = 0; i < 5; ++i) {
        args[i].jobs = &jobs;
        args[i].res = &res;
        args[i].wn = i;
        if ((s = pthread_create(&ths[i], NULL, thread_func, &args[i])) != 0)
            handle_error_en(s, "pthread_create");
    }
    for (int i = 0; i < MAXJOB; ++i) {
        jobs_add(&jobs, i);
    }
    jobs.closed = 1;
    for (int i = 0; i < 5; ++i)
        if ((s = pthread_join(ths[i], NULL)) != 0)
            handle_error_en(s, "pthread_join");
    printf("results: ");
    for (int i = 0; i < MAXJOB; ++i)
        printf("%-3d", res.arr[i]);
    printf("\n");

    exit(EXIT_SUCCESS);
}
