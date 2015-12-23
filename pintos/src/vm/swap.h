#ifndef VM_SWAP_H
#define VM_SWAP_H

#include <list.h>
#include "devices/block.h"
#include "frame.h"
#include "threads/vaddr.h"

#define SECTORS_PER_PAGE  (PGSIZE / BLOCK_SECTOR_SIZE)
#define SECTORS_PER_SLOT  SECTORS_PER_PAGE

struct swap_slot
  {
    struct suppage *suppage;
    struct thread *owner;
    
    block_sector_t sec_no_start;  // section start number.

    struct list_elem elem;
  };

void swap_init (void);
bool swap_store (struct frame *);
bool swap_load (struct frame *);
bool swap_delete (struct suppage *);

#endif /* vm/swap.h */ 
