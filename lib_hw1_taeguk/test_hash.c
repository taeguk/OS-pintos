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
