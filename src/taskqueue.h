/*
 * Task queue
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

#ifndef TASKQUEUE_H_
#define TASKQUEUE_H_

#include <stdlib.h>

#define NEW_TASKQUEUE \
        (taskqueue_t*)malloc(sizeof(taskqueue_t))

/* struct and types  */

typedef struct task_s {
    void (*func)(void*);
    void *args;
} task_t; // task

typedef struct tasknode_s {
    task_t *task;
    struct tasknode_s *next;
} tasknode_t; // task node

typedef struct taskqueue_s {
    tasknode_t *first;
    tasknode_t *last;
    int size;
} taskqueue_t; // task queue

/* taskqueue functions */

taskqueue_t* taskqueue_new();
void taskqueue_init(taskqueue_t * __restrict);
int  taskqueue_put(taskqueue_t * __restrict, void (*)(void *), void *);
int  taskqueue_take(taskqueue_t * __restrict, task_t * __restrict);
void taskqueue_clear(taskqueue_t * __restrict);
void taskqueue_destroy(taskqueue_t * __restrict);
void tasknode_destory(tasknode_t * __restrict);
void task_destroy(task_t * __restrict);

#endif /* TASKQUEUE_H_ */
