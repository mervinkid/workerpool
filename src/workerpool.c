/*
 * Worker pool
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Mervin <mofei2816@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "workerpool.h"

static void workerpool_workerthread_func(void *);
static void workerthread_init(workerthread_t *);
static void workerthread_join(workerthread_t * __restrict, uint);

workerpool_t* workerpool_new() {
    return (workerpool_t*)malloc(sizeof(workerpool_t));
}

void workerpool_init(workerpool_t * pool, uint poolsize) {
    
    // Init pool only when pool has not been inited.
    if (workerpool_status(pool) != INVALID) {
        return;
    }

    poolsize = poolsize > MAX_WORKERPOOL_SIZE ? MAX_WORKERPOOL_SIZE : poolsize;
    
    pool->worker_threads = NULL;
    
    // Init pool mutex lock.
    // This lock used for make pool status operation thread safe.
    pthread_mutex_t *pool_mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(pool_mutex, NULL);
    pool->poolsafe.pool_mutex = pool_mutex;
    
    // Init task queue mutex lock.
    // This lock used for make task queue thread safe.
    pthread_mutex_t *taskqueue_mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(taskqueue_mutex, NULL);
    pool->poolsafe.taskqueue_mutex = taskqueue_mutex;
    
    // Init worker condition mutex lock.
    // This lock used for make worker condition safe.
    pthread_mutex_t *worker_cond_mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(worker_cond_mutex, NULL);
    pool->poolsafe.worker_notify_mutex = worker_cond_mutex;
    
    // Init worker condition.
    // This condition used for make worker thread manageable.
    pthread_cond_t *worker_notify = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
    pthread_cond_init(worker_notify, NULL);
    pool->poolsafe.worker_notify = worker_notify;
    
    // Update pool status.
    pool->poolsafe.pool_status = STOP;
    
    // Init task queue.
    pool->taskqueue = taskqueue_new();
    taskqueue_init(pool->taskqueue);
    
    pool->poolsize = poolsize;
    
    return;
}

void workerpool_destroy(workerpool_t *pool) {
    
    if (workerpool_status(pool) == INVALID) {
        // Pool is invalid.
        return;
    }
    
    // Stop pool.
    workerpool_stop(pool);
    
    // Destroy lock and condition.
    pthread_mutex_destroy(pool->poolsafe.pool_mutex);
    pthread_mutex_destroy(pool->poolsafe.worker_notify_mutex);
    pthread_mutex_destroy(pool->poolsafe.taskqueue_mutex);
    pthread_cond_destroy(pool->poolsafe.worker_notify);
    
    // Free memory.
    free(pool->worker_threads);
    free(pool->taskqueue);
    free(pool);
    pool = NULL;
    
    return;
}

int workerpool_start(workerpool_t *pool) {
    
    if (workerpool_status(pool) == INVALID) {
        // Pool is invalid.
        return -1;
    }
    
    // Lock
    pthread_mutex_lock(pool->poolsafe.pool_mutex);
    
    uint pool_status = pool->poolsafe.pool_status;
    if (pool_status == RUNNING) {
        return 0;
    }
    
    // Update pool status
    pool->poolsafe.pool_status = RUNNING;
    
    // Init and start work threads
    pool->worker_threads = (workerthread_t*)malloc(sizeof(workerthread_t) * pool->poolsize);
    for (uint i = 0; i < pool->poolsize; i++) {
        workerthread_init(pool->worker_threads+i);
        pthread_create((pool->worker_threads+i)->thread, NULL, (void*)workerpool_workerthread_func, (void*)pool);
        DEBUG_INFO("[INFO] worker -%10d - start.\n", (int)*(pool->worker_threads+i)->thread);
    }
    
    // Unlock
    pthread_mutex_unlock(pool->poolsafe.pool_mutex);
    
    return 0;
}

int workerpool_stop(workerpool_t* pool) {
    
    if (workerpool_status(pool) == INVALID) {
        return -1;
    }
    
    uint pool_status = pool->poolsafe.pool_status;
    if (pool_status == STOP || pool_status == PAUSE) {
        return 0;
    }
    
    // Lock
    pthread_mutex_lock(pool->poolsafe.pool_mutex);
    
    // Update pool status.
    // Set pool status from RUNNING to STOP.
    pool->poolsafe.pool_status = STOP;
    
    // Notify workers
    pthread_mutex_lock(pool->poolsafe.worker_notify_mutex);
    pthread_cond_broadcast(pool->poolsafe.worker_notify);
    pthread_mutex_unlock(pool->poolsafe.worker_notify_mutex);
    
    // Wait for worker threads response
    workerthread_join(pool->worker_threads, pool->poolsize);
    free(pool->worker_threads);
    pool->worker_threads = NULL;
    
    // Unlock
    pthread_mutex_unlock(pool->poolsafe.pool_mutex);
    
    return 0;
}

int workerpool_pause(workerpool_t *pool) {
    
    if (workerpool_status(pool) == INVALID) {
        return -1;
    }
    
    uint pool_status = pool->poolsafe.pool_status;
    
    if (pool_status == PAUSE) {
        return 0;
    }
    
    if (pool_status != RUNNING) {
        return -1;
    }
    
    // Lock
    pthread_mutex_lock(pool->poolsafe.pool_mutex);
    
    // Update pool status.
    // Set pool status from RUNNING to PAUSE.
    pool->poolsafe.pool_status = PAUSE;
    
    // Notify workers.
    pthread_mutex_lock(pool->poolsafe.worker_notify_mutex);
    pthread_cond_broadcast(pool->poolsafe.worker_notify);
    pthread_mutex_unlock(pool->poolsafe.worker_notify_mutex);
    
    // Wait for worker threads response.
    workerthread_join(pool->worker_threads, pool->poolsize);
    free(pool->worker_threads);
    pool->worker_threads = NULL;
    
    // Unlock
    pthread_mutex_unlock(pool->poolsafe.pool_mutex);
    
    return 0;
}

int workerpool_task_put(workerpool_t *pool, void (*taskfunc)(void*), void *arg) {
    
    if (workerpool_status(pool) == INVALID || taskfunc == NULL) {
        return -1;
    }
    if (taskqueue_put(pool->taskqueue, taskfunc, arg) == -1) {
        return -1;
    }
    
    // Notify workers
    pthread_mutex_lock(pool->poolsafe.worker_notify_mutex);
    pthread_cond_broadcast(pool->poolsafe.worker_notify);
    pthread_mutex_unlock(pool->poolsafe.worker_notify_mutex);
    return 0;
}

/*
 * Return pool size
 */
