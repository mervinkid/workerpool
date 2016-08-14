# workerpool

![Build](https://img.shields.io/badge/build-passing-brightgreen.svg?style=flat)
![License MIT](https://img.shields.io/badge/license-MIT-blue.svg?style=flat&maxAge=2592000)

```
                    _                               _ 
__      _____  _ __| | _____ _ __ _ __   ___   ___ | |
\ \ /\ / / _ \| '__| |/ / _ \ '__| '_ \ / _ \ / _ \| |
 \ V  V / (_) | |  |   <  __/ |  | |_) | (_) | (_) | |
  \_/\_/ \___/|_|  |_|\_\___|_|  | .__/ \___/ \___/|_|
                                 |_|                 
```

Workerpool is a excellent worker thread dispatcher powered by C language.<br> 
It provides thread pool and task queue. <br>
There is no any global variable. 

## Usage

### Get start

Workerpool use `GNU make` for project management.

- Compile, build and test
```bash
$ make 
```

- Install to system
```bash
$ make install
```

- Cleanup build output
```bash
$ make clean
```

- Uninstall from system
```bash
$ make uninstall
```

### Start coding

1. Install to your system or copy the files under the `src` directory to your project.
2. Include the header file `workerpool.h`.

### Types

**Type `workerpool_t`**
```C
typedef struct workerpool_s {
    taskqueue_t *taskqueue;
    poolsafe_t poolsafe;
    uint poolsize;                      /* pool size */
    workerthread_t *worker_threads;     /* workers */
} workerpool_t; // worker pool
```
**Note:**
This is the major structure for `workerpool`. Do not modify any data directly.

### APIs

- `workerpool_t* workerpool_new();` 

    >Return an allocated pointer of type `workerpool_t`. 

- `void workerpool_init(workerpool_t * __restrict, uint);`

    >Init data for a allocated pointer of type `workerpool_t`.<br>
    >The first argument is the pointer of type `workerpool_t` which will be inited.<br>
    >The second argument is the number of worker threads.

- `void workerpool_destroy(workerpool_t * __restrict);`

    >Destroy a allocated pointer of type `workerpool_t`.

- `int  workerpool_start(workerpool_t * __restrict);`

    >Start a workerpool.
    >Workerpool will create worker threads and process task from task queue.<br>
    >The status of workerpool will be set to `POOL_STATUS_RUNNING`.

- `int  workerpool_pause(workerpool_t * __restrict);`

    >Pause a workerpool.<br>
    >It will stop all worker thread immediately and keep all pending task in task queue.<br>
    >The status of workerpool will be set to `POOL_STATUS_PAUSE`.

- `int  workerpool_stop(workerpool_t * __restrict);`

    >Stop a workerpool.<br>
    >It will stop all worker thread when all task in task queue have been processed.<br>
    >The status of workerpool will be set to `POOL_STATUS_STOP`.

- `int  workerpool_task_put(workerpool_t * __restrict, void (*)(void*), void*);`

    >Put a task function to workerpool.

- `uint workerpool_poolsize(workerpool_t * __restrict);`

    >Return the pool size (number of worker thread) of workerpool.

- `int workerpool_poolsize_update(workerpool_t * __restrict, uint);`

    >Modify the pool size of workerpool.<br>
    >It will stop all worker thread immediately then update the pool size of specified workerpool.<br>
    >After it done. It will restart the workerpool with new pool size.

- `uint workerpool_status(workerpool_t * __restrict);`
 
    >Return the current status of specified workerpool pointer.

### Sample
```C
#include <workerpool.h>

static void *sample_task(void *);

int main(int argc, const char* argv) {
    // create a workerpool
    workerpool_t *pool = workerpool_new();
    // init the pool with 4 workers
    workerpool_init(pool, 4);
    // start the pool
    workerpool_start(pool);
    // put task to the pool
    workerpool_task_put(pool, (void (*)(void*))sample_task, NULL);
    // pause the pool
    workerpool_pause(pool);
    // modify pool size from 4 to 8
    workerpool_poolsize_update(pool, 8);
    // stop the pool
    workerpool_stop(pool);
    // destroy the pool
    workerpool_destroy(pool);
}

static void *sample_task(void *arg) {
    // put your task code here.
    return NULL;
}
```

## Contributing

1. Fork it.
2. Create your feature branch. (`$ git checkout feature/my-feature-branch`)
3. Commit your changes. (`$ git commit -am 'What feature I just added.'`)
4. Push to the branch. (`$ git push origin feature/my-feature-branch`)
5. Create a new Pull Request

## Authors

[@Mervin](https://github.com/mofei2816) 

## License

The MIT License (MIT). For detail see [LICENSE](LICENSE).

