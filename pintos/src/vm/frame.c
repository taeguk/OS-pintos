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

  list_push_back (&frame_list, &frame->elem);

  if (load_from_swap && !swap_load (frame))
    {
      frame_unmap (frame);
      return false;
    }

  suppage->frame = frame;

  return true;
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

  frame->suppage->frame = NULL;
  frame_unmap (frame);
}

/* Find a frame to evict. Use dirty and accessed bits. */
static struct frame *frame_find_eviction (void)
{
  // 1. find a frame to evict.
  //    - use dirty and accessed bits. See pagedir.c and manual.

  // temporary code for test.
  return list_entry (list_begin (&frame_list), struct frame, elem);
}

