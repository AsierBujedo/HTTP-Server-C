#ifndef TPOOL_H
#define TPOOL_H

#include <pthread.h>

typedef struct {
    pthread_t **pool;
    int num_threads;
    pthread_mutex_t *mutex;
} thread_pool;

thread_pool* initPool(pthread_mutex_t *mutex);
void addThread(thread_pool *pool, pthread_t *thread);
void backCopy(thread_pool *thpool, int idx);
void endThread(thread_pool *pool, pthread_t *thread);
void endAllThreads(thread_pool *pool);


#endif // TPOOL_H