#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "hash.h"
#include "bitmap.h"
#include "testlib.h"
#include "test_list.h"
#include "test_hash.h"
#include "test_bitmap.h"

static const char *command_list[] = 
{
	"list_insert", "list_splice", "list_push", "list_push_front", "list_push_back",
	"list_remove", "list_pop_front", "list_pop_back", "list_front", "list_back",
	"list_size", "list_empty",
	"list_reverse", "list_sort", "list_insert_ordered", "list_unique",
	"list_max", "list_min", "list_swap",

	"hash_insert", "hash_replace", "hash_find", "hash_delete",
	"hash_clear", "hash_size", "hash_empty", "hash_apply",

	"bitmap_size", "bitmap_set", "bitmap_mark", "bitmap_reset", "bitmap_flip", "bitmap_test",
	"bitmap_set_all", "bitmap_set_multiple", "bitmap_count", "bitmap_contains",
	"bitmap_any", "bitmap_none", "bitmap_all",
	"bitmap_scan", "bitmap_scan_and_flip", "bitmap_dump", "bitmap_expand"
};

static bool (*wrap_command[])(struct Request *, struct WrapDataStructure *[]) = 
{
	wrap_list_insert, wrap_list_splice, wrap_list_push, wrap_list_push_front, wrap_list_push_back,
	wrap_list_remove, wrap_list_pop_front, wrap_list_pop_back, wrap_list_front, wrap_list_back,
	wrap_list_size, wrap_list_empty,
	wrap_list_reverse, wrap_list_sort, wrap_list_insert_ordered, wrap_list_unique,
	wrap_list_max, wrap_list_min, wrap_list_swap,

	wrap_hash_insert, wrap_hash_replace, wrap_hash_find, wrap_hash_delete,
	wrap_hash_clear, wrap_hash_size, wrap_hash_empty, wrap_hash_apply,

	wrap_bitmap_size, wrap_bitmap_set, wrap_bitmap_mark, wrap_bitmap_reset, wrap_bitmap_flip, wrap_bitmap_test,
	wrap_bitmap_set_all, wrap_bitmap_set_multiple, wrap_bitmap_count, wrap_bitmap_contains,
	wrap_bitmap_any, wrap_bitmap_none, wrap_bitmap_all,
	wrap_bitmap_scan, wrap_bitmap_scan_and_flip, wrap_bitmap_dump, wrap_bitmap_expand
};

static struct Command *command_hash_table[HASH_SIZE];

static struct WrapDataStructure wds_arr[MAX_DATA_STRUCTURE_NUM];
static int wds_pool[MAX_DATA_STRUCTURE_NUM];
static int wds_pool_top;
static bool wds_is_using_idx[MAX_DATA_STRUCTURE_NUM];

int main(void)
{
	struct Request req;

	initialize();

	do {
		while(!fetch_request(&req)) {
			// error handling
			fprintf(stderr, "Invalid input\n");
		}
	} while(process_request(&req));
	// need specific error handling when process_request returns "false"

	clean_resource();

	return 0;
}

void initialize(void)
{
	int i;

	for(i = 0; i < sizeof(command_list) / sizeof(char*); ++i) {
		unsigned int hash = hash_command(command_list[i]);
		struct Command *cmd = malloc(sizeof(*cmd));
		cmd->cmd_str = command_list[i];
		cmd->cmd_id = i;
		command_hash_table[hash] = cmd;
	}

	wds_pool_top = MAX_DATA_STRUCTURE_NUM - 1;
	for(i = 0; i < MAX_DATA_STRUCTURE_NUM; ++i) {
		wds_pool[i] = i;
	}
}

void clean_resource(void)
{
	int i;

	for(i = 0; i < sizeof(command_list) / sizeof(char*); ++i) {
		unsigned int hash = hash_command(command_list[i]);
		free(command_hash_table[hash]);
	}
}

bool fetch_request(struct Request *req)
{
	static char input[MAX_INPUT_SIZE];
	
	fgets(input, MAX_INPUT_SIZE, stdin);

	req->token_cnt = 0;
	req->token[req->token_cnt] = strtok(input, " \t\n");
	while(req->token_cnt <= MAX_REQUEST_TOKEN_NUM && req->token[req->token_cnt]) {
		req->token[++req->token_cnt] = strtok(NULL, " \t\n");
	}

	if(req->token_cnt <= 0 || req->token_cnt > MAX_REQUEST_TOKEN_NUM)
		return false;
	else
		return classify_request(req);
}

