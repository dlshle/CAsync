#include <stdio.h>
#include <stdlib.h>
//#include "ErrorHandler.h"
#include <pthread.h>
#include "AsyncTask.h"

void* work(void* arg) {
	printf("%s\n", (char*)arg);
	return (void*)"haha";
}

/*
void std_report(char* msg) {
	printf("%s\n", msg);
}

void perror_report(char *msg) {
	perror(msg);
}

typedef void (*reporter)(char*);

reporter reporters[2] = {std_report, perror_report};

void report_by_type(int type, char* msg) {
	reporters[type](msg);
}
*/

int main(int argc, char** argv) {
	Task* t = &work;
	//report_error("unknown operation", 0);
	pthread_t th;
	pthread_create(&th, NULL, work, "asd");
	pthread_join(th, NULL);

	struct AsyncTask* task = New_Task(t, "asd1");
	Start_Task(task);
	pthread_join(task->thread, NULL);
	printf("result: %s\n", (char*)task->result);
	return 0;
}
