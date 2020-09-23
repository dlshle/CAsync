#include "ErrorHandler.h"
#include "AsyncTask.h"
#include <stdio.h>
#include <sys/syscall.h>

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
	if (VERBOSE) printf("running async procedure %p(%d)\n", t, t->id);
  if (t->task != 0) {
    if (VERBOSE) printf("Async task %p(%d) running\n", t, t->id);
    t->state = RUNNING;
    t->result = (t->task)(t->arg);
  }
  if (VERBOSE) printf("Async task %p(%d) finished\n", t, t->id);
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

// TEMP
struct AsyncTask* New_Task(Task* task, void* arg) {
  return _init_async_task(1, task, arg);
}

void _start_async_task(struct AsyncTask* t) {
  if (!t) return ;
	if (VERBOSE) printf("Starting async task %p(%d)\n", t, t->id);
  if (t->state != NOT_STARTED) {
		printf("Async task %p(%d) has already started\n", t, t->id);
    report_error("ERROR trying to start a STARTED Async Task", LOGIC_ERR);
  }
  if (pthread_create(&(t->thread), NULL, &_async_task_procedure, t) != 0) {
		printf("Async task %p(%d) can not be created\n", t, t->id);
    report_error("ERROR unable to create thread for Async Task", THREAD_ERR);
  }
}

// TEMP
void Start_Task(struct AsyncTask* t) {
  _start_async_task(t);
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

void* _worker_procedure(void* pool) {
  struct TaskPool* p = (struct TaskPool*)pool;
  void* job;
  int* sval;
	if (VERBOSE) printf("worker report: worker is running on thread %d\n", (long int)syscall(224));
  do {
    if (Queue_size(p->q) == 0) {
			if (VERBOSE) printf("worker report: Queue size is 0\n");
      LOCK_POOL(p);
      p->state = P_IDLE;
      // sem_getvalue(&(p->waiting_sem), sval);
			// if (VERBOSE) printf("worker report: pool state is IDLE, sem value: %d\n", *sval);
      // if (*sval == 0) sem_post(&(p->waiting_sem));
      UNLOCK_POOL(p);
    }
    sem_wait(&(p->jobs_sem));
    // no need to acquire p lock as Queue has built-in lock
    job = Queue_poll(p->q);
		if (VERBOSE) printf("worker report: job acquired.\n");
    LOCK_POOL(p);
    p->state = P_BUSY;
    // sem_getvalue(&(p->waiting_sem), sval);
    // if (*sval == 1) sem_wait(&(p->waiting_sem));
		// if (VERBOSE) printf("worker report: sem value: %d\n", *sval);
    UNLOCK_POOL(p);
    _async_task_procedure(job);
		if (VERBOSE) printf("worker report: job done.\n");
  } while (p->status == P_RUNNING);
}

void _start_pool(struct TaskPool* p) {
  if (!p || p->state != P_NOT_STARTED) return ;
	if (VERBOSE) printf("starting pool...");
  for (int i = 0; i < p->num_workers; i++) {
		if (VERBOSE) printf("initiating worker %d(%d)\n", i, (p->workers)[i]->id);
    _start_async_task((p->workers)[i]);
		if (VERBOSE) printf("worker %d(%d) initiated\n", i, (p->workers)[i]->id);
  }
  p->state = P_BUSY;
}

struct TaskPool* pool_init(unsigned int pool_size) {
  if (pool_size > MAX_POOL_SIZE) {
    pool_size = MAX_POOL_SIZE;
  }
  struct TaskPool* p = malloc(sizeof(struct TaskPool));
  p->num_workers = pool_size;
  p->workers = malloc(sizeof(struct AsyncTask*) * pool_size);
  for (int i = 0; i < pool_size; i++) {
    (p->workers)[i] = _init_async_task(i, &_worker_procedure, (void*)p);
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
	_start_pool(p);
	p->status = P_RUNNING;
  return p;
}

void pool_free(struct TaskPool* p, int wait) {
  if (!p) return ;
	LOCK_POOL(p);
	p->status = P_STOP;
	p->state = P_DONE;
	UNLOCK_POOL(p);
  for (int i = 0; i < p->num_workers; i++) {
    _free_async_task(p->workers[i], wait);
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
		LOCK_POOL(p);
		p->state = P_DONE;
    if (VERBOSE) printf("pool_wait state is DONE\n");
		UNLOCK_POOL(p);
    if (VERBOSE) printf("pool_wait will join working threads\n");
		for (int i = 0; i < p->num_workers; i++) {
			pthread_join((p->workers)[i]->thread, NULL);
		}
  }
  return ;
}

char* pool_dump_str(struct TaskPool* p) {
  return "";
}

void pool_dump(struct TaskPool* p) {
  return ;
}

char* _pool_state_arr[] = {
  "NOT_STARTED",
  "IDLE",
  "BUSY",
  "DONE"
};

char* pool_state(struct TaskPool* p) {
  return _pool_state_arr[p->state];
}

char* pool_status(struct TaskPool* p) {
	return p->status == P_RUNNING?"RUNNING":"STOPPED";
}