bool classify_request(struct Request *req)
{
	static char *req_table[] = { "create", "delete", "dumpdata", "quit" };
	static int req_id_table[] = { REQUEST_ID_CREATE, REQUEST_ID_DELETE, REQUEST_ID_DUMPDATA, REQUEST_ID_QUIT };
	int i;
	bool moreSearchFlag = true;
	
	req->id = REQUEST_ID_ELSE;
	for(i = 0; i < sizeof(req_table) / sizeof(char*); ++i) {
		if(strcmp(req->token[0], req_table[i]) == 0) {
			req->id = req_id_table[i];
			moreSearchFlag = false;
			break;
		}
	}

	return moreSearchFlag ? 
		(get_command(req->token[0]) == NULL ? false : true) : true;
}

bool process_request(struct Request *req)
{
	bool ret;
	
	switch(req->id) {
		case REQUEST_ID_CREATE:
			ret = process_request_create(req);
			break;

		case REQUEST_ID_DELETE:
			ret = process_request_delete(req);
			break;

		case REQUEST_ID_DUMPDATA:
			ret = process_request_dumpdata(req);
			break;

		case REQUEST_ID_QUIT:
			ret = false;
			break;

		default:	// command
			ret = process_request_command(req);
	}

	return ret;
}

bool process_request_create(struct Request *req)
{
	// need error handling when token_cnt < 3
	if(req->token_cnt < 3) {
		return false;
	}
	
	struct WrapDataStructure *wds;

	if(find_wds_by_name(req->token[2])) {
		return false;
	}

	wds = get_available_wds();
	if(!wds) {
		return false;
	}

	strncpy(wds->name, req->token[2], sizeof(wds->name));

	if(strcmp(req->token[1], "list") == 0) {
		wds->ds_type = DATA_STRUCTURE_TYPE_LIST;
		wds->ds = malloc(sizeof(struct list));
		list_init(wds->ds);
	}
	else if(strcmp(req->token[1], "hashtable") == 0) {
		wds->ds_type = DATA_STRUCTURE_TYPE_HASHTABLE;
		wds->ds = malloc(sizeof(struct hash));
		hash_init(wds->ds, h_hash_func, h_less_func, 0);
	}
	else if(strcmp(req->token[1], "bitmap") == 0) {
		int bit_cnt;
		if(req->token_cnt < 4) {
			wds->ds_type = DATA_STRUCTURE_TYPE_NOPE;
			destroy_wds(wds);
			return false;
		}
		bit_cnt = atoi(req->token[3]);
		wds->ds_type = DATA_STRUCTURE_TYPE_BITMAP;
		wds->ds = bitmap_create(bit_cnt);
	}
	else {
		// error handling
	}

	return true;
}

bool process_request_delete(struct Request *req)
{
	struct WrapDataStructure *pDel;

	pDel = find_wds_by_name(req->token[1]);
	if(!pDel) {
		return false;
	}

	destroy_wds(pDel);

	return true;
}

bool process_request_dumpdata(struct Request *req)
{
	// need error handling when token_cnt < 2
	if(req->token_cnt < 2) {
		return false;
	}
	
	struct WrapDataStructure *wds;

	if(!(wds = find_wds_by_name(req->token[1]))) {
		return false;
	}

	switch(wds->ds_type) {
		
		case DATA_STRUCTURE_TYPE_LIST: 
		{
			struct list *list = wds->ds;
			struct list_elem *e;
			for(e = list_begin(list); e != list_end(list); e = list_next(e)) {
				struct ListItem *item = list_entry(e, struct ListItem, elem);
				fprintf(stdout, "%d ", item->data);
			}
			fprintf(stdout, "\n");
		}
			break;

		case DATA_STRUCTURE_TYPE_HASHTABLE:
		{
			struct hash *h = wds->ds;
			struct hash_iterator *iter;
			for(hash_first(iter, h); hash_next(iter);) {
				struct HashItem *item = hash_entry(hash_cur(iter), struct HashItem, elem);
				fprintf(stdout, "%d ", item->data);
			}
			fprintf(stdout, "\n");
		}
			break;

		case DATA_STRUCTURE_TYPE_BITMAP:
		{
			struct bitmap *bitmap = wds->ds;
			size_t size = bitmap_size(bitmap);
			int i;
			for (i = 0; i < size; i++) {
				fprintf(stdout, "%d", (bitmap_test(bitmap, i) ? 1 : 0));
			}
			fprintf(stdout, "\n");
		}
			break;

		default:
			return false;
	}
}

