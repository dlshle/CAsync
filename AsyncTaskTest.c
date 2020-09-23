#include "AsyncTask.h"
#include <stdio.h>
#include <stdlib.h>

void* test(void* arg) {
	printf("test function\n");
	printf("arg: %s\n", (char*)(arg));
}

int main(int argc, char** argv) {
	struct TaskPool* p = pool_init(3);
	struct AsyncTask* t = pool_schedule(p, test, "a");
	struct AsyncTask* t1 = pool_schedule(p, test, "2");
	struct AsyncTask* t2 = pool_schedule(p, test, "c");
	struct AsyncTask* t3 = pool_schedule(p, test, "4");
	struct AsyncTask* t4 = pool_schedule(p, test, "e");
	printf("pool status: %s\n", pool_status(p));
	// gotta wait before free
	pool_free(p, 1);
	return 0;
}
