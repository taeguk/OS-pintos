#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  uint32_t syscall_num;

  // will be coded - taeguk
  printf ("system call!\n");

  syscall_num = * (uint32_t*) f->esp;

  // call of handler of system call

  // save system call's result.
  // store return value to eax.

  //thread_exit ();
}

// must implement role to check wheter address which is system call's parameter is placed at below PHYS_BASE.
// I think making a function to check that.
// And every system call use this function to check if address is user area.
// Invalid pointer to user area is process in page_fault() in exception.c
// -taeguk
