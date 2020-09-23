#include "Queue.h"
//#include "LinkedList.h"

struct Queue* Queue_new() {
	struct Queue* q = malloc(sizeof(struct Queue));
	q->list = LinkedList_new();
	return q;
}

void Queue_free(struct Queue* q) {
	LinkedList_free(q->list, 1);
	free(q);
}

size_t Queue_size(struct Queue* q) {
	return q->list->size;
}

void Queue_set_safe(struct Queue* q, char safe) {
	LinkedList_set_safe(q->list, safe);
}

void Queue_offer(struct Queue* q, void* val) {
	LinkedList_add_back(q->list, val);
}

void* Queue_poll(struct Queue* q) {
	return LinkedList_remove_front(q->list);
}
