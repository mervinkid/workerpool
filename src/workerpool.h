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

#ifndef WORKERPOOL_H_
#define WORKERPOOL_H_

#include <pthread.h>    /* POSIX threading */
#include <stdlib.h>     /* memory dynomic alloc */
#include <sys/types.h>  /* types */
#include <unistd.h>

#include "taskqueue.h"

#ifdef DEBUG
    #include <stdio.h>
    #define DEBUG_INFO(fmt, ...)    printf(fmt, __VA_ARGS__)
#else
    #define DEBUG_INFO(fmt, ...)
#endif

#define MAX_WORKERPOOL_SIZE     0xff

/* pool status */
typedef enum pool_status_e {
    INVALID,
    PAUSE,
    RUNNING,
    STOP
} pool_status_t;

/* struct and types */

typedef struct workerthread_s {
    pthread_t *thread;
} workerthread_t; // worker thread

typedef struct poolsafe_s {
    pthread_cond_t *worker_notify, *queue_notify;
    pthread_mutex_t *worker_notify_mutex, *queue_notify_mutex, *taskqueue_mutex, *pool_mutex;
    pool_status_t pool_status;
} poolsafe_t; // worker safe

typedef struct workerpool_s {
    taskqueue_t *taskqueue;
    poolsafe_t poolsafe;
    uint poolsize;                      /* pool size */
    uint buffersize;
    workerthread_t *worker_threads;     /* workers */
} workerpool_t; // worker pool

/* workerpool functions */

workerpool_t* workerpool_new();
void workerpool_init(workerpool_t * __restrict, uint, uint);
void workerpool_destroy(workerpool_t * __restrict);
int  workerpool_start(workerpool_t * __restrict);
int  workerpool_pause(workerpool_t * __restrict);
int  workerpool_stop(workerpool_t * __restrict);
int  workerpool_task_put(workerpool_t * __restrict, void (*)(void*), void*);
uint workerpool_poolsize(workerpool_t * __restrict);
int workerpool_poolsize_update(workerpool_t * __restrict, uint);
pool_status_t workerpool_status(workerpool_t * __restrict);

#endif /* WORKERPOOL_H_ */
