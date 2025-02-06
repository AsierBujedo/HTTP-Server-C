#include<stdio.h>
#include<stdlib.h>
#include"tpool.h"


/**
 * @brief Initializes a thread pool.
 *
 * This function allocates memory for a thread pool structure and initializes its members.
 *
 * @param mutex A pointer to a pthread_mutex_t that will be used by the thread pool.
 * @return A pointer to the initialized thread pool structure.
 *
 * @note The function will terminate the program with an error message if memory allocation fails.
 */
thread_pool* initPool(pthread_mutex_t *mutex) {
    thread_pool *thpool = malloc(sizeof(thread_pool));
    if(thpool == NULL) {
        fprintf(stderr, "Failed allocating memory");
        exit(EXIT_FAILURE);
    }
    thpool->pool = malloc(sizeof(pthread_t*));
    if(thpool->pool == NULL) {
        fprintf(stderr, "Failed allocating memory");
        exit(EXIT_FAILURE);
    }
    thpool->num_threads = 0;
    thpool->mutex = mutex;
    return thpool;
}

/**
 * @brief Adds a new thread to the thread pool.
 *
 * This function increments the number of threads in the thread pool and reallocates
 * memory to accommodate the new thread. If memory reallocation fails, it prints an
 * error message to stderr and exits the program.
 *
 * @param thpool Pointer to the thread pool structure.
 * @param thread Pointer to the thread to be added.
 */
void addThread(thread_pool *thpool, pthread_t *thread) {
    thpool->num_threads++;
    thpool->pool = realloc(*thpool->pool, sizeof(pthread_t) * thpool->num_threads);
    if(thpool->pool == NULL) {
        fprintf(stderr, "Failed reallocating memory");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Shifts elements in the thread pool array to the left starting from a given index.
 *
 * This function takes a thread pool and an index, and shifts all elements in the pool array
 * to the left starting from the specified index. The element at the given index is overwritten
 * by the element at the next index, and so on, effectively removing the element at the given index.
 *
 * @param thpool A pointer to the thread pool structure.
 * @param idx The index from which to start shifting elements.
 */
void backCopy(thread_pool *thpool, int idx) {
    for (int i = idx; i < thpool->num_threads; i++) {
        thpool->pool[i] = thpool->pool[i+1];
    }
}

/**
 * @brief Ends a thread in the thread pool.
 *
 * This function cancels a specific thread in the thread pool, frees its memory,
 * and reallocates the pool to adjust the number of threads.
 *
 * @param thpool Pointer to the thread pool structure.
 * @param thread Pointer to the thread to be ended.
 *
 * @note This function modifies the thread pool by removing the specified thread.
 *       It also handles memory reallocation for the thread pool array.
 *       If memory reallocation fails, the function prints an error message and exits the program.
 */
void endThread(thread_pool *thpool, pthread_t *thread) {
    for (int i = 0; i < thpool->num_threads; i++) {
        if(pthread_equal(*(thpool->pool[i]), *thread)) {
            pthread_cancel(*thread);
            free(thread);
            thpool->num_threads--;
            backCopy(thpool, i);
            thpool->pool = realloc(thpool->pool, sizeof(pthread_t*) * thpool->num_threads);
            if(thpool->pool == NULL) {
                fprintf(stderr, "Failed reallocating memory");
                exit(EXIT_FAILURE);
            }
        }
    }
}

/**
 * @brief Terminates all threads in the thread pool.
 *
 * This function iterates through all the threads in the thread pool and cancels each one.
 * After all threads are cancelled, the number of threads in the pool is set to 0.
 *
 * @param thpool Pointer to the thread pool structure.
 */
void endAllThreads(thread_pool *thpool) {
    for (int i = 0; i < thpool->num_threads; i++) {
        pthread_cancel(*thpool->pool[i]);
    }
    thpool->num_threads = 0;
}