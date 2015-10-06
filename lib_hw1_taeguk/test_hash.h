#ifndef __LIB_KERNEL_TEST_HASH_H
#define __LIB_KERNEL_TEST_HASH_H

#include "hash.h"
#include "testlib.h"

struct HashItem {
	struct hash_elem elem;
	int data;
};

unsigned h_hash_func(const struct hash_elem *e, void *aux);
bool h_less_func(const struct hash_elem *a, const struct hash_elem *b, void *aux);
void h_free_func (struct hash_elem *e, void *aux);
void h_square_func (struct hash_elem *e, void *aux);
void h_triple_func (struct hash_elem *e, void *aux);
	
bool wrap_hash_insert(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_hash_replace(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_hash_find(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_hash_delete(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_hash_clear(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_hash_size(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_hash_empty(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_hash_apply(struct Request *req, struct WrapDataStructure *wds[]);

#endif
