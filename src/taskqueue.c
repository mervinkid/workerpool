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

#include "taskqueue.h"

taskqueue_t* taskqueue_new() {
    return (taskqueue_t*)malloc(sizeof(taskqueue_t));
}

/*
 * Init queue
 */
void taskqueue_init(taskqueue_t* queue) {

    if (queue == NULL) {
        return;
    }
    queue->size = 0;
    queue->first = NULL;
    queue->last = NULL;
    return;
}

/*
 * Put task into queue.
 */
int taskqueue_put(taskqueue_t *queue, void (*func)(void*), void *arg) {
    
    if (func == NULL || queue == NULL) {
        return -1;
    }
    
    task_t *task = (task_t*)malloc(sizeof(task_t));
    if (task == NULL) {
        return -1;
    }
    task->func = func;
    task->args = arg;
    
    tasknode_t *item = (tasknode_t*)malloc(sizeof(tasknode_t));
    item->task = task;
    item->next = NULL;
    
    if (queue->first == NULL) {
        queue->first = item;
        queue->last = item;
        queue->size = 1;
    } else {
        if (queue->last == NULL) {
            return -1;
        }
        queue->last->next = item;
        queue->last = item;
        queue->size++;
    }
    return queue->size;
}

/*
 * Take the task from first node and remove the node from queue.
 * Return 0 if success or -1.
 */
int taskqueue_take(taskqueue_t *queue, task_t *task) {
    
    if (queue == NULL || queue->first == NULL || queue->size == 0) {
        return -1;
    }

    task->func = queue->first->task->func;
    task->args = queue->first->task->args;
    tasknode_t *tmp = queue->first;
    queue->first = queue->first->next;
    tasknode_destory(tmp);
    
    if (queue->size > 0) {
        queue->size--;
    }
    if (queue->first == NULL) {
        queue->last = NULL;
    }
    return 0;
}

/*
 * Clear all task from queue.
 */
void taskqueue_clear(taskqueue_t *queue) {
    
    if (queue == NULL) {
        return;
    }
    
    tasknode_t *node_cursor = queue->first;
    
    while (node_cursor != NULL) {
        tasknode_t *tmp = node_cursor->next;
        tasknode_destory(node_cursor);
        node_cursor = tmp;
    }
    queue->last = NULL;
    queue->size = 0;
}

/*
 * Destroy queue.
 */
void taskqueue_destroy(taskqueue_t *queue) {
    
    if (queue == NULL) {
        return;
    }
    
    taskqueue_clear(queue);
    free(queue);
    queue = NULL;
}

void tasknode_destory(tasknode_t *node) {
    
    if (node == NULL) {
        return;
    }
    free(node);
}

void task_destroy(task_t *task) {
    
    if (task == NULL) {
        return;
    }
    free(task->args);
    free(task);
}

