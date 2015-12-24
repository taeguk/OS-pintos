#include "suppage.h"
#include "threads/vaddr.h"
#include "threads/malloc.h"
#include <stdio.h>

static void suppage_remove (struct suppage *);

void suppage_init (void)
{
  /* Do nothing. */
}

/* Search suppage by vaddr in current thread's supplemental page table. */
struct suppage *suppage_search (void *vaddr)
{
  //printf ("[Debug] suppage_search ()!\n");
  struct thread *t = thread_current ();
  void *upage = pg_round_down (vaddr);
  struct list_elem *e;

  for (e = list_begin (&t->suppage_list); e != list_end (&t->suppage_list);
       e = list_next (e))
    {
      struct suppage *suppage = list_entry (e, struct suppage, elem);
      if (suppage->upage == upage)
        return suppage;
    }

  return NULL;
}

/* Allocate upage to current thread. */
bool suppage_alloc (void *vaddr, bool writable)
{
  //printf ("[Debug] suppage_alloc ()!\n");
  struct suppage *suppage;

  suppage = malloc (sizeof (struct suppage));
  suppage->upage = pg_round_down (vaddr);
  suppage->writable = writable;
  suppage->owner = thread_current ();

  if (!frame_map (suppage, false))
    {
      free (suppage);
      return false;
    }

  list_push_back (&suppage->owner->suppage_list, &suppage->elem);
  //printf ("[Debug]  writable : %s\n", writable ? "true" : "false");
  //printf ("[Debug] success of suppage_alloc ()!\n");
  return true;
}

/* Deallocate upage to current thread. */
bool suppage_dealloc (void *vaddr)
{
  //printf ("[Debug] suppage_dealloc ()!\n");
  struct suppage *suppage;

  suppage = suppage_search (vaddr);
  if (suppage == NULL)
    return false;

  suppage_remove (suppage);

  //printf ("[Debug] success of suppage_dealloc ()!\n");
  return true;
}

/* Clear current thread's supplemental page table */
void suppage_clear (void)
{
  //printf ("[Debug] suppage_clear ()!\n");
  struct thread *t = thread_current ();
  struct list_elem *e;

  for (e = list_begin (&t->suppage_list); e != list_end (&t->suppage_list);)
    {
      struct suppage *suppage = list_entry (e, struct suppage, elem);
      e = list_next (e);
      //printf ("[Debug]  suppage : 0x%08x\n", suppage);
      suppage_remove (suppage);
    }
  //printf ("[Debug] success of suppage_clear ()!\n");
}

static void suppage_remove (struct suppage *suppage)
{
  //printf ("[Debug] suppage_remove ()!\n");
  if (suppage->frame == NULL)
    swap_delete (suppage);
  else
    frame_unmap (suppage->frame);
  list_remove (&suppage->elem);
  free (suppage);
}
