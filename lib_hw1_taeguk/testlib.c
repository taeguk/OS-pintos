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
// list_hash_table
// hashTable_hash_table
// bitmap_hash_table

int main(void)
{
	struct Request req;

	initialize();

	do {
		if(!fetch_request(&req)) {
			// error handling
		}
	} while(process_request(&req));

	clean_resource();

	return 0;
}

void initialize(void)
{
	int i;

	for(i = 0; i < sizeof command_list / sizeof char*; ++i) {
		unsigned int hash = hash_command(command_list[i]);
		struct Command *cmd = malloc(sizeof *cmd);
		command_hash_table[hash] = cmd;
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

void process_request(struct Request *req)
{
	switch(req->id) {
		case REQUEST_ID_CREATE:
			process_request_create(req);
			break;

		case REQUEST_ID_DELETE:
			process_request_delete(req);
			break;

		case REQUEST_ID_DUMPDATA:
			process_request_dumpdata(req);
			break;

		case REQUEST_ID_QUIT:
			break;

		default:	// command
			process_request_command(req);
	}
}

void process_request_create(struct Request *req)
{
}

void process_request_delete(struct Request *req)
{
}

void process_request_dumpdata(struct Request *req)
{
}

void process_request_command(struct Request *req)
{
}

struct Command* get_command(char *cmd_str)
{
	int hash = hash_command(cmd_str);
	return command_hash_table[hash];
}

unsigned int hash_command(char *cmd_str)
{
	int hash = hash_string_func(cmd_str);

	while(command_hash_table[hash] && strcmp(command_hash_table[hash]->cmd_str, cmd_str) != 0) {
		hash = (hash * 585 + cmd_str[0]) % HASH_SIZE;
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
