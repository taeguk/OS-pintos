#include "frame.h"
#include "threads/palloc.h"
#include "userprog/pagedir.h"
#include "threads/malloc.h"
#include "swap.h"
#include <stdio.h>

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
  //printf ("[Debug] frame_map ()!\n");
  struct frame *frame;
  void *kpage;

  kpage = palloc_get_page (PAL_USER);
  while (kpage == NULL)
    {
      if (!frame_eviction ())
        return false;
      kpage = palloc_get_page (PAL_USER);
    }
  
  //printf ("[Debug]  kpage : 0x%08x.\n", kpage);

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
  
  //printf ("[Debug]  owner : %s, pagedir : 0x%08x, upage : 0x%08x.\n", frame->owner->name, frame->owner->pagedir, frame->suppage->upage);

  //printf ("[Debug] success of frame_map ()!\n");

  return true;
}

/* Unmap a frame from frame list. */
void frame_unmap (struct frame *frame)
{
  //printf ("[Debug] frame_unmap ()! frame : 0x%08x\n", frame);
  frame->suppage->frame = NULL;
  //printf ("[Debug]  what?!\n");
  list_remove (&frame->elem);
  //printf ("[Debug]  owner : %s, pagedir : 0x%08x, upage : 0x%08x.\n", frame->owner->name, frame->owner->pagedir, frame->suppage->upage);
  pagedir_clear_page (frame->owner->pagedir, frame->suppage->upage);
  //printf ("[Debug]  hahaha\n");
  palloc_free_page (frame->kpage);
  //printf ("[Debug] success of frame_unmap ()!\n");
  free (frame);
}

static bool frame_eviction (void)
{
  //printf ("[Debug] frame_eviction ()!\n");
  struct frame *frame;

  frame = frame_find_eviction ();
  if (frame == NULL)
    return false;

  return frame_evict (frame);
}

/* Evict a frame. */
static bool frame_evict (struct frame *frame)
{
  //printf ("[Debug] frame_evict ()!\n");
  if (!swap_store (frame))
    return false;

  frame_unmap (frame);
  return true;
}

/* Find a frame to evict. Use dirty and accessed bits. */
static struct frame *frame_find_eviction (void)
{
  //printf ("[Debug] frame_find_eviction ()!\n");
  // 1. find a frame to evict.
  //    - use dirty and accessed bits. See pagedir.c and manual.

  // temporary code for test.
  return list_entry (list_begin (&frame_list), struct frame, elem);
}

