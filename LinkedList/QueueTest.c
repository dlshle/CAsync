#include "Queue.h"
#include <stdlib.h>
#include <stdio.h>

void assertInt(char* desc, int a, int b) {
	printf("\t%s\n\t\tasserting %d and %d: %s\n", desc, a, b, (a == b)?"PASS":"FAIL");
}

void assertChar(char* desc, char a, char b) {
	printf("\t%s\n\t\tasserting %c and %c: %s\n", desc, a, b, (a == b)?"PASS":"FAIL");
}

void assertPointerAddr(char* desc, void* a, void* b) {
	printf("\t%s\n\t\tasserting %p and %p: %s\n", desc, a, b, (a == b)?"PASS":"FAIL");
}

void test_q_set_safe() {
	printf("Test q set safe");
	struct Queue* q = Queue_new();
	Queue_set_safe(q, 0);
	assertChar("Is q safe off", q->list->safe, 0);
	Queue_set_safe(q, 1);
	assertChar("Is q safe on", q->list->safe, 1);
	Queue_set_safe(q, 0);
	assertChar("Is q safe off", q->list->safe, 0);
	Queue_free(q);
}

void test_q_offer() {
	printf("Test q offer\n");
	int testInts[] = {1,2,3,4,5,6,7,8,9};
	struct Queue* q = Queue_new();
	Queue_offer(q, &testInts[0]);
	assertInt("Is q of size 1", Queue_size(q), 1);
	assertInt("Is q tail val equals testInts[0]", *((int*)(q->list->tail->val)), testInts[0]);
	Queue_offer(q, &testInts[1]);
	assertInt("Is q of size 2", Queue_size(q), 2);
	assertInt("Is q tail val equals testInts[1]", *((int*)(q->list->tail->val)), testInts[1]);
	Queue_offer(q, &testInts[2]);
	assertInt("Is q of size 3", Queue_size(q), 3);
	assertInt("Is q tail val equals testInts[2]", *((int*)(q->list->tail->val)), testInts[2]);
	Queue_free(q);
}

void test_q_poll() {
	printf("Test q poll\n");
	int testInts[] = {1,2,3,4,5,6,7,8,9};
	struct Queue* q = Queue_new();
	Queue_offer(q, &testInts[0]);
	assertInt("Is q of size 1", Queue_size(q), 1);
	assertInt("Is q tail val equals testInts[0]", *((int*)(q->list->tail->val)), testInts[0]);
	Queue_offer(q, &testInts[1]);
	assertInt("Is q of size 2", Queue_size(q), 2);
	assertInt("Is q tail val equals testInts[1]", *((int*)(q->list->tail->val)), testInts[1]);
	Queue_offer(q, &testInts[2]);
	assertInt("Is q of size 3", Queue_size(q), 3);
	assertInt("Is q tail val equals testInts[2]", *((int*)(q->list->tail->val)), testInts[2]);

	int val = *((int*)Queue_poll(q));
	assertInt("Is the first remove val is testInts[2]", val, testInts[2]);
	assertInt("Is q of size 2", Queue_size(q), 2);
	val = *((int*)Queue_poll(q));
	assertInt("Is the second remove val is testInts[1]", val, testInts[1]);
	assertInt("Is q of size 1", Queue_size(q), 1);
	val = *((int*)Queue_poll(q));
	assertInt("Is the third remove val is testInts[0]", val, testInts[0]);
	assertInt("Is q of size 0", Queue_size(q), 0);

	void* empty = Queue_poll(q);
	assertPointerAddr("Is the last removed pointer null", empty, 0);

	Queue_offer(q, &testInts[0]);
	assertInt("Is q of size 1", Queue_size(q), 1);
	assertInt("Is q tail val equals testInts[0]", *((int*)(q->list->tail->val)), testInts[0]);
	val = *((int*)Queue_poll(q));
	assertInt("Is the first remove val is testInts[0]", val, testInts[0]);
	assertInt("Is q of size 2", Queue_size(q), 0);
	
	Queue_free(q);
}

int main(int argc, char** argv) {
	test_q_set_safe();
	test_q_offer();
	test_q_poll();
	return 0;
}
