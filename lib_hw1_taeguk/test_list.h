#ifndef __LIB_KERNEL_TEST_LIST_H
#define __LIB_KERNEL_TEST_LIST_H

#include "list.h"

struct ListItem {
	struct list_elem elem;
	int data;
};

bool list_less_func(const struct list_elem *a, const struct list_elem *b, void *aux);

#endif
