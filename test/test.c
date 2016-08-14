/*
 * Test for workerpool
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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "workerpool.h"

#define WORKER  4

static void *task_func(void *);

int main() {
    
    printf("Test start.\n");
    
    workerpool_t *pool = workerpool_new();
    
    assert(workerpool_status(pool) == POOL_STATUS_INVALID);
    
    workerpool_init(pool, WORKER);
    
    assert(workerpool_poolsize(pool) == WORKER);
    assert(workerpool_status(pool) == POOL_STATUS_STOP);
    
    workerpool_start(pool);
    
    assert(workerpool_status(pool) == POOL_STATUS_RUNNING);
    
    for (int i = 0; i < 20; i++) {
        int *arg = (int*)malloc(sizeof(int));
        *arg = i;
        workerpool_task_put(pool, (void (*)(void*))task_func, (void*)arg);
    }
    
    workerpool_stop(pool);
    
    assert(workerpool_status(pool) == POOL_STATUS_STOP);
    
    printf("Test finish.\n");
    
    return 0;
}

static void *task_func(void *arg) {
    printf("thread %10d: task %4d.\n", (int)pthread_self(), (int)*(int*)arg);
    return NULL;
}