uint workerpool_poolsize(workerpool_t *pool) {
    
    if (workerpool_status(pool) == INVALID) {
        return 0;
    }
    return pool->poolsize;
}

/*
 * Update pool size
 */
int workerpool_poolsize_update(workerpool_t *pool, uint size) {
    
    uint pool_status = workerpool_status(pool);
    
    if (pool_status == INVALID) {
        return -1;
    }
    
    // Return when new size is eq with current size.
    if (size == pool->poolsize) {
        return 0;
    }
    
    if (pool_status == RUNNING) {
        workerpool_pause(pool);
    }
    pool->poolsize = size;
    if (pool_status == RUNNING) {
        workerpool_start(pool);
    }
    return 0;
}

/*
 * Return current status of pool
 */
pool_status_t workerpool_status(workerpool_t *pool) {
    
    if (pool == NULL) {
        return INVALID;
    }
    pool_status_t current = pool->poolsafe.pool_status;
    if (current != INVALID  &&
        current != RUNNING  &&
        current != STOP     &&
        current != PAUSE) {
        pool->poolsafe.pool_status = INVALID;
    }
    return pool->poolsafe.pool_status ;
}

/*
 * Worker thread function
 */
static void workerpool_workerthread_func(void *ptr) {
    
    workerpool_t *pool = (workerpool_t*)ptr;
    if (workerpool_status(pool) == INVALID) {
        return;
    }
    // Worker loop
    while (1) {
        
        // If pool have been set to pause,
        // break worker loop and finish current thread immediately.
        if (pool->poolsafe.pool_status == PAUSE) {
            break;
        }
        
        // Load task from queue.
        pthread_mutex_lock(pool->poolsafe.taskqueue_mutex);
        DEBUG_INFO("[INFO] worker -%10d - load task.\n", (int)pthread_self());
        task_t task;
        int r = taskqueue_take(pool->taskqueue, &task);
        pthread_mutex_unlock(pool->poolsafe.taskqueue_mutex);
        
        if (r == -1) {
            
            // If pool have been set to stop,
            // break worker loop and finish current thread then task queue is empty.
            pthread_mutex_lock(pool->poolsafe.worker_notify_mutex);
            if (pool->poolsafe.pool_status == STOP) {
                pthread_mutex_unlock(pool->poolsafe.worker_notify_mutex);
                break;
            }
            
            DEBUG_INFO("[INFO] worker -%10d - wait.\n", (int)pthread_self());

            pthread_cond_wait(pool->poolsafe.worker_notify, pool->poolsafe.worker_notify_mutex);
            
            pthread_mutex_unlock(pool->poolsafe.worker_notify_mutex);
            
            DEBUG_INFO("[INFO] worker -%10d - weakup.\n", (int)pthread_self());
            
            continue;
        }
        void (*task_func)(void*) = task.func;
        void *task_arg = task.args;
        task_func(task_arg);
    }
    DEBUG_INFO("[INFO] worker -%10d - finish.\n", (int)pthread_self());
    return;
}

static void workerthread_init(workerthread_t *workerthread) {
    
    if (workerthread == NULL) {
        return;
    }
    workerthread->thread = (pthread_t*)malloc(sizeof(pthread_t));
}

static void workerthread_join(workerthread_t *workerthreads, uint size) {
    
    for (uint i = 0; i < size; i++) {
        pthread_t *curosr = ((workerthreads+i)->thread);
        pthread_join(*curosr, NULL);
        DEBUG_INFO("[INFO] worker -%10d - stop.\n", (int)*curosr);
        free((workerthreads+i)->thread);
    }
}