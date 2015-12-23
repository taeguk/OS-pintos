#ifndef VM_FRAME_H
#define VM_FRAME_H

#include "threads/thread.h"
#include <list.h>
#include "suppage.h"

struct frame
  {
    void *kpage;              // kernel page = paddr + PYS_BASE.
    struct suppage *suppage;  // supplemental page.
    struct thread *owner;     // owner of frame.
    //int64_t access_ticks;   // last access ticks. for LRU.
    /* will be added more. */

    struct list_elem elem;    // for frame table.
  };

/* Initialize frame list. */
void frame_init (void);

/* Mapping upage to frame */
bool frame_map (struct suppage *suppage, bool load_from_swap);

/* Unmap a frame from frame list. */
void frame_unmap (struct frame *);

#endif /* vm/frame.h */ 
