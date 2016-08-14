//
//  main.c
//  wotacher
//
//  Created by Mervin Zhang on 8/10/16.
//  Copyright © 2016 Mervin Zhang. All rights reserved.
//

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
