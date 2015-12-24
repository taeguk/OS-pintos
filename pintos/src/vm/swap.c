#include "swap.h"
#include "threads/synch.h"
#include "threads/malloc.h"
#include <string.h>
#include <stdio.h>

static struct block *swap_block;
static struct list free_list;
static struct list slot_list;
static block_sector_t slot_cnt;
static block_sector_t max_slot_cnt;
static struct lock swap_lock;

static struct swap_slot *swap_search (struct suppage *);
static void swap_remove (struct swap_slot *);
static void swap_read (block_sector_t sec_no_start, void *buffer, int size);
static void swap_write (block_sector_t sec_no_start, void *buffer, int size);

void swap_init (void)
{
  swap_block = block_get_role (BLOCK_SWAP);
  if (swap_block == NULL)
    PANIC ("Can't open the swap block");

  list_init (&slot_list);
  list_init (&free_list);
  lock_init (&swap_lock);

  slot_cnt = 0;
  max_slot_cnt = block_size (swap_block) / SECTORS_PER_SLOT;
}

bool swap_store (struct frame *frame)
{
  //printf ("[Debug] swap_store ()! free_list : %s, slot_cnt : %d, max_slot_cnt : %d\n", list_empty (&free_list) ? "empty" : "something", slot_cnt, max_slot_cnt);

  struct swap_slot *slot;
  if (list_empty (&free_list))
    {
      if (slot_cnt >= max_slot_cnt)
        return false;
      slot = malloc (sizeof(struct swap_slot));
      slot->sec_no_start = slot_cnt * SECTORS_PER_SLOT;
      ++slot_cnt;
    }
  else
    {
      slot = list_entry (list_pop_back (&free_list), struct swap_slot, elem);
    }

  //printf ("[Debug]  kiki\n");

  slot->suppage = frame->suppage;
  slot->owner = frame->owner;
  
  swap_write (slot->sec_no_start, frame->kpage, PGSIZE);

  list_push_back (&slot_list, &slot->elem);

  //printf ("[Debug] success of swap_store ()!\n");
  return true;
}

bool swap_load (struct frame *frame)
{
  //printf ("[Debug] swap_load ()!\n");
  struct swap_slot *slot = swap_search (frame->suppage);
  if (slot == NULL)
    return false;

  swap_read (slot->sec_no_start, frame->kpage, PGSIZE);
  swap_remove (slot);

  //printf ("[Debug] success of swap_load ()!\n");
  return true;
}

bool swap_delete (struct suppage *suppage)
{
  //printf ("[Debug] swap_delete ()!\n");
  struct swap_slot *slot = swap_search (suppage);
  if (slot == NULL)
    return false;

  swap_remove (slot);

  //printf ("[Debug] ~swap_delete ()!\n");
  return true;
}

static struct swap_slot *swap_search (struct suppage *suppage)
{
  //printf ("[Debug] swap_search ()!\n");
  struct list_elem *e;

  for (e = list_begin (&slot_list); e != list_end (&slot_list); 
       e = list_next (e))
    {
      struct swap_slot *slot = list_entry (e, struct swap_slot, elem);
      if (slot->suppage == suppage)
        return slot;
    }

  return NULL;
}

static void swap_remove (struct swap_slot *slot)
{
  //printf ("[Debug] swap_remove ()!\n");
  list_remove (&slot->elem);
  list_push_back (&free_list, &slot->elem);
}

static void swap_read (block_sector_t sec_no_start, void *buffer, int size)
{
  //printf ("[Debug] swap_read ()!\n");
  block_sector_t sec_no = sec_no_start;
  uint32_t size_lb = (uint32_t) size & ~(uint32_t) (BLOCK_SECTOR_SIZE-1);
  char b[BLOCK_SECTOR_SIZE];
  int s;
  int cnt = 0;
  
  for (s = 0; s < size_lb; s += BLOCK_SECTOR_SIZE, ++cnt)
    block_read (swap_block, sec_no++, buffer + s);
  block_read (swap_block, sec_no++, b);
  memcpy (buffer + s, b, size - s);

  //printf ("[Debug]  cnt : %d\n", cnt);
}

static void swap_write (block_sector_t sec_no_start, void *buffer, int size)
{
  //printf ("[Debug] swap_write ()!\n");
  block_sector_t sec_no = sec_no_start;
  uint32_t size_lb = (uint32_t) size & ~(uint32_t) (BLOCK_SECTOR_SIZE-1);
  char b[BLOCK_SECTOR_SIZE];
  int s;
  int cnt = 0;

  for (s = 0; s < size_lb; s += BLOCK_SECTOR_SIZE, ++cnt)
    block_write (swap_block, sec_no++, buffer + s);
  memcpy (b, buffer + s, size - s);
  block_write (swap_block, sec_no++, b);
  //printf ("[Debug]  cnt : %d\n", cnt);
}
