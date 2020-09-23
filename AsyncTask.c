#include "ErrorHandler.h"
#include "AsyncTask.h"
#include <stdio.h>

// Lock
void _acquire_pool_lock(pthread_mutex_t* lock) {
  if (pthread_mutex_lock(lock) != 0) {
    report_error("ERROR acquiring lock", MUTEX_ERR);
  }
}

void _release_pool_lock(pthread_mutex_t* lock) {
  pthread_mutex_unlock(lock);
}

// Async task

// assume task has never been initiated before
void* _async_task_procedure(void* task) {
  struct AsyncTask* t = (struct AsyncTask*)task;
  if (!t) return 0;
  if (t->task != 0) {
    if (VERBOSE) printf("Async task %p running\n", t);
    t->state = RUNNING;
    t->result = (t->task)(t->arg);
  }
  if (VERBOSE) printf("Async task %p running\n", t);
  t->state = DONE;
}

struct AsyncTask* _init_async_task(size_t id, Task* task, void* arg) {
  struct AsyncTask* t = malloc(sizeof(struct AsyncTask));
  t->id = id;
  t->task = task;
  t->arg = arg;
  t->state = NOT_STARTED;
  if (VERBOSE) printf("Async task %p initiated\n", t);
  return t;
}

void _start_async_task(struct AsyncTask* t) {
  if (!t) return ;
  if (t->state != NOT_STARTED) {
    report_error("ERROR trying to start a STARTED Async Task", LOGIC_ERR);
  }
  if (pthread_create(&(t->thread), NULL, &_async_task_procedure, t) != 0) {
    report_error("ERROR unable to create thread for Async Task", THREAD_ERR);
  }
}

int _free_async_task(struct AsyncTask* task, int wait) {
  if (!task) return 0;
  if (task->state == RUNNING && wait) {
    pthread_join(task->thread, NULL);
  }
  free(task);
}

// Task pool

#define LOCK_POOL(p) _acquire_pool_lock(&(p->lock))
#define UNLOCK_POOL(p) _release_pool_lock(&(p->lock))

void *_worker_procedure(void* pool) {
  struct TaskPool* p = (struct TaskPool*)pool;
  void* job;
  int* sval;
  do {
    if (Queue_size(p->q) == 0) {
      LOCK_POOL(p);
      p->state = P_IDLE;
      sem_getvalue(&(p->waiting_sem), sval);
      if (*sval == 0) sem_post(&(p->waiting_sem));
      UNLOCK_POOL(p);
    }
    sem_wait(&(p->jobs_sem));
    // no need to acquire p lock as Queue has built-in lock
    job = Queue_poll(p->q);
    LOCK_POOL(p);
    p->state = P_BUSY;
    sem_getvalue(&(p->waiting_sem), sval);
    if (*sval == 1) sem_wait(&(p->waiting_sem));
    UNLOCK_POOL(p);
    _async_task_procedure(job);
  } while (p->state != P_DONE);
}

void _start_pool(struct TaskPool* p) {
  if (!p || p->state != P_NOT_STARTED) return ;
  LOCK_POOL(p);
  for (int i = 0; i < p->num_workers; i++) {
    _start_async_task(p->workers[i]);
  }
  p->state = P_BUSY;
  UNLOCK_POOL(p);
}

struct TaskPool* pool_init(unsigned int pool_size) {
  if (pool_size > MAX_POOL_SIZE) {
    pool_size = MAX_POOL_SIZE;
  }
  struct TaskPool* p = malloc(sizeof(struct TaskPool));
  p->num_workers = pool_size;
  p->workers = malloc(sizeof(struct AsyncTask*) * pool_size);
  for (int i = 0; i < pool_size; i++) {
    (p->workers)[i] = _init_async_task(1, _worker_procedure, p);
  }
  if (pthread_mutex_init(&(p->lock), NULL) != 0) {
	  report_error("ERROR initialize TaskPool lock.\n", MUTEX_ERR);
  }
  if (sem_init(&(p->jobs_sem), 0, 0) != 0) {
	  report_error("ERROR initialize TaskPool sem.\n", MUTEX_ERR);
  }
  if (sem_init(&(p->waiting_sem), 0, 1) != 0) {
	  report_error("ERROR initialize TaskPool sem.\n", MUTEX_ERR);
  }
  p->q = Queue_new();
  p->state = P_NOT_STARTED;
  return p;
}

void pool_free(struct TaskPool* p, int wait) {
  if (!p) return ;
  if (wait == 1) {
    pool_wait(p);
  }
  for (int i = 0; i < p->num_workers; i++) {
    _free_async_task(p->workers[i], 1);
  }
  free(p->workers);
  Queue_free(p->q);
  free(p);
}

struct AsyncTask* pool_schedule(struct TaskPool* p, Task* task, void* arg) {
	if (!(p && task)) return 0;
	LOCK_POOL(p);
	if (!(p->q)) p->q = Queue_new();
	Queue_offer(p->q, _init_async_task(1, task, arg));
  sem_post(&(p->jobs_sem));
	UNLOCK_POOL(p);
}

void pool_wait(struct TaskPool* p) {
  if (p->state == P_BUSY) {
    sem_wait(&(p->waiting_sem));
  }
  return ;
}

char* pool_dump_str(struct TaskPool* p) {
  return "";
}

void pool_dump(struct TaskPool* p) {
  return ;
}
