#include "testlib.h"

const char *command_list[] = 
{
	"list_insert", "list_splice", "list_push", "list_push_front", "list_push_back",
	"list_remove", "list_pop_front", "list_pop_back", "list_front", "list_back",
	"list_size", "list_empty",
	"list_reverse", "list_sort", "list_insert_ordered", "list_unique",
	"list_max", "list_min",

	"hash_insert", "hash_replace", "hash_find", "hash_delete",
	"hash_clear", "hash_size", "hash_empty", "hash_apply",

	"bitmap_size", "bitmap_set", "bitmap_mark", "bitmap_reset", "bitmap_flip", "bitmap_test",
	"bitmap_set_all", "bitmap_set_multiple", "bitmap_count", "bitmap_contains",
	"bitmap_any", "bitmap_none", "bitmap_all",
	"bitmap_scan", "bitmap_scan_and_flip", "bitmap_dump"
};

struct Command *command_hash_table[HASH_SIZE];

struct WrapDataStructure *wds_arr[MAX_DATA_STRUCTURE_NUM];
int wds_pool[MAX_DATA_STRUCTURE_NUM];
int wds_pool_top;
bool wds_is_using_idx[MAX_DATA_STRUCTURE_NUM];


int main(void)
{
	struct Request req;

	initialize();

	do {
		if(!fetch_request(&req)) {
			// error handling
		}
	} while(process_request(&req));
	// need specific error handling when process_request returns "false"

	clean_resource();

	return 0;
}

void initialize(void)
{
	int i;

	for(i = 0; i < sizeof command_list / sizeof char*; ++i) {
		unsigned int hash = hash_command(command_list[i]);
		struct Command *cmd = malloc(sizeof *cmd);
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

	for(i = 0; i < sizeof command_list / sizeof char*; ++i) {
		unsigned int hash = hash_command(command_list[i]);
		free(command_hash_table[hash]);
	}
}

bool fetch_request(struct Request *req)
{
	static char input[MAX_INPUT_SIZE];
	
	fgets(input, MAX_INPUT_SIZE, stdin);

	req->token_cnt = 0;
	req->token[req->token_cnt] = strtok(input, " \t");
	while(req->token_cnt <= MAX_REQUEST_TOKEN_NUM && req->token[req->token_cnt]) {
		req->token[++req->token_cnt] = strtok(input, " \t");
	}

	if(req->token_cnt <= 0 || req->token_cnt > MAX_REQUEST_TOKEN_NUM)
		return false;
	else
		return classify_request(req);
}

bool classify_request(struct Request *req)
{
	static char *req_table[] = { "create", "delete", "dumpdata", "quit" };
	static int req_id_table[] = { REQUEST_ID_CREATE, REQUEST_ID_DELETE, REQUEST_ID_DUMPDATA,
		REQUEST_ID_COMMNAD, REQUEST_ID_QUIT };
	int i;
	bool moreSearchFlag = true;
	
	for(i = 0; i < sizeof req_table / sizeof char*; ++i) {
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
	
	WrapDataStructure *wds;
	unsigned int hash;

	if(find_wds_by_name(req->token[2])) {
		return false;
	}

	wds = get_available_wds();
	if(!wds) {
		return false;
	}

	strncpy(wds->name, req->token[2], sizeof wds->name);

	if(strcmp(req->token[1], "list") == 0) {
		wds->type = DATA_STRUCTURE_TYPE_LIST;
		wds->ds = malloc(sizeof struct list);
		list_init(wds->ds);
	}
	else if(strcmp(req->token[1], "hashtable") == 0) {
		wds->type = DATA_STRUCTURE_TYPE_HASHTABLE;
		//wds->ds = malloc(sizeof struct hash);
	}
	else if(strcmp(req->token[1], "bitmap") == 0) {
		wds->type = DATA_STRUCTURE_TYPE_BITMAP;
	}
	else {
		// error handling
	}

	return true;
}

bool process_request_delete(struct Request *req)
{
	WrapDataStructure *pDel;

	pDel = find_wds_by_name(req->token[1]);
	if(!pDel) {
		return false;
	}

	destory_wds(pDel);

	return true;
}

bool process_request_dumpdata(struct Request *req)
{
}

bool process_request_command(struct Request *req)
{
}

WrapDataStructure* find_wds_by_name(const char *name)
{
	WrapDataStructure *res = NULL;
	int i;

	for(i = 0; i < MAX_DATA_STRUCTURE_NUM; ++i) {
		if(!wds_is_using_idx[i])
			continue;
		if(strncmp(wds_arr[i]->name, name, sizeof wds_arr[i]->name) == 0) {
			res = &wds_arr[i];
			break;
		}
	}

	return res;
}

WrapDataStructure* get_available_wds(void)
{
	if(wds_pool_top >= 0) {
		int idx = wds_pool[wds_pool_top--];
		wds_is_using_idx[idx] = true;
		return wds_arr[idx];
	}
	else {
		return NULL;
	}
}

bool destory_wds(WrapDataStructure* pDel)
{
	int idx;

	// destroy ds
	
	idx = pDel - wds_arr;
	wds_arr[++wds_pool_top] = idx;
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
