#ifndef VM_SUPPAGE_H
#define VM_SUPPAGE_H

#include "frame.h"
#include "threads/thread.h"
#include "lib/kernel/list.h"

struct suppage
  {
    void *uaddr;            // user page = vaddr.
    struct frame *frame;    // frame. not allocated to frame if NULL.
    /* will be added more. */

    struct list_elem elem;  // for supplemental page table.
  };

/* Search suppage by vaddr in current thread's supplemental page table. */
struct suppage *suppage_search (void *vaddr); 

/* Add suppage to current thread's supplemental page table. */
bool suppage_add (void *vaddr);

/* Clear current thread's supplemental page table */
void suppage_clear (void);

#endif /* vm/suppage.h */ 
