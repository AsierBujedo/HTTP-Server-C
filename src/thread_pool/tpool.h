#ifndef TPOOL_H
#define TPOOL_H

#include <pthread.h>

typedef struct {
    pthread_t **pool;
    int num_threads;
} thread_pool;

thread_pool* initPool();
void addThread(thread_pool *pool, pthread_t *thread);
void backCopy(thread_pool *thpool, int idx);
void endThread(thread_pool *pool, pthread_t *thread);
void endAllThreads(thread_pool *pool);


#endif // TPOOL_H