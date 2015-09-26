#ifndef __LIB_KERNEL_TESTLIB_H
#define __LIB_KERNEL_TESTLIB_H

#include <stdio.h>
#include <string.h>
#include "list.h"
#include "hash.h"
#include "bitmap.h"

#define MAX_DATA_STRUCTURE_NAME_SIZE	100
#define MAX_DATA_STRUCTURE_NUM			30

#define MAX_INPUT_SIZE	500
#define MAX_REQUEST_TOKEN_NUM	20
#define MAX_COMMAND_STRING_SIZE		100

#define HASH_SIZE	10000

#define REQUEST_ID_CREATE			1
#define REQUEST_ID_DELETE			2
#define	REQUEST_ID_DUMPDATA			3
#define REQUEST_ID_QUIT				4
#define REQUEST_ID_COMMNAD_START	100

#define DATA_STRUCTURE_TYPE_LIST		1
#define DATA_STRUCTURE_TYPE_HASHTABLE	2
#define DATA_STRUCTURE_TYPE_BITMAP		3

struct Request {
	int id;
	int token_cnt;
	char *token[MAX_REQUEST_TOKEN_NUM+1];
};

struct Command {
	const char *cmd_str;
	int cmd_id;
};

struct WrapDataStructure {
	struct void *ds;
	int ds_type;
	char name[MAX_DATA_STRUCTURE_NAME_SIZE];
};

void initialize(void);
void cleanResource(void);

bool fetch_request(struct Request *);
bool classify_request(struct Request *);

bool process_request(struct Request *);
bool process_request_create(struct Request *);
bool process_request_delete(struct Request *);
bool process_request_dumpdata(struct Request *);
bool process_request_command(struct Request *);

inline struct Command* get_command(const char *);
unsigned int hash_command(const char *);
inline struct WrapDataStructure* get_wds(const char *);
unsigned int hash_wds(const char *);
unsigned int hash_string_func(const char *);
inline unsigned int hash_string_func_for_collision(unsigned int, const char *);

#endif
