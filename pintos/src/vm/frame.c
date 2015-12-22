#include "frame.h"

static struct list frame_list;

/* Initialize frame list. */
void frame_init (void)
{
  // do!
}

/* Find a frame to evict. Use dirty and accessed bits. */
struct frame *frame_find_eviction (void)
{
  // 1. find a frame to evict.
  //    - use dirty and accessed bits. See pagedir.c and manual.
}

/* Evict a frame. */
bool frame_evict (struct frame *frame)
{
  // 1. evict a frame to swap list.
  // 1-1. if success, return true.
  // 1-2. if fails, return false.
}

/* Add frame to frame list. */
void frame_add (struct frame *)
{
  // do!
}

/* Delete and free a frame from frame list. */
void frame_free (struct frame *frame)
{
  // palloc_free_page to kpage.
}

/* Mapping upage to frame */
bool frame_map (struct suppage *suppage)
{
  // 1. call palloc_get_page(PAL_USER) 
  // 1-1. if success, create frame and add it to frame list.
  // 1-2. if fails, do eviction.
  // 1-2-1. if eviction success, do (1) one more.
  // 1-2-2. if eviction fails, return false.
}
