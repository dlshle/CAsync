#include "LinkedList.h"
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

void test_list_set_safe() {
	printf("Test list set safe");
	struct LinkedList* list = LinkedList_new();
	LinkedList_set_safe(list, 0);
	assertChar("Is list safe off", list->safe, 0);
	LinkedList_set_safe(list, 1);
	assertChar("Is list safe on", list->safe, 1);
	LinkedList_set_safe(list, 0);
	assertChar("Is list safe off", list->safe, 0);
	LinkedList_free(list, 1);
}

void test_list_add_front() {
	printf("Test list add front\n");
	int testInts[] = {1,2,3,4,5,6,7,8,9};
	struct LinkedList* list = LinkedList_new();
	LinkedList_add_front(list, &testInts[0]);
	assertInt("Is list of size 1", list->size, 1);
	assertInt("Is list head val equals testInts[0]", *((int*)(list->head->val)), testInts[0]);
	LinkedList_add_front(list, &testInts[1]);
	assertInt("Is list of size 2", list->size, 2);
	assertInt("Is list head val equals testInts[1]", *((int*)(list->head->val)), testInts[1]);
	LinkedList_add_front(list, &testInts[2]);
	assertInt("Is list of size 3", list->size, 3);
	assertInt("Is list head val equals testInts[2]", *((int*)(list->head->val)), testInts[2]);
	LinkedList_dump(list);
	LinkedList_free(list, 1);
}

void test_list_add_back() {
	printf("Test list add back\n");
	int testInts[] = {1,2,3,4,5,6,7,8,9};
	struct LinkedList* list = LinkedList_new();
	LinkedList_add_back(list, &testInts[0]);
	assertInt("Is list of size 1", list->size, 1);
	assertInt("Is list tail val equals testInts[0]", *((int*)(list->tail->val)), testInts[0]);
	LinkedList_add_back(list, &testInts[1]);
	assertInt("Is list of size 2", list->size, 2);
	assertInt("Is list tail val equals testInts[0]", *((int*)(list->tail->val)), testInts[1]);
	LinkedList_add_back(list, &testInts[2]);
	assertInt("Is list of size 3", list->size, 3);
	assertInt("Is list tail val equals testInts[0]", *((int*)(list->tail->val)), testInts[2]);
	LinkedList_dump(list);
	LinkedList_free(list, 1);
}

void test_list_remove_front() {
	printf("Test list remove front\n");
	int testInts[] = {1,2,3,4,5,6,7,8,9};
	struct LinkedList* list = LinkedList_new();
	LinkedList_add_front(list, &testInts[0]);
	assertInt("Is list of size 1", list->size, 1);
	assertInt("Is list head val equals testInts[0]", *((int*)(list->head->val)), testInts[0]);
	LinkedList_add_front(list, &testInts[1]);
	assertInt("Is list of size 2", list->size, 2);
	assertInt("Is list head val equals testInts[1]", *((int*)(list->head->val)), testInts[1]);
	LinkedList_add_front(list, &testInts[2]);
	assertInt("Is list of size 3", list->size, 3);
	assertInt("Is list head val equals testInts[2]", *((int*)(list->head->val)), testInts[2]);
	LinkedList_dump(list);

	int val = *((int*)LinkedList_remove_front(list));
	assertInt("Is the first remove val is testInts[2]", val, testInts[2]);
	assertInt("Is list of size 2", list->size, 2);
	val = *((int*)LinkedList_remove_front(list));
	assertInt("Is the second remove val is testInts[1]", val, testInts[1]);
	assertInt("Is list of size 1", list->size, 1);
	val = *((int*)LinkedList_remove_front(list));
	assertInt("Is the third remove val is testInts[0]", val, testInts[0]);
	assertInt("Is list of size 0", list->size, 0);

	void* empty = LinkedList_remove_front(list);
	assertPointerAddr("Is the last removed pointer null", empty, 0);

	LinkedList_add_front(list, &testInts[0]);
	assertInt("Is list of size 1", list->size, 1);
	assertInt("Is list head val equals testInts[0]", *((int*)(list->head->val)), testInts[0]);
	val = *((int*)LinkedList_remove_front(list));
	assertInt("Is the first remove val is testInts[0]", val, testInts[0]);
	assertInt("Is list of size 2", list->size, 0);
	
	LinkedList_free(list, 1);
}

int main(int argc, char** argv) {
	test_list_set_safe();
	test_list_add_front();
	test_list_add_back();
	test_list_remove_front();
	return 0;
}
