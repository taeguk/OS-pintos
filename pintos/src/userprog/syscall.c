#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include <stdint.h>
#include "lib/user/syscall.h"
#include "threads/vaddr.h"

#define STACK_BLOCK   4
#define SYS_ARG_PTR(ESP, IDX) ((uintptr_t) (ESP) + (IDX) * STACK_BLOCK + STACK_BLOCK)

static bool check_user_pointer (const void *);
static void syscall_handler (struct intr_frame *);

/* syscalls... */
static int    syscall_fibonacci (int n);
static int    syscall_sum_of_four_integers (int a, int b, int c, int d);
static void   syscall_halt (void);
static void   syscall_exit (int status);
static pid_t  syscall_exec (const char *file);
static int    syscall_wait (pid_t pid);
static int    syscall_read (int fd, void *buffer, unsigned size);
static int    syscall_write (int fd, void *buffer, unsigned size);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static bool 
check_user_pointer (const void *ptr)
{
  return is_user_vaddr(ptr);
}

static void
syscall_handler (struct intr_frame *f /*UNUSED*/) 
{
  uint32_t syscall_num;

  // will be coded - taeguk
  printf ("[Debug] system_handler() start\n");
  
  hex_dump((uintptr_t) f->esp, (const char *) f->esp, (uintptr_t) PHYS_BASE - (uintptr_t) f->esp, true);

  syscall_num = * (uint32_t *) f->esp;

  switch(syscall_num)
    {
    case SYS_FIBONACCI:           // p 2-1
      f->eax = 
        syscall_fibonacci ( * (int *) SYS_ARG_PTR (f->esp, 0) );
      break;

    case SYS_SUM_OF_FOUR_INTEGERS: // p 2-1
      f->eax = 
        syscall_sum_of_four_integers ( * (int *) SYS_ARG_PTR (f->esp, 0)  ,
                                        * (int *) SYS_ARG_PTR (f->esp, 1) ,
                                        * (int *) SYS_ARG_PTR (f->esp, 3) ,
                                        * (int *) SYS_ARG_PTR (f->esp, 4) );
      break;

    case SYS_HALT:  // project 2-1
      syscall_halt ();
      break;

    case SYS_EXIT:  // p 2-1
      syscall_exit ( * (int *) SYS_ARG_PTR (f->esp, 0) );
      break;

    case SYS_EXEC:  // p 2-1
      f->eax = (uint32_t)
        syscall_exec ( * (char **) SYS_ARG_PTR (f->esp, 0) );
      break;

    case SYS_WAIT:  // p 2-1
      f->eax = 
        syscall_wait ( * (pid_t *) SYS_ARG_PTR (f->esp, 0) );
      break;

    case SYS_READ:  // p 2-1
      f->eax = 
        syscall_read ( * (int *) SYS_ARG_PTR (f->esp, 0) ,
                        * (void **) SYS_ARG_PTR (f->esp, 1) ,
                        * (unsigned *) SYS_ARG_PTR (f->esp, 2) );
      break;

    case SYS_WRITE: // p 2-1
      f->eax = 
        syscall_write ( * (int *) SYS_ARG_PTR (f->esp, 0) ,
                        * (void **) SYS_ARG_PTR (f->esp, 1) ,
                        * (unsigned *) SYS_ARG_PTR (f->esp, 2) );
      break;
    

    case SYS_CREATE:
      break;
    case SYS_REMOVE:
      break;
    case SYS_OPEN:
      break;
    case SYS_FILESIZE:
      break;
    case SYS_SEEK:
      break;
    case SYS_TELL:
      break;
    case SYS_CLOSE:
      break;
    default:
      break;
    }
  
  printf ("[Debug] system_handler() end\n");

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

static int 
syscall_fibonacci (int n)
{
  return 111;  // you must modify this! This is only for compile test.
}

static int
syscall_sum_of_four_integers (int a, int b, int c, int d)
{
  return -1;  // you must modify this! This is only for compile test.
}

static void 
syscall_halt (void)
{
}

static void
syscall_exit (int status)
{
  struct thread *cur = thread_current ();
  cur->normal_exit = true;
  cur->exit_code = status;
  thread_exit();
}

static pid_t
syscall_exec (const char *file)
{
  return 1111;  // you must modify this! This is only for compile test.
}

static int
syscall_wait (pid_t pid)
{
  return -1;  // you must modify this! This is only for compile test.
}

static int
syscall_read (int fd, void *buffer, unsigned size)
{
  return -1;  // you must modify this! This is only for compile test.
}

static int
syscall_write (int fd, void *buffer, unsigned size)
{
  return -1;  // you must modify this! This is only for compile test.
}
