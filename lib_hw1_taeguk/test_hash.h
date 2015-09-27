#ifndef __LIB_KERNEL_TEST_HASH_H
#define __LIB_KERNEL_TEST_HASH_H

#include "hash.h"

struct HashItem {
	struct hash_elem elem;
	int data;
};

unsigned h_hash_func(const struct hash_elem *e, void *aux);
bool h_less_func(const struct hash_elem *a, const struct hash_elem *b, void *aux);

#endif
