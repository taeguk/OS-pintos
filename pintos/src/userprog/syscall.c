#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include <stdint.h>
#include "lib/user/syscall.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "userprog/process.h"
#include "filesys/filesys.h"

#define STACK_BLOCK   4
#define SYS_ARG_PTR(ESP, IDX) ((uintptr_t) (ESP) + (IDX) * STACK_BLOCK)
#define SYS_RETURN(RET, VALUE) ({ *(RET) = (int) (VALUE) ; return; })

inline static bool chk_valid_ptr (const void *);
static void syscall_handler (struct intr_frame *);

/* syscalls... */
static void syscall_fibonacci (void *arg_top, int *ret);
static void syscall_sum_of_four_integers  (void *arg_top, int *ret);
static void syscall_halt  (void *arg_top, int *ret);
static void syscall_exit  (void *arg_top, int *ret);
static void syscall_exec  (void *arg_top, int *ret);
static void syscall_wait  (void *arg_top, int *ret);

static void syscall_create  (void *arg_top, int *ret);
static void syscall_remove  (void *arg_top, int *ret);
static void syscall_read    (void *arg_top, int *ret);
static void syscall_write   (void *arg_top, int *ret);

/* added by taeguk */
static void (*syscall_table[SYS_MAX_NUM]) (void*, int*);
static int esp_fix_val[SYS_MAX_NUM];      // needed to mystery problem, shift of syscall arguments...
static int arg_size[SYS_MAX_NUM];

// my work-------
// create, remove
// read, write
//
// youjoon work : open, close, seek, tell, filesize

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");

  syscall_table[SYS_FIBONACCI] = syscall_fibonacci;
  esp_fix_val[SYS_FIBONACCI] = 0;
  arg_size[SYS_FIBONACCI] = STACK_BLOCK * 1;
  
  syscall_table[SYS_SUM_OF_FOUR_INTEGERS] = syscall_sum_of_four_integers;
  esp_fix_val[SYS_SUM_OF_FOUR_INTEGERS] = 20;
  arg_size[SYS_SUM_OF_FOUR_INTEGERS] = STACK_BLOCK * 4;
  
  syscall_table[SYS_HALT] = syscall_halt;
  esp_fix_val[SYS_HALT] = 0;
  arg_size[SYS_HALT] = STACK_BLOCK * 0;

  syscall_table[SYS_EXIT] = syscall_exit;
  esp_fix_val[SYS_EXIT] = 0;
  arg_size[SYS_EXIT] = STACK_BLOCK * 1;

  syscall_table[SYS_EXEC] = syscall_exec;
  esp_fix_val[SYS_EXEC] = 0;
  arg_size[SYS_EXEC] = STACK_BLOCK * 1;
  
  syscall_table[SYS_WAIT] = syscall_wait;
  esp_fix_val[SYS_WAIT] = 0;
  arg_size[SYS_WAIT] = STACK_BLOCK * 1;


  syscall_table[SYS_CREATE] = syscall_create;
  esp_fix_val[SYS_CREATE] = 12;
  arg_size[SYS_CREATE] = STACK_BLOCK * 2;

  syscall_table[SYS_REMOVE] = syscall_remove;
  esp_fix_val[SYS_REMOVE] = 0;
  arg_size[SYS_REMOVE] = STACK_BLOCK * 1;

  syscall_table[SYS_READ] = syscall_read;
  esp_fix_val[SYS_READ] = 16;
  arg_size[SYS_READ] = STACK_BLOCK * 3;

  syscall_table[SYS_WRITE] = syscall_write;
  esp_fix_val[SYS_WRITE] = 16;
  arg_size[SYS_WRITE] = STACK_BLOCK * 3;

  /* you must add initialization to here when new system call added. */
}

inline static bool
chk_valid_ptr (const void *ptr)
{
  return is_user_vaddr (ptr);
}

/*
static int
get_user (const uint8_t *uaddr)
{
  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
       : "=&a" (result) : "m" (*uaddr));
  return result;
}

static bool
put_user (uint8_t *udst, uint8_t byte)
{
  int error_code;
  asm ("movl $1f, %0; movb %b2, %1; 1:"
       : "=&a" (error_code), "=m" (*udst) : "q" (byte));
  return error_code != -1;
}
*/

