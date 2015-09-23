#ifndef __LIB_KERNEL_TESTLIB_H
#define __LIB_KERNEL_TESTLIB_H

#include <stdio.h>
#include <string.h>

#define MAX_INPUT_SIZE	500
#define MAX_REQUEST_TOKEN_NUM	20
#define MAX_COMMAND_STRING_SIZE	100
#define HASH_SIZE	10000

#define REQUEST_ID_CREATE	1
#define REQUEST_ID_DELETE	2
#define	REQUEST_ID_DUMPDATA	3
#define REQUEST_ID_QUIT		4
#define REQUEST_ID_COMMNAD_START	100

struct Request {
	int id;
	int token_cnt;
	char *token[MAX_REQUEST_TOKEN_NUM+1];
}

struct Command {
	char cmd_str[MAX_COMMAND_STRING_SIZE];
	int cmd_id;
}

void initialize(void);
void cleanResource(void);

bool fetch_request(struct Request *);
bool classify_request(struct Request *);

void process_request(struct Request *);
void process_request_create(struct Request *);
void process_request_delete(struct Request *);
void process_request_dumpdata(struct Request *);
void process_request_command(struct Request *);

struct Command* get_command(char *cmd_str);
unsigned int hash_command(char *cmd_str);
unsigned int hash_string_func(const char *);

#endif
