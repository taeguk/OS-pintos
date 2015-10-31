#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include <stdint.h>
#include "lib/user/syscall.h"
#include "threads/vaddr.h"

#define STACK_BLOCK   4
#define SYS_ARG_PTR(ESP, IDX) ((uintptr_t) (ESP) + (IDX) * STACK_BLOCK)

inline static bool chk_user_ptr (const void *);
inline static bool chk_valid_ptr (const void *);
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

inline static bool 
chk_user_ptr (const void *ptr)
{
  return is_user_vaddr(ptr);
}

inline static bool
chk_valid_ptr (const void *ptr)
{
  return chk_user_ptr (ptr);
}

inline static bool
chk_valid_sp (const void *sp_top, int arg_cnt)
{
  ASSERT (arg_cnt > 0);
  return chk_user_ptr ( SYS_ARG_PTR (sp_top, arg_cnt-1) );
}

inline static void
handle_invalid_sp (void)
{
  thread_exit ();
}

static int
get_user (const uint8_t *uaddr)
{
  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
       : "=&a" (result) : "m" (*uaddr));
  return result;
}

/* Writes BYTE to user address UDST.
 * UDST must be below PHYS_BASE.
 * Returns true if successful, false if a segfault occurred. */
static bool
put_user (uint8_t *udst, uint8_t byte)
{
  int error_code;
  asm ("movl $1f, %0; movb %b2, %1; 1:"
       : "=&a" (error_code), "=m" (*udst) : "q" (byte));
  return error_code != -1;
}

static void
syscall_handler (struct intr_frame *f /*UNUSED*/) 
{
  uint32_t syscall_num;
  void *arg_top;

  // will be coded - taeguk
  //hex_dump((uintptr_t) f->esp, (const char *) f->esp, (uintptr_t) PHYS_BASE - (uintptr_t) f->esp, true);

  syscall_num = * (uint32_t *) f->esp;

  switch(syscall_num)
    {
    case SYS_FIBONACCI:           // p 2-1
      arg_top = (void *) ((uintptr_t) f->esp + STACK_BLOCK * 1);
      if (!chk_valid_sp (arg_top, 1)) handle_invalid_sp ();
      f->eax = 
        syscall_fibonacci ( * (int *) SYS_ARG_PTR (arg_top, 0) );
      break;

    case SYS_SUM_OF_FOUR_INTEGERS: // p 2-1
      arg_top = (void *) ((uintptr_t) f->esp + STACK_BLOCK * 6);
      if (!chk_valid_sp (arg_top, 4)) handle_invalid_sp ();
      f->eax = 
        syscall_sum_of_four_integers ( * (int *) SYS_ARG_PTR (arg_top, 0)  ,
                                        * (int *) SYS_ARG_PTR (arg_top, 1) ,
                                        * (int *) SYS_ARG_PTR (arg_top, 2) ,
                                        * (int *) SYS_ARG_PTR (arg_top, 3) );
      break;

    case SYS_HALT:  // project 2-1
      syscall_halt ();
      break;

    case SYS_EXIT:  // p 2-1
      arg_top = (void *) ((uintptr_t) f->esp + STACK_BLOCK * 1);
      if (!chk_valid_sp (arg_top, 1)) handle_invalid_sp ();
      syscall_exit ( * (int *) SYS_ARG_PTR (arg_top, 0) );
      break;

    case SYS_EXEC:  // p 2-1
      arg_top = (void *) ((uintptr_t) f->esp + STACK_BLOCK * 1);
      if (!chk_valid_sp (arg_top, 1)) handle_invalid_sp ();
      f->eax = (uint32_t)
        syscall_exec ( * (char **) SYS_ARG_PTR (arg_top, 0) );
      break;

    case SYS_WAIT:  // p 2-1
      arg_top = (void *) ((uintptr_t) f->esp + STACK_BLOCK * 1);
      if (!chk_valid_sp (arg_top, 1)) handle_invalid_sp ();
      f->eax = 
        syscall_wait ( * (pid_t *) SYS_ARG_PTR (arg_top, 0) );
      break;

    case SYS_READ:  // p 2-1
      arg_top = (void *) ((uintptr_t) f->esp + STACK_BLOCK * 5);
      if (!chk_valid_sp (arg_top, 3)) handle_invalid_sp ();
      f->eax = 
        syscall_read ( * (int *) SYS_ARG_PTR (arg_top, 0) ,
                        * (void **) SYS_ARG_PTR (arg_top, 1) ,
                        * (unsigned *) SYS_ARG_PTR (arg_top, 2) );
      break;

    case SYS_WRITE: // p 2-1
      arg_top = (void *) ((uintptr_t) f->esp + STACK_BLOCK * 5);
      if (!chk_valid_sp (arg_top, 3)) handle_invalid_sp ();
      f->eax = 
        syscall_write ( * (int *) SYS_ARG_PTR (arg_top, 0) ,
                        * (void **) SYS_ARG_PTR (arg_top, 1) ,
                        * (unsigned *) SYS_ARG_PTR (arg_top, 2) );
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
  int a, b, c, i;
  a = 0; b = c = 1;

  if(n == 1) return 1;

  for(i = 1; i < n; ++i)
    {
      c = a + b;
      a = b;
      b = c;
    }

  return c;  // you must modify this! This is only for compile test.
}

static int
syscall_sum_of_four_integers (int a, int b, int c, int d)
{
  return a+b+c+d;  // you must modify this! This is only for compile test.
}

static void 
syscall_halt (void)
{
  shutdown_power_off();
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
  if (! chk_valid_ptr (file))
    return -1;

  // the method to check loading is success must be added.
  return process_execute(file);  // you must modify this! This is only for compile test.
}

static int
syscall_wait (pid_t pid)
{
  return process_wait(pid);  // you must modify this! This is only for compile test.
}

static int
syscall_read (int fd, void *buffer, unsigned size)
{
  // must be modified...
  int i;

  if (! chk_valid_ptr (buffer))
    return -1;

  if(fd == 0 && buffer != NULL)
    {
      for(i = 0; i < size; ++i)
        *(char*)(buffer + i) = input_getc();
      return i;
    }

  return -1;  // you must modify this! This is only for compile test.
}

static int
syscall_write (int fd, void *buffer, unsigned size)
{
  //printf("[Debug] syscall_write() start\n");
  //printf("[Debug] fd = %d, buffer = 0x%08p, size = %u\n", fd, buffer, size);
  if (! chk_valid_ptr (buffer))
    return -1;

  if(fd == 1 && buffer != NULL)
    {
    //printf("[Debug] syscall_write() haaam.\n");
      putbuf(buffer, size);
      return size;
    }
  //printf("[Debug] syscall_write() end (must be not called.)\n");

  return 0;  // you must modify this! This is only for compile test.
}
