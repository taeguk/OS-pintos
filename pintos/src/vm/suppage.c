#include "suppage.h"
#include "threads/vaddr.h"
#include "threads/malloc.h"

static void suppage_remove (struct suppage *);

/* Search suppage by vaddr in current thread's supplemental page table. */
struct suppage *suppage_search (void *vaddr)
{
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
  return true;
}

/* Deallocate upage to current thread. */
bool suppage_dealloc (void *vaddr)
{
  struct suppage *suppage;

  suppage = suppage_search (vaddr);
  if (suppage == NULL)
    return false;

  suppage_remove (suppage);

  return true;
}

/* Clear current thread's supplemental page table */
void suppage_clear (void)
{
  struct thread *t = thread_current ();
  struct list_elem *e;

  for (e = list_begin (&t->suppage_list); e != list_end (&t->suppage_list);)
    {
      struct suppage *suppage = list_entry (e, struct suppage, elem);
      e = list_next (e);
      suppage_remove (suppage);
    }
}

static void suppage_remove (struct suppage *suppage)
{
  frame_unmap (suppage->frame);
  list_remove (&suppage->elem);
  free (suppage);
}
