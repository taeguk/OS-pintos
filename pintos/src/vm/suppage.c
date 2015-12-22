#include "suppage.h"

/* Search suppage by vaddr in current thread's supplemental page table. */
struct suppage *suppage_search (void *vaddr)
{
  // search a suppage which uaddr == vaddr's page alignment address.
  // return NULL if not exists.
}

/* Allocate upage to current thread. */
bool suppage_alloc (void *vaddr, bool writable)
{
  // 1. get uaddr (= vaddr's page alignment address).
  // 2. create suppage struct.
  // 3. mapping upage to frame using frame_map().
}

/* Deallocate upage to current thread. */
bool suppage_dealloc (void *vaddr)
{
  // 1. call frame_unmap() to frame.
  // 2. delete from suppage_list.
}

/* Clear current thread's supplemental page table */
void suppage_clear (void)
{
  // 1. iterate all suppages in current thread's suppage_list.
  // 2. call suppage_dealloc().
}
