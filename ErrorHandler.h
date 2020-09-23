#include <stdio.h>

#ifndef __ERROR_HANDLER_H__
#define __ERROR_HANDLER_H__

#define MAX_ERROR_CODE 5
#define LOGIC_ERR 0
#define MEM_ERR 1
#define THREAD_ERR 2
#define MUTEX_ERR 3
#define NETWORK_ERR 4
#define SOCKET_ERR 5

#define MAX_REPORT_TYPES 1
#define REPORT_TYPE 0
#define REPORT_PRINTF 0
#define REPORT_PERROR 1

// reporter is a function that takes a string and report
typedef void (*reporter)(char*);

void report_error(char* msg, int code);

#endif
