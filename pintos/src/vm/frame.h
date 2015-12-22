#ifndef VM_FRAME_H
#define VM_FRAME_H

#include "threads/thread.h"
#include "lib/kernel/list.h"
#include "suppage.h"

struct frame
  {
    void *kpage;              // kernel page = paddr + PYS_BASE.
    struct suppage *suppage;  // supplemental page.
    struct thread *owner;     // owner of frame.
    int64_t access_ticks;   // last access ticks. for LRU.
    /* will be added more. */

    struct list_elem elem;    // for frame table.
  };

/* Initialize frame list. */
void frame_init (void);

/* Find a frame to evict. Use dirty and accessed bits. */
struct frame *frame_find_eviction (void);

/* Evict a frame. */
bool frame_evict (struct frame *);

/* Add frame to frame list. */
void frame_add (struct frame *);

/* Delete and free a frame from frame list. */
void frame_free (struct frame *);

#endif /* vm/frame.h */ 
