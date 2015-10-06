#include <stdio.h>
#include "testlib.h"
#include "test_bitmap.h"
#include "bitmap.h"

bool wrap_bitmap_size(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;
	size_t size;

	size = bitmap_size(bitmap);

	fprintf(stdout, "%u\n", size);

	return true;
}

bool wrap_bitmap_set(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;
	int idx = atoi(req->token[2]);
	bool val = (strcmp(req->token[3], "true") == 0 ? true : false);

	bitmap_set(bitmap, idx, val);

	return true;
}

bool wrap_bitmap_mark(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;
	int idx = atoi(req->token[2]);

	bitmap_mark(bitmap, idx);

	return true;
}

bool wrap_bitmap_reset(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;
	int idx = atoi(req->token[2]);

	bitmap_reset(bitmap, idx);

	return true;
}

bool wrap_bitmap_flip(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;
	int idx = atoi(req->token[2]);

	bitmap_flip(bitmap, idx);

	return true;
}

bool wrap_bitmap_test(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;
	int idx = atoi(req->token[2]);
	bool val;

	val = bitmap_test(bitmap, idx);

	fprintf(stdout, "%s\n", (val ? "true" : "false"));

	return true;
}

bool wrap_bitmap_set_all(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;
	bool val = (strcmp(req->token[2], "true") == 0 ? true : false);

	bitmap_set_all(bitmap, val);

	return true;
}

bool wrap_bitmap_set_multiple(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;
	int start = atoi(req->token[2]);
	int cnt = atoi(req->token[3]);
	bool val = (strcmp(req->token[4], "true") == 0 ? true : false);

	bitmap_set_multiple(bitmap, start, cnt, val);

	return true;
}

bool wrap_bitmap_count(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;
	int start = atoi(req->token[2]);
	int cnt = atoi(req->token[3]);
	bool val = (strcmp(req->token[4], "true") == 0 ? true : false);
	size_t count;
	
	count = bitmap_count(bitmap, start, cnt, val);
	fprintf(stdout, "%d\n", count);

	return true;
}

bool wrap_bitmap_contains(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;
	int start = atoi(req->token[2]);
	int cnt = atoi(req->token[3]);
	bool val = (strcmp(req->token[4], "true") == 0 ? true : false);
	bool containsFlag;
	
	containsFlag = bitmap_contains(bitmap, start, cnt, val);
	fprintf(stdout, "%s\n", (containsFlag ? "true" : "false"));

	return true;
}

bool wrap_bitmap_any(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;
	int start = atoi(req->token[2]);
	int cnt = atoi(req->token[3]);
	bool flag;
	
	flag = bitmap_any(bitmap, start, cnt);
	fprintf(stdout, "%s\n", (flag ? "true" : "false"));

	return true;
}

bool wrap_bitmap_none(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;
	int start = atoi(req->token[2]);
	int cnt = atoi(req->token[3]);
	bool flag;
	
	flag = bitmap_none(bitmap, start, cnt);
	fprintf(stdout, "%s\n", (flag ? "true" : "false"));

	return true;
}

bool wrap_bitmap_all(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;
	int start = atoi(req->token[2]);
	int cnt = atoi(req->token[3]);
	bool flag;
	
	flag = bitmap_all(bitmap, start, cnt);
	fprintf(stdout, "%s\n", (flag ? "true" : "false"));

	return true;
}

bool wrap_bitmap_scan(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;
	int start = atoi(req->token[2]);
	int cnt = atoi(req->token[3]);
	bool val = (strcmp(req->token[4], "true") == 0 ? true : false);
	size_t res;
	
	res = bitmap_scan(bitmap, start, cnt, val);
	fprintf(stdout, "%u\n", res);

	return true;
}

bool wrap_bitmap_scan_and_flip(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;
	int start = atoi(req->token[2]);
	int cnt = atoi(req->token[3]);
	bool val = (strcmp(req->token[4], "true") == 0 ? true : false);
	size_t res;
	
	res = bitmap_scan_and_flip(bitmap, start, cnt, val);
	fprintf(stdout, "%u\n", res);

	return true;
}

bool wrap_bitmap_dump(struct Request *req, struct WrapDataStructure *wds[])
{
	struct bitmap *bitmap = wds[0]->ds;

	bitmap_dump(bitmap);

	return true;
}
