#include "swap.h"
#include "threads/synch.h"
#include "threads/malloc.h"

static struct disk *swap_disk;
static struct list slot_list;
static disk_sector_t slot_cnt;
static disk_sector_t max_slot_cnt;
static struct lock swap_lock;

static struct swap_slot *swap_search (struct suppage *);
static void swap_remove (struct swap_slot *);
static void swap_read (disk_sector_t sec_no_start, void *buffer, int size);
static void swap_write (disk_sector_t sec_no_start, void *buffer, int size);

void swap_init (void)
{
  swap_disk = disk_get (1,1);
  if (swap_disk == NULL)
    PANIC ("Can't open the swap disk");

  list_init (&slot_list);
  lock_init (&swap_lock);

  slot_cnt = 0;
  max_slot_cnt = disk_size (swap_disk) / SECTORS_PER_SLOT;
}

bool swap_store (struct frame *frame)
{
  if (slot_cnt >= max_slot_cnt)
    return false;

  struct swap_slot *slot;

  slot = malloc (sizeof(struct swap_slot));
  slot->suppage = frame->suppage;
  slot->owner = frame->owner;
  slot->sec_no_start = slot_cnt * SECTORS_PER_SLOT;
  
  swap_write (slot->sec_no_start, frame->kpage, PGSIZE);

  return true;
}

bool swap_load (struct frame *frame)
{
  struct swap_slot *slot = swap_search (frame->suppage);
  if (slot == NULL)
    return false;

  swap_read (slot->sec_no_start, frame->kpage, PGSIZE);
  swap_remove (slot);

  return true;
}

static struct swap_slot *swap_search (struct suppage *suppage)
{
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
  list_remove (&slot->elem);
  free (slot);
}

static void swap_read (disk_sector_t sec_no_start, void *buffer, int size)
{
  disk_sector_t sec_no = sec_no_start;
  uint32_t size_lb = (uint32_t) size & ~(uint32_t) (DISK_SECTOR_SIZE-1);
  char b[DISK_SECTOR_SIZE];
  int s;
  
  for (s = 0; s < size_lb; s += DISK_SECTOR_SIZE)
    disk_read (swap_disk, sec_no++, buffer + s);
  disk_read (swap_disk, sec_no++, b);
  memcpy (buffer + s, b, size - s);
}

static void swap_write (disk_sector_t sec_no_start, void *buffer, int size)
{
  disk_sector_t sec_no = sec_no_start;
  uint32_t size_lb = (uint32_t) size & ~(uint32_t) (DISK_SECTOR_SIZE-1);
  char b[DISK_SECTOR_SIZE];
  int s;

  for (s = 0; s < size_lb; s += DISK_SECTOR_SIZE)
    disk_write (swap_disk, sec_no++, buffer + s);
  memcpy (b, buffer + s, size - s);
  disk_write (swap_disk, sec_no++, b);
}
