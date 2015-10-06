#include <stdio.h>
#include <stdlib.h>
#include "testlib.h"
#include "test_list.h"
#include "list.h"

bool l_less_func(const struct list_elem *a, const struct list_elem *b, void *aux)
{
	struct ListItem *item1 = list_entry(a, struct ListItem, elem);
	struct ListItem *item2 = list_entry(b, struct ListItem, elem);
	return item1->data < item2->data;
}

bool wrap_list_insert(struct Request *req, struct WrapDataStructure *wds[])
{
	struct list *list = (struct list*) wds[0]->ds;
	int idx = atoi(req->token[2]);
	int val = atoi(req->token[3]);
	struct ListItem *new_item;
	struct list_elem *e;

	e = list_find_nth(list, idx);

	new_item = malloc(sizeof(struct ListItem));
	new_item->data = val;
	
	list_insert(e, (struct list_elem*) new_item);

	return true;
}

bool wrap_list_splice(struct Request *req, struct WrapDataStructure *wds[])
{
	struct list *list1 = (struct list*) wds[0]->ds;
	int idx1 = atoi(req->token[2]);
	struct list *list2 = (struct list*) wds[1]->ds;
	int idx2 = atoi(req->token[4]);
	int idx3 = atoi(req->token[5]);
	struct list_elem *e[3];

	e[0] = list_find_nth(list1,idx1);
	e[1] = list_find_nth(list2,idx2);
	e[2] = list_find_nth(list2,idx3);

	list_splice(e[0], e[1], e[2]);

	return true;
}

bool wrap_list_push(struct Request *req, struct WrapDataStructure *wds[])
{
	// i don't know what i do :(
	return wrap_list_push_front(req, wds);
}

bool wrap_list_push_front(struct Request *req, struct WrapDataStructure *wds[])
{
	struct list *list = (struct list*) wds[0]->ds;
	int val = atoi(req->token[2]);
	struct ListItem *new_item;

	new_item = malloc(sizeof(struct ListItem));
	new_item->data = val;
	
	list_push_front(list, (struct list_elem*) new_item);

	return true;
}

bool wrap_list_push_back(struct Request *req, struct WrapDataStructure *wds[])
{
	struct list *list = (struct list*) wds[0]->ds;
	int val = atoi(req->token[2]);
	struct ListItem *new_item;

	new_item = malloc(sizeof(struct ListItem));
	new_item->data = val;
	
	list_push_back(list, (struct list_elem*) new_item);

	return true;
}

bool wrap_list_remove(struct Request *req, struct WrapDataStructure *wds[])
{
	struct list *list = (struct list*) wds[0]->ds;
	int idx = atoi(req->token[2]);
	struct list_elem *e;

	e = list_find_nth(list, idx);

	list_remove(e);

	free(list_entry(e, struct ListItem, elem));

	return true;
}

bool wrap_list_pop_front(struct Request *req, struct WrapDataStructure *wds[])
{
	struct list *list = (struct list*) wds[0]->ds;
	struct list_elem *e;

	e = list_pop_front(list);
	
	free(list_entry(e, struct ListItem, elem));

	return true;
}

bool wrap_list_pop_back(struct Request *req, struct WrapDataStructure *wds[])
{
	struct list *list = (struct list*) wds[0]->ds;
	struct list_elem *e;

	e = list_pop_back(list);

	free(list_entry(e, struct ListItem, elem));

	return true;
}

bool wrap_list_front(struct Request *req, struct WrapDataStructure *wds[])
{	
	struct list *list = (struct list*) wds[0]->ds;
	struct list_elem *e;
	struct ListItem *item;

	e = list_front(list);
	item = list_entry(e, struct ListItem, elem);

	fprintf(stdout, "%d\n", item->data);

	return true;
}

bool wrap_list_back(struct Request *req, struct WrapDataStructure *wds[])
{	
	struct list *list = (struct list*) wds[0]->ds;
	struct list_elem *e;
	struct ListItem *item;

	e = list_back(list);
	item = list_entry(e, struct ListItem, elem);

	fprintf(stdout, "%d\n", item->data);

	return true;
}

bool wrap_list_size(struct Request *req, struct WrapDataStructure *wds[])
{
	struct list *list = (struct list*) wds[0]->ds;
	size_t size;

	size = list_size(list);

	fprintf(stdout, "%u\n", size);

	return true;
}

bool wrap_list_empty(struct Request *req, struct WrapDataStructure *wds[])
{
	struct list *list = (struct list*) wds[0]->ds;
	bool emptyFlag;

	emptyFlag = list_empty(list);

	fprintf(stdout, "%s\n", (emptyFlag ? "true" : "false"));

	return true;
}

bool wrap_list_reverse(struct Request *req, struct WrapDataStructure *wds[])
{
	struct list *list = (struct list*) wds[0]->ds;

	list_reverse(list);

	return true;
}

bool wrap_list_sort(struct Request *req, struct WrapDataStructure *wds[])
{
	struct list *list = (struct list*) wds[0]->ds;

	list_sort(list, l_less_func, NULL);

	return true;
}

bool wrap_list_insert_ordered(struct Request *req, struct WrapDataStructure *wds[])
{
	struct list *list = (struct list*) wds[0]->ds;
	int val = atoi(req->token[2]);
	struct ListItem *item;

	item = malloc(sizeof(struct ListItem));
	item->data = val;

	list_insert_ordered(list, (struct list_elem*) item, l_less_func, NULL);

	return true;
}

bool wrap_list_unique(struct Request *req, struct WrapDataStructure *wds[])
{
	struct list *list1 = (struct list*) wds[0]->ds;
	struct list *list2 = (struct list*) wds[1]->ds;

	list_unique(list1, list2, l_less_func, NULL);

	return true;
}

bool wrap_list_max(struct Request *req, struct WrapDataStructure *wds[])
{
	struct list *list = (struct list*) wds[0]->ds;
	struct list_elem *e;
	struct ListItem *item;

	e = list_max(list, l_less_func, NULL);
	item = list_entry(e, struct ListItem, elem);

	fprintf(stdout, "%d\n", item->data);

	return true;
}

bool wrap_list_min(struct Request *req, struct WrapDataStructure *wds[])
{
	struct list *list = (struct list*) wds[0]->ds;
	struct list_elem *e;
	struct ListItem *item;

	e = list_min(list, l_less_func, NULL);
	item = list_entry(e, struct ListItem, elem);

	fprintf(stdout, "%d\n", item->data);

	return true;
}

