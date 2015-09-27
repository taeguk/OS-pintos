#include "test_list.h"
#include "list.h"

bool list_less_func(const struct list_elem *a, const struct list_elem *b, void *aux)
{
	struct ListItem *item1 = list_entry(a, struct ListItem, elem);
	struct ListItem *item2 = list_entry(b, struct ListItem, elem);
	return item1->data < item2->data;
}
