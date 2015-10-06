#ifndef __LIB_KERNEL_TESTLIB_H
#define __LIB_KERNEL_TESTLIB_H

#include "list.h"
#include "hash.h"
#include "bitmap.h"

#define MAX_INPUT_SIZE				500
#define MAX_REQUEST_TOKEN_NUM		20
#define MAX_COMMAND_STRING_SIZE		100

#define HASH_SIZE		10000

#define REQUEST_ID_CREATE			1
#define REQUEST_ID_DELETE			2
#define	REQUEST_ID_DUMPDATA			3
#define REQUEST_ID_QUIT				4
#define REQUEST_ID_ELSE				5

#define MAX_DATA_STRUCTURE_NAME_SIZE	100
#define MAX_DATA_STRUCTURE_NUM			30

#define DATA_STRUCTURE_TYPE_NOPE		0	// for 'just' destroying
#define DATA_STRUCTURE_TYPE_LIST		1
#define DATA_STRUCTURE_TYPE_HASHTABLE	2
#define DATA_STRUCTURE_TYPE_BITMAP		3

/*
 * Structure for storing user's input.
 * id : REQUEST_ID_*
 * token_cnt : the number of tokens.
 * token : tokens that are tokenized from input_line with whitespace.
 */
struct Request {
	int id;
	int token_cnt;
	char *token[MAX_REQUEST_TOKEN_NUM+1];
};

/*
 * Structure for storing extern command information.
 */
struct Command {
	const char *cmd_str;
	int cmd_id;
};

/*
 * Structure for wrapping data structure that we will test.
 */
struct WrapDataStructure {
	void *ds;
	int ds_type;
	char name[MAX_DATA_STRUCTURE_NAME_SIZE];
};

/*
 * Initializing and cleaning resources.
 */
void initialize(void);
void clean_resource(void);

/*
 * Prompt input from user.
 * Fetch and classify requests.
 */
bool fetch_request(struct Request *);
bool classify_request(struct Request *);

/*
 * Processing request.
 */
bool process_request(struct Request *);
bool process_request_create(struct Request *);
bool process_request_delete(struct Request *);
bool process_request_dumpdata(struct Request *);
bool process_request_command(struct Request *);

/*
 * Get command's wds to need.
 */
bool get_wds_for_command(struct Request *req, struct Command *cmd, struct WrapDataStructure *wds[]);

/* 
 * Finding wds.
 * Getting available wds to use.
 * Destroy wds.
 */
struct WrapDataStructure* find_wds_by_name(const char *);
struct WrapDataStructure* get_available_wds(void);
bool destroy_wds(struct WrapDataStructure *);

/*
 * Hash for command_hash_table.
 */
inline struct Command* get_command(const char *);
unsigned int hash_command(const char *);

/*
 * My hash functions.
 * 1. hash string function.
 * 2. second hash string function for collision.
 */
unsigned int hash_string_func(const char *);
inline unsigned int hash_string_func_for_collision(unsigned int, const char *);

#endif
