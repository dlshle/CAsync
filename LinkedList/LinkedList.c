#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include "LinkedList.h"

// Lock
void _acquire_lock(pthread_mutex_t* lock) {
  if (pthread_mutex_lock(lock) != 0) {
		printf("Error, unable to acquire mutex lock\n");
  }
}

void _release_lock(pthread_mutex_t* lock) {
  pthread_mutex_unlock(lock);
}

// Node
struct ListNode* _ListNode_new(void* val, struct ListNode* next) {
	struct ListNode* node = malloc(sizeof(struct ListNode));
	node->val = val;
	node->next = next;
	return node;
}

void _ListNode_free(struct ListNode* node, int r) {
	if (!node) return ;
	if (r && node->next) {
		_ListNode_free(node->next, r);
	}
	free(node);
}

// List
#define IS_0(x) x?1:0
#define LOCK(x) &(x->lock)

void _LinkedList_safe_check(struct LinkedList* list, char lock) {
	if (!list) return ;
	if (!list->safe) return ;
	if (lock) {
		_acquire_lock(&(list->lock));
	} else {
		_release_lock(&(list->lock));
	}
}

struct LinkedList* LinkedList_new() {
	struct LinkedList* list = malloc(sizeof(struct LinkedList));
	list->size = 0;
	list->safe = 1;
	if (pthread_mutex_init(&(list->lock), 0) != 0) {
		list->safe = 0;
		printf("%s\n", "LinkedList: unable to set safe mode");
	}
	return list;
}

void LinkedList_free(struct LinkedList* list, int free_node) {
	if (!list) return ;
	if (list->safe) {
		_acquire_lock(&(list->lock));
	}
	if (free_node) {
		_ListNode_free(list->head, 1);
	}
	if (list->safe) {
		_release_lock(&(list->lock));
	}
	LinkedList_set_safe(list, 0);
	free(list);
}

void LinkedList_add_front(struct LinkedList* list, void* val) {
	if (!list) return ;
	struct ListNode* previousHead;
	_LinkedList_safe_check(list, 1);
	if (list->size == 0) {
		list->head = _ListNode_new(val, 0);
		list->tail = list->head;
	} else if (list->size == 1) {
		list->head = _ListNode_new(val, 0);
		list->head->next = list->tail;
	} else {
		previousHead = list->head;
		list->head = _ListNode_new(val, previousHead);
	}
	list->size ++;
	_LinkedList_safe_check(list, 0);
}

void LinkedList_add_back(struct LinkedList* list, void* val) {
	if (!list) return ;
	_LinkedList_safe_check(list, 1);
	if (list->size == 0) {
		list->head = _ListNode_new(val, 0);
		list->tail = list->head;
	} else if (list->size == 1) {
		list->tail = _ListNode_new(val, 0);
		list->head->next = list->tail;
	} else {
		list->tail->next = _ListNode_new(val, 0);
		list->tail = list->tail->next;
	}
	list->size ++;
	_LinkedList_safe_check(list, 0);
}

void* LinkedList_remove_front(struct LinkedList* list) {
	if (!list) return 0;
	struct ListNode* front = list->head;
	void* res;
	_LinkedList_safe_check(list, 1);
	if (list->size == 0) {
		_LinkedList_safe_check(list, 0);
		return 0;
	} else if (list->size == 1) {
		list->tail = 0;
		list->head = 0;
		res = front->val;
		_ListNode_free(front, 0);
	} else {
		list->head = list->head->next;
		res = front->val;
		_ListNode_free(front, 0);
	}
	list->size--;
	_LinkedList_safe_check(list, 0);
	return res;
}

// try not to use...
void* LinkedList_remove_back(struct LinkedList* list) {
	if (!list) return 0;
	struct ListNode* tail = list->tail;
	void* res;
	_LinkedList_safe_check(list, 1);
	if (list->size == 0) {
		_LinkedList_safe_check(list, 0);
		return 0;
	} else if (list->size == 1) {
		list->tail = 0;
		list->head = 0;
		res = tail->val;
		_ListNode_free(tail, 0);
	} else {
		tail = list->head;
		for (int i = 0; i < (list->size - 1); i++) {
			tail = tail->next;
		}
		res = list->tail->val;
		list->tail = tail;
		_ListNode_free(tail, 0);
	}
	list->size--;
	_LinkedList_safe_check(list, 0);
	return res;
}

int LinkedList_set_safe(struct LinkedList* list, char safe) {
	if (!list) return 0;
	int error_code = 0;
	if (safe && !(list->safe)) {
		list->safe = 1;
		error_code = pthread_mutex_init(&(list->lock), 0);
		if (error_code != 0) {
			list->safe = 0;
			printf("LinkedList: unable to set safe mode to 1 error_code = %d\n", error_code);
			return 0;
		}
	} else if (!safe && list->safe) {
		_acquire_lock(&(list->lock));
		list->safe = 0;
		_release_lock(&(list->lock));
		error_code = pthread_mutex_destroy(&(list->lock));
		if (error_code != 0) {
			_acquire_lock(&(list->lock));
			list->safe = 1;
			_release_lock(&(list->lock));
			printf("LinkedList: unable to set safe mode to 0 error_code = %d\n", error_code);
			return 0;
		}
	}
	return 1;
}

void LinkedList_dump(struct LinkedList* list) {
  if (!list) return ;
  printf("LinkedList dump: {\n");
  printf("\tsize:%d\n\tsafe:%s\n\thead:%p\n\ttail:%p\n}\n", list->size, list->safe?"true":"false", list->head, list->tail);
}
