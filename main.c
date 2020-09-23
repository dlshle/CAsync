#include <stdio.h>
#include <stdlib.h>
#include "ErrorHandler.h"

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
	report_error("unknown operation", 0);
	return 0;
}
