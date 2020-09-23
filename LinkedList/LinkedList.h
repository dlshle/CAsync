#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__
#include <pthread.h>

struct ListNode {
	void* val;
	struct ListNode* next;
};

struct LinkedList {
	size_t size;
	char safe;
	struct ListNode* head;
	struct ListNode* tail;
	pthread_mutex_t lock;
};

struct LinkedList* LinkedList_new();
void LinkedList_free(struct LinkedList* list, int free_node);
void LinkedList_add_front(struct LinkedList* list, void* val);
void LinkedList_add_back(struct LinkedList* list, void* val);
void* LinkedList_remove_front(struct LinkedList* list);
void* LinkedList_remove_back(struct LinkedList* list);

int LinkedList_set_safe(struct LinkedList* list, char safe);
void LinkedList_dump(struct LinkedList* list);
#endif
