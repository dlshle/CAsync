#ifndef __ASYNC_H__
#define __ASYNC_H__
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include "./LinkedList/Queue.h"

#define MAX_POOL_SIZE 1024
#define VERBOSE 0

enum AsyncTaskState {
	NOT_STARTED = 0,
	RUNNING,
	DONE
};

typedef void* Task(void* arg);

struct AsyncTask {
	size_t id;
	pthread_t thread;
	Task* task;
	void* arg;
	void* result;
	enum AsyncTaskState state;
	// encapsulation
	void* pool;
};

// TEMP

void Start_Task(struct AsyncTask* t);
struct AsyncTask* New_Task(Task* task, void* arg);

struct TaskNode {
	struct AsyncTask* task;
	struct TaskNode* next;
};

struct TaskQueue {
	struct TaskNode* head;
	struct TaskNode* tail;
	size_t size;
	pthread_mutex_t lock;
};

enum TaskPoolState {
	P_NOT_STARTED = 0,
	P_IDLE,
	P_BUSY,
	P_DONE
};

enum TaskPoolStatus {
  P_RUNNING = 0,
  P_STOP
};

struct TaskPool {
	size_t num_workers;
	struct AsyncTask** workers;
	pthread_mutex_t lock;
	sem_t jobs_sem;
	sem_t waiting_sem;
	struct Queue* q;
	volatile enum TaskPoolState state;
  volatile enum TaskPoolStatus status;
};

struct TaskPool* pool_init(unsigned int pool_size);
void pool_free(struct TaskPool* p, int wait);
struct AsyncTask* pool_schedule(struct TaskPool* p, Task* task, void* arg);
void pool_wait(struct TaskPool* p);
char* pool_dump_str(struct TaskPool* p);
void pool_dump(struct TaskPool* p);
char* pool_status(struct TaskPool* p);

#endif
