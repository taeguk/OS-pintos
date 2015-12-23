#ifndef VM_SUPPAGE_H
#define VM_SUPPAGE_H

#include "frame.h"
#include "threads/thread.h"
#include "lib/kernel/list.h"

struct suppage
  {
    void *upage;            // user page = vaddr.
    struct frame *frame;    // mapped frame. not mapped to frame if NULL.
    struct thread *owner;
    bool writable;
    /* will be added more. */

    struct list_elem elem;  // for supplemental page table.
  };

/* Search suppage by vaddr in current thread's supplemental page table. */
struct suppage *suppage_search (void *vaddr); 

/* Allocate and deallocate upage to current thread. */
bool suppage_alloc (void *vaddr, bool writable);
bool suppage_dealloc (void *vaddr);

/* Clear current thread's supplemental page table */
void suppage_clear (void);

#endif /* vm/suppage.h */ 
