#include "frame.h"
#include "threads/palloc.h"
#include "userprog/pagedir.h"
#include "threads/malloc.h"
#include "swap.h"

static struct list frame_list;

static bool frame_eviction (void);
static bool frame_evict (struct frame *);
static struct frame *frame_find_eviction (void);

/* Initialize frame list. */
void frame_init (void)
{
  list_init (&frame_list);
}

/* Mapping upage to frame */
bool frame_map (struct suppage *suppage, bool load_from_swap)
{
  struct frame *frame;
  void *kpage;

  kpage = palloc_get_page (PAL_USER);
  while (kpage == NULL)
    {
      if (!frame_eviction ())
        return false;
      kpage = palloc_get_page (PAL_USER);
    }

  pagedir_set_page (suppage->owner->pagedir, 
                    suppage->upage, kpage, suppage->writable);
  frame = malloc (sizeof (struct frame));
  frame->kpage = kpage;
  frame->suppage = suppage;

  frame->owner = suppage->owner;
  free (frame);
}

static bool frame_eviction (void)
{
  struct frame *frame;

  frame = frame_find_eviction ();
  if (frame == NULL)

    return false;

  return frame_evict (frame);
}

/* Evict a frame. */
static bool frame_evict (struct frame *frame)
{
  if (!swap_store (frame))
    return false;

  free (frame);
}

/* Unmap a frame from frame list. */
void frame_unmap (struct frame *frame)
{
  frame->suppage->frame = NULL;
  list_remove (&frame->elem);
  pagedir_clear_page (frame->owner->pagedir, frame->suppage->upage);
  palloc_free_page (frame->kpage);
  free (frame);
}

/* Find a frame to evict. Use dirty and accessed bits. */
static struct frame *frame_find_eviction (void)
{
  /* 
    find a frame to evict.

    1st loop, set accessed bits to 0
    2nd loop, find a frame to evict.
    
    Priority List
    (accessed bit, dirty bit) / state
    ---------------------------------
    #1 (0, 0) / state = 1, neither recently used nor modified
    #2 (0, 1) / state = 2, not recently used but modified
    #3 (1, 0) / state = 3, recently used but clean
    #4 (1, 1) / state = 4, recently used and modified
  */

  struct list_elem *e;

  for (e = list_begin (&frame_list); e != list_end (&frame_list); e = list_next (e))
    {
      struct frame *frame = list_entry (e, struct frame, elem);
      int state = 1;

      if( pagedir_is_dirty (
    }

  



  /* temporary code for test.
  return list_entry (list_begin (&frame_list), struct frame, elem);
  */

}

