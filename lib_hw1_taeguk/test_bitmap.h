#ifndef __LIB_KERNEL_TEST_BITMAP_H
#define __LIB_KERNEL_TEST_BITMAP_H

#include "bitmap.h"
#include "testlib.h"

bool wrap_bitmap_size(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_bitmap_set(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_bitmap_mark(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_bitmap_reset(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_bitmap_flip(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_bitmap_test(struct Request *req, struct WrapDataStructure *wds[]);

bool wrap_bitmap_set_all(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_bitmap_set_multiple(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_bitmap_count(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_bitmap_contains(struct Request *req, struct WrapDataStructure *wds[]);

bool wrap_bitmap_any(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_bitmap_none(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_bitmap_all(struct Request *req, struct WrapDataStructure *wds[]);

bool wrap_bitmap_scan(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_bitmap_scan_and_flip(struct Request *req, struct WrapDataStructure *wds[]);
bool wrap_bitmap_dump(struct Request *req, struct WrapDataStructure *wds[]);

bool wrap_bitmap_expand(struct Request *req, struct WrapDataStructure *wds[]);

#endif
