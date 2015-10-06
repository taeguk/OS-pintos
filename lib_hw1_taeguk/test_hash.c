#include <stdio.h>
#include "testlib.h"
#include "test_hash.h"
#include "hash.h"

unsigned h_hash_func(const struct hash_elem *e, void *aux)
{
	struct HashItem *item = hash_entry(e, struct HashItem, elem);
	return hash_int(item->data);
}

bool h_less_func(const struct hash_elem *a, const struct hash_elem *b, void *aux)
{
	unsigned h1 = h_hash_func(a, aux);
	unsigned h2 = h_hash_func(b, aux);

	return h1 < h2;
}

void h_free_func (struct hash_elem *e, void *aux)
{
	struct HashItem *item = hash_entry(e, struct HashItem, elem);
	free(item);
}

void h_square_func (struct hash_elem *e, void *aux)
{
	struct HashItem *item = hash_entry(e, struct HashItem, elem);
	item->data *= item->data;
}

void h_triple_func (struct hash_elem *e, void *aux)
{
	struct HashItem *item = hash_entry(e, struct HashItem, elem);
	item->data = item->data * item->data * item->data;
}

bool wrap_hash_insert(struct Request *req, struct WrapDataStructure *wds[])
{
	struct hash *h = wds[0]->ds;
	int val = atoi(req->token[2]);
	struct HashItem *new_item;
	struct hash_elem *old;

	new_item = malloc(sizeof(struct HashItem));
	new_item->data = val;

	if(old = hash_insert(h, (struct hash_elem*) new_item)) {
		free(new_item);
	}

	return true;
}

bool wrap_hash_replace(struct Request *req, struct WrapDataStructure *wds[])
{
	struct hash *h = wds[0]->ds;
	int val = atoi(req->token[2]);
	struct HashItem *new_item;
	struct hash_elem *old;

	new_item = malloc(sizeof(struct HashItem));
	new_item->data = val;

	if(old = hash_insert(h, (struct hash_elem*) new_item)) {
		struct HashItem *old_item = hash_entry(old, struct HashItem, elem);
		free(old_item);
	}

	return true;
}

bool wrap_hash_find(struct Request *req, struct WrapDataStructure *wds[])
{
	struct hash *h = wds[0]->ds;
	int val = atoi(req->token[2]);
	struct HashItem *trgt_item;
	struct hash_elem *res;

	trgt_item = malloc(sizeof(struct HashItem));
	trgt_item->data = val;

	if(res = hash_find(h, (struct hash_elem*) trgt_item)) {
		struct HashItem *item = hash_entry(res, struct HashItem, elem);
		fprintf(stdout, "%d\n", item->data);
	}

	free(trgt_item);

	return true;
}

bool wrap_hash_delete(struct Request *req, struct WrapDataStructure *wds[])
{
	struct hash *h = wds[0]->ds;
	int val = atoi(req->token[2]);
	struct HashItem *trgt_item;
	struct hash_elem *res;

	trgt_item = malloc(sizeof(struct HashItem));
	trgt_item->data = val;
	
	if(res = hash_delete(h, (struct hash_elem*) trgt_item)) {
		struct HashItem *item = hash_entry(res, struct HashItem, elem);
		free(item);
	}

	free(trgt_item);

	return true;
}

bool wrap_hash_clear(struct Request *req, struct WrapDataStructure *wds[])
{
	struct hash *h = wds[0]->ds;

	hash_clear(h, h_free_func);

	return true;
}

bool wrap_hash_size(struct Request *req, struct WrapDataStructure *wds[])
{
	struct hash *h = wds[0]->ds;
	size_t size;

	size = hash_size(h);

	fprintf(stdout, "%u\n", size);

	return true;
}

bool wrap_hash_empty(struct Request *req, struct WrapDataStructure *wds[])
{
	struct hash *h = wds[0]->ds;
	bool emptyFlag;

	emptyFlag = hash_empty(h);

	fprintf(stdout, "%s\n", (emptyFlag ? "true" : "false"));

	return true;
}

bool wrap_hash_apply(struct Request *req, struct WrapDataStructure *wds[])
{
	struct hash *h = wds[0]->ds;
	hash_action_func *action_func = NULL;

	if(strcmp(req->token[2], "square") == 0) {
		action_func = h_square_func;
	}
	else if(strcmp(req->token[2], "triple") == 0) {
		action_func = h_triple_func;
	}

	if(action_func) {
		hash_apply(h, action_func);
	}

	return true;
}
