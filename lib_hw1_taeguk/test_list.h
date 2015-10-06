#ifndef __LIB_KERNEL_TEST_LIST_H
#define __LIB_KERNEL_TEST_LIST_H

#include "list.h"
#include "testlib.h"

struct ListItem {
	struct list_elem elem;
	int data;
};

bool l_less_func(const struct list_elem *a, const struct list_elem *b, void *aux);

bool wrap_list_insert(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_splice(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_push(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_push_front(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_push_back(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_remove(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_pop_front(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_pop_back(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_front(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_back(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_size(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_empty(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_reverse(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_sort(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_insert_ordered(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_unique(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_max(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_list_min(struct Request *req, struct WrapDataStructure *wds[]);

#endif
