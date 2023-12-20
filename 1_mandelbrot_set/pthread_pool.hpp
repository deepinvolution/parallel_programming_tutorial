#include <vector>
#include <queue>
#include <pthread.h>

#ifndef __TPOOL_H__
#define __TPOOL_H__

#include <stdbool.h>
#include <stddef.h>

typedef void* (*thread_func_t)(void* arg);

typedef struct thread_pool_work {
    thread_func_t     func;
    void*             arg;
    struct thread_pool_work* next;
} thread_pool_work_t;

typedef struct thread_pool {
    thread_pool_work_t*    work_first;
    thread_pool_work_t*    work_last;
    pthread_mutex_t  work_mutex;
    pthread_cond_t   work_cond;
    size_t           thread_cnt;
    bool             stop;
    pthread_t*       workers;
} thread_pool_t;

static thread_pool_work_t *thread_pool_work_create(thread_func_t func, void *arg)
{
    thread_pool_work_t *work;

    if (func == nullptr) return nullptr;

    work       = new thread_pool_work_t;
    work->func = func;
    work->arg  = arg;
    work->next = NULL;
    return work;
}

static void thread_pool_work_destroy(thread_pool_work_t *work)
{
    if (work == nullptr) return;
    delete work;
}

static thread_pool_work_t *thread_pool_work_get(thread_pool_t *tm)
{
    thread_pool_work_t *work;

    if (tm == nullptr) return nullptr;

    work = tm->work_first;
    if (work == nullptr) return nullptr;

    if (work->next == nullptr) {
        tm->work_first = nullptr;
        tm->work_last  = nullptr;
    } else {
        tm->work_first = work->next;
    }

    return work;
}

static void* thread_pool_worker(void* arg)
{
    thread_pool_t      *tm = (thread_pool_t*)arg;
    thread_pool_work_t *work;

    while (true) {
        pthread_mutex_lock(&(tm->work_mutex));

        while (tm->work_first == nullptr && !tm->stop)
            pthread_cond_wait(&(tm->work_cond), &(tm->work_mutex));

        work = thread_pool_work_get(tm);

        pthread_mutex_unlock(&(tm->work_mutex));

        if (tm->stop && !work) break;

        if (work) {
            work->func(work->arg);
            thread_pool_work_destroy(work);
        }
    }
    pthread_exit(nullptr);
}

thread_pool_t *thread_pool_create(size_t num)
{
    thread_pool_t   *tm;
    pthread_t  thread;

    tm             = new thread_pool_t;
    tm->thread_cnt = num;
    tm->workers    = new pthread_t[num];

    pthread_mutex_init(&(tm->work_mutex), NULL);
    pthread_cond_init(&(tm->work_cond), NULL);

    tm->work_first = NULL;
    tm->work_last  = NULL;

    for (int i = 0; i < num; i++)
        pthread_create(&tm->workers[i], NULL, thread_pool_worker, tm);

    return tm;
}

void thread_pool_destroy(thread_pool_t *tm)
{
    thread_pool_work_t *work;
    thread_pool_work_t *work2;

    if (tm == NULL)
        return;

    pthread_mutex_lock(&(tm->work_mutex));
    work = tm->work_first;
    while (work != NULL) {
        work2 = work->next;
        thread_pool_work_destroy(work);
        work = work2;
    }
    tm->stop = true;
    pthread_cond_broadcast(&(tm->work_cond));
    pthread_mutex_unlock(&(tm->work_mutex));

    pthread_mutex_destroy(&(tm->work_mutex));
    pthread_cond_destroy(&(tm->work_cond));

    free(tm);
}

bool thread_pool_add_work(thread_pool_t *tm, thread_func_t func, void *arg)
{
    thread_pool_work_t *work;

    if (tm == nullptr) return false;

    work = thread_pool_work_create(func, arg);
    if (work == nullptr) return false;

    pthread_mutex_lock(&(tm->work_mutex));
    if (tm->work_first == NULL) {
        tm->work_first = work;
        tm->work_last  = tm->work_first;
    } else {
        tm->work_last->next = work;
        tm->work_last       = work;
    }

    pthread_mutex_unlock(&(tm->work_mutex));
    pthread_cond_signal(&(tm->work_cond));

    return true;
}

void thread_pool_wait(thread_pool_t *tm)
{
    if (tm == NULL)
        return;

    tm->stop = true;
    pthread_cond_broadcast(&(tm->work_cond));
    for (int i = 0; i < tm->thread_cnt; i++)
        pthread_join(tm->workers[i], nullptr);
}


#endif /* __TPOOL_H__ */
