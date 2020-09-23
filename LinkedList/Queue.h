#ifndef __QUEUE_H__
#define __QUEUE_H__
#include "LinkedList.h"
struct Queue {
	struct LinkedList* list;
};

struct Queue* Queue_new();
void Queue_free(struct Queue* q);
size_t Queue_size(struct Queue* q);
void Queue_set_safe(struct Queue* q, char safe);
void Queue_offer(struct Queue* q, void* val);
void* Queue_poll(struct Queue* q);

#endif
