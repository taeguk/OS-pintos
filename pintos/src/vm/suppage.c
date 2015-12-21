#include "suppage.h"

/* Search suppage by vaddr in current thread's supplemental page table. */
struct suppage *suppage_search (void *vaddr)
{
  // search a suppage which uaddr == vaddr's page alignment address.
}

/* Add suppage to current thread's supplemental page table. */
bool suppage_add (void *vaddr)
{
  // 1. get uaddr (= vaddr's page alignment address).
  // 2. call palloc_get_page(PAL_USER) 
  // 2-1. if success, create frame and add it to frame list.
  // 2-2. if fails, do eviction (use functions of frame.h).
  // 2-2-1. if eviction success, do 2 one more.
  // 2-2-2. if eviction fails, return false. (means thread_exit())
}

/* Clear current thread's supplemental page table */
void suppage_clear (void)
{
  // 1. iterate all suppages in current thread's suppage_list.
  // 2. call frame_free to each suppages' frame.
}
