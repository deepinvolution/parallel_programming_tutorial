#include <vector>
#include <queue>
#include <pthread.h>

#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

typedef void* (*thread_func_t)(void* arg);

// TODO: optimize data copy when
class ThreadTask {
public:
    ThreadTask() : _empty(true) {}
    ThreadTask(thread_func_t func, void* arg) : _func(func),
                                                _arg(arg),
                                                _empty(false) {}
    bool empty() { return _empty; }
    void run() { _func(_arg); }

private:
    thread_func_t _func; // function
    void* _arg; // function inputs
    bool _empty; // function state
};

// Implement a producer-consumer thread pool class
class ThreadPool {
public:
    ThreadPool(int num);
    ~ThreadPool();
    bool push(thread_func_t func, void *arg);
    void join();

private:
    // shared data
    std::vector<pthread_t> workers;
    std::queue<ThreadTask> tasks;

    // mutex and cond to protect task queue
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    // state of thread pool
    bool terminate;

    // opague function for threads' routine
    static void* thread_pool_worker(void* arg);
};

void* ThreadPool::thread_pool_worker(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    while (true) {
        pthread_mutex_lock(&(pool->mutex));

        while (pool->tasks.empty() && !pool->terminate)
            pthread_cond_wait(&(pool->cond), &(pool->mutex));

        ThreadTask task;
        if (!pool->tasks.empty()) {
            task = pool->tasks.front();
            pool->tasks.pop();
        }

        pthread_mutex_unlock(&(pool->mutex));

        if (pool->terminate && task.empty()) break;
        if (!task.empty()) task.run();
    }
    pthread_exit(nullptr);
}

ThreadPool::ThreadPool(int num) {
    workers.resize(num);
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    workers.resize(num);
    for (int i = 0; i < num; i++)
        pthread_create(&workers[i], nullptr,
                       &ThreadPool::thread_pool_worker, this);
}

ThreadPool::~ThreadPool() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

bool ThreadPool::push(thread_func_t func, void *arg) {
    if (!func) return false;
    ThreadTask task(func, arg);
    pthread_mutex_lock(&mutex);
    tasks.push(task);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);
    return true;
}

void ThreadPool::join() {
    terminate = true;
    pthread_cond_broadcast(&cond);
    for (int i = 0; i < workers.size(); i++)
        pthread_join(workers[i], nullptr);
}

#endif /* __THREAD_POOL_H__ */
