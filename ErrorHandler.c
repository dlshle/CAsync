#include <stdlib.h>
#include "ErrorHandler.h"

#define DEFAULT_REPORT_TYPE 0
#define DEFAULT_REPORTER(msg) printf("%s\n", msg)

void std_report(char* msg) {
	printf("%s\n", msg);
}

void perror_report(char* msg) {
	perror(msg);
}

char* error_codes[] = { "Logical error(0)", "Memory error(1)", "Thread error(2)", "Mutex error(3)", "Network error(4)", "Socket error(5)" };

reporter reporters[] = { std_report, perror_report };

char* _translate_error_code(int code) {
	return error_codes[code];
}

void _report(char* msg) {
  if (REPORT_TYPE < 0 || REPORT_TYPE > MAX_REPORT_TYPES) {
    DEFAULT_REPORTER(msg);
  }
  reporters[REPORT_TYPE](msg);
}

void report_error(char* msg, int code) {
	_report("ERROR!");
	_report(_translate_error_code(code));
	_report(msg);
	exit(1);
}
