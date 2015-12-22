#include "suppage.h"

/* Search suppage by vaddr in current thread's supplemental page table. */
static struct suppage *suppage_search (void *vaddr)
{
  // search a suppage which uaddr == vaddr's page alignment address.
  // return NULL if not exists.
}

/* Add suppage to current thread's supplemental page table. */
static bool suppage_add (void *vaddr)
{
  // 1. get uaddr (= vaddr's page alignment address).
  // 2. mapping upage to frame using frame_map().
}

/* Clear current thread's supplemental page table */
void suppage_clear (void)
{
  // 1. iterate all suppages in current thread's suppage_list.
  // 2. call frame_free to each suppages' frame.
}

/* Allocate upage to current thread. */
bool suppage_alloc (void *vaddr)
{
  // 1. get uaddr (= vaddr's page alignment address).
  // 2. search a existed upage using suppage_search().
  // 2-1. if exists, mapping upage to frame using frame_map().
  // 2-1-1. if success, return true.
  // 2-1-2. if fails, return false.
  // 2-2. if not exists, do suppage_add().
  // 2-1-1. if success, return true.
  // 2-1-2. if fails, return false.
}