bool process_request_command(struct Request *req)
{
	struct Command *cmd;
	struct WrapDataStructure *wds[2];
	bool MultipleWdsFlag = false;

	// need error handling when token_cnt < 2
	if(req->token_cnt < 2) {
		return false;
	}
	
	if(!(cmd = get_command(req->token[0]))) {
		return false;
	}

	if(!get_wds_for_command(req, cmd, wds)) {
		return false;
	}

	return wrap_command[cmd->cmd_id](req, wds);
}

bool get_wds_for_command(struct Request *req, struct Command *cmd, struct WrapDataStructure *wds[])
{
	if(!(wds[0] = find_wds_by_name(req->token[1]))) {
		return false;
	}

	if(strcmp(cmd->cmd_str, "list_splice") == 0) {	
		if(!(wds[1] = find_wds_by_name(req->token[3]))) {
			return false;
		}
	}
	else if(strcmp(cmd->cmd_str, "list_unique") == 0) {
		wds[1] = NULL;
		if(req->token_cnt >= 3 && !(wds[1] = find_wds_by_name(req->token[2]))) {
			return false;
		}
	}

	return true;
}

struct WrapDataStructure* find_wds_by_name(const char *name)
{
	struct WrapDataStructure *res = NULL;
	int i;

	for(i = 0; i < MAX_DATA_STRUCTURE_NUM; ++i) {
		if(!wds_is_using_idx[i])
			continue;
		if(strncmp(wds_arr[i].name, name, sizeof(wds_arr[i].name)) == 0) {
			res = &wds_arr[i];
			break;
		}
	}

	return res;
}

struct WrapDataStructure* get_available_wds(void)
{
	if(wds_pool_top >= 0) {
		int idx = wds_pool[wds_pool_top--];
		wds_is_using_idx[idx] = true;
		return &wds_arr[idx];
	}
	else {
		return NULL;
	}
}

bool destroy_wds(struct WrapDataStructure *pDel)
{
	int idx;

	// destroy ds
	switch(pDel->ds_type) {
		
		case DATA_STRUCTURE_TYPE_LIST:
		{
			struct list *list = pDel->ds;

			while(!list_empty(list)) {
				struct list_elem *elem = list_pop_front(list);
				struct ListItem *item = list_entry(elem, struct ListItem, elem);
				free(item);
			}
			free(list);
		}
			break;

		case DATA_STRUCTURE_TYPE_HASHTABLE:
			hash_destroy(pDel->ds, h_free_func);
			free(pDel->ds);
			break;

		case DATA_STRUCTURE_TYPE_BITMAP:
			bitmap_destroy(pDel->ds);
			break;

		case DATA_STRUCTURE_TYPE_NOPE:
			break;

		default:
			return false;
	}
	
	idx = pDel - wds_arr;
	wds_pool[++wds_pool_top] = idx;
	wds_is_using_idx[idx] = false;

	return true;
}

inline struct Command* get_command(const char *cmd_str)
{
	unsigned int hash = hash_command(cmd_str);
	return command_hash_table[hash];
}

unsigned int hash_command(const char *cmd_str)
{
	unsigned int hash = hash_string_func(cmd_str);

	while(command_hash_table[hash] && strcmp(command_hash_table[hash]->cmd_str, cmd_str) != 0) {
		hash = hash_string_func_for_collision(hash, cmd_str);
	}

	return hash;
}

unsigned int hash_string_func(const char *str)
{
	unsigned int hash = 2729;
	int c;
	while(c = *str++)
		hash = (hash * 585) + c;
	return hash % HASH_SIZE;
}

inline unsigned int hash_string_func_for_collision(unsigned int hash, const char *str)
{
	return (hash * 585 + str[0]) % HASH_SIZE;
}