static void
syscall_handler (struct intr_frame *f)
{
  uint32_t syscall_num;

  //hex_dump((uintptr_t) f->esp, (const char *) f->esp, (uintptr_t) PHYS_BASE - (uintptr_t) f->esp, true);

  syscall_num = * (uint32_t *) f->esp;

  if (syscall_num >= SYS_MAX_NUM || syscall_table[syscall_num] == NULL)
    thread_exit ();
  else
    {
      void *arg_top;
      
      arg_top = (void*) ((uintptr_t) f->esp + STACK_BLOCK + esp_fix_val[syscall_num]);

      if (! is_user_vaddr((void*) ((uintptr_t) arg_top + arg_size[syscall_num] - STACK_BLOCK)))
        thread_exit ();

      syscall_table[syscall_num](arg_top, &f->eax);
    }
}

static void 
syscall_fibonacci (void *arg_top, int *ret)
{
  // Load syscall arguments.
  int n = * (int *) SYS_ARG_PTR (arg_top, 0);

  int a, b, c, i;

  a = 0; b = c = 1;

  if(n == 0) 
    SYS_RETURN (ret, 0);

  for(i = 1; i < n; ++i)
    {
      c = a + b;
      a = b;
      b = c;
    }

  SYS_RETURN (ret, c);
}

static void
syscall_sum_of_four_integers (void *arg_top, int *ret)
{
  /* Load syscall arguments. */
  int a = * (int *) SYS_ARG_PTR (arg_top, 0);
  int b = * (int *) SYS_ARG_PTR (arg_top, 1);
  int c = * (int *) SYS_ARG_PTR (arg_top, 2);
  int d = * (int *) SYS_ARG_PTR (arg_top, 3);

  SYS_RETURN (ret, a+b+c+d);
}

static void 
syscall_halt (void *arg_top UNUSED, int *ret UNUSED)
{
  shutdown_power_off();
}

static void
syscall_exit (void *arg_top, int *ret UNUSED)
{
  /* Load syscall arguments */
  int status = * (int *) SYS_ARG_PTR (arg_top, 0);

  struct thread *cur = thread_current ();
  cur->normal_exit = true;
  cur->exit_code = status;
  thread_exit();
}

static void
syscall_exec (void *arg_top, int *ret)
{
  /* Load syscall arguments */
  const char *file = * (char **) SYS_ARG_PTR (arg_top, 0);

  if (! chk_valid_ptr (file))
    SYS_RETURN (ret, -1);

  SYS_RETURN ( ret, process_execute(file) ); 
}

static void
syscall_wait (void *arg_top, int *ret)
{
  /* Load syscall arguments. */
  pid_t pid = * (pid_t *) SYS_ARG_PTR (arg_top, 0); 

  SYS_RETURN ( ret, process_wait(pid) );
}

static void 
syscall_create (void *arg_top, int *ret)
{
  /* Load syscall arguments. */
  const char *file = * (char **) SYS_ARG_PTR (arg_top, 0); 
  unsigned initial_size = * (unsigned *) SYS_ARG_PTR (arg_top, 1); 

  if (! chk_valid_ptr (file))
    thread_exit ();

  if (file == NULL)
    thread_exit ();
  
  SYS_RETURN (ret, filesys_create (file, initial_size));
}

static void 
syscall_remove (void *arg_top, int *ret)
{
  /* Load syscall arguments. */
  const char *file = * (char **) SYS_ARG_PTR (arg_top, 0);
 
  if (! chk_valid_ptr (file))
    thread_exit ();

  if (file == NULL)
    thread_exit ();

  SYS_RETURN (ret, filesys_remove (file));
}

static void
syscall_read (void *arg_top, int *ret)
{
  // must be modified...
  /* Load syscall arguments */
  int fd = * (int *) SYS_ARG_PTR (arg_top, 0);
  void *buffer = * (void **) SYS_ARG_PTR (arg_top, 1);
  unsigned size = * (unsigned *) SYS_ARG_PTR (arg_top, 2);

  int i;

  if (! chk_valid_ptr (buffer))
    SYS_RETURN (ret, -1);

  if (fd == 0 && buffer != NULL)
    {
      for(i = 0; i < size; ++i)
        *(char*)(buffer + i) = input_getc();
      SYS_RETURN (ret, i);
    }

  SYS_RETURN (ret, -1);
}

static void
syscall_write (void *arg_top, int *ret)
{  
  /* Load syscall arguments */
  int fd = * (int *) SYS_ARG_PTR (arg_top, 0);
  void *buffer = * (void **) SYS_ARG_PTR (arg_top, 1);
  unsigned size = * (unsigned *) SYS_ARG_PTR (arg_top, 2);

  if (! chk_valid_ptr (buffer))
    SYS_RETURN (ret, -1);

  if (fd == 1 && buffer != NULL)
    {
      putbuf (buffer, size);
      SYS_RETURN (ret, size);
    }

  SYS_RETURN (ret, 0);
}
