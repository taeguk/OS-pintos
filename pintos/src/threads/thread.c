#include "threads/thread.h"
#include <debug.h>
#include <stddef.h>
#include <random.h>
#include <stdio.h>
#include <string.h>
#include "threads/flags.h"
#include "threads/interrupt.h"
#include "threads/intr-stubs.h"
#include "threads/palloc.h"
#include "threads/switch.h"
#include "threads/synch.h"
#include "threads/vaddr.h"

#include "devices/timer.h"
#include "lib/kernel/real.h"

#ifdef USERPROG
#include "userprog/process.h"
#endif

/* Random value for struct thread's `magic' member.
   Used to detect stack overflow.  See the big comment at the top
   of thread.h for details. */
#define THREAD_MAGIC 0xcd6abf4b

/* List of processes in THREAD_READY state, that is, processes
   that are ready to run but not actually running. */
//static struct list ready_list;
/* the number of threads in the ready_list. */
//static int ready_cnt;

/* added by taeguk for project 1 */

static real load_avg;

/* Multi-level Queue of processes in THREAD_READY state */
static struct list ready_queue[PRI_MAX+1];
/* the number of threads in the ready queues. */
static int ready_threads;

static bool thread_less_priority (const struct list_elem *a, const struct list_elem *b, void *aux UNUSED);
 
/* Priority Queue of sleeping processes in THREAD_BLOCKED state */
static struct list sleep_queue;
/* the number of threads in the block_list. */
static int sleep_cnt;   

/* List of all processes.  Processes are added to this list
   when they are first scheduled and removed when they exit. */
static struct list all_list;

/* Idle thread. */
static struct thread *idle_thread;

/* Initial thread, the thread running init.c:main(). */
static struct thread *initial_thread;

/* Lock used by allocate_tid(). */
static struct lock tid_lock;

/* Stack frame for kernel_thread(). */
struct kernel_thread_frame 
  {
    void *eip;                  /* Return address. */
    thread_func *function;      /* Function to call. */
    void *aux;                  /* Auxiliary data for function. */
  };

/* Statistics. */
static long long idle_ticks;    /* # of timer ticks spent idle. */
static long long kernel_ticks;  /* # of timer ticks in kernel threads. */
static long long user_ticks;    /* # of timer ticks in user programs. */

/* Scheduling. */
#define TIME_SLICE 4            /* # of timer ticks to give each thread. */
static unsigned thread_ticks;   /* # of timer ticks since last yield. */

#ifndef USERPROG
bool thread_prior_aging;
#endif

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
bool thread_mlfqs;

static void kernel_thread (thread_func *, void *aux);

static void idle (void *aux UNUSED);
static struct thread *running_thread (void);
static struct thread *next_thread_to_run (void);
static void init_thread (struct thread *, const char *name, int priority);
static bool is_thread (struct thread *) UNUSED;
static void *alloc_frame (struct thread *, size_t size);
static void schedule (void);
void thread_schedule_tail (struct thread *prev);
static tid_t allocate_tid (void);

#ifdef USERPROG
static int get_avail_fd (struct thread *t);
#endif

/* Initializes the threading system by transforming the code
   that's currently running into a thread.  This can't work in
   general and it is possible in this case only because loader.S
   was careful to put the bottom of the stack at a page boundary.

   Also initializes the run queue and the tid lock.

   After calling this function, be sure to initialize the page
   allocator before trying to create any threads with
   thread_create().

   It is not safe to call thread_current() until this function
   finishes. */
void
thread_init (void) 
{
  int i=0;
  ASSERT (intr_get_level () == INTR_OFF);

  lock_init (&tid_lock);
  // modified by taeguk.
  //list_init (&ready_list);
  for (i = PRI_MIN; i <= PRI_MAX; ++i)
    list_init (&ready_queue[i]);
  list_init (&all_list);

  // added by younjoon
  list_init (&sleep_queue);

  /* Set up a thread structure for the running thread. */
  initial_thread = running_thread ();
  init_thread (initial_thread, "main", PRI_DEFAULT);
  initial_thread->status = THREAD_RUNNING;
  initial_thread->tid = allocate_tid ();
}

/* Starts preemptive thread scheduling by enabling interrupts.
   Also creates the idle thread. */
void
thread_start (void) 
{
  /* Create the idle thread. */
  struct semaphore start_idle;
  sema_init (&start_idle, 0);
  thread_create ("idle", PRI_MIN, idle, &start_idle);

  /* Start preemptive thread scheduling. */
  intr_enable ();

  /* Wait for the idle thread to initialize idle_thread. */
  sema_down (&start_idle);
}

/* Called by the timer interrupt handler at each timer tick.
   Thus, this function runs in an external interrupt context. */
void
thread_tick (void) 
{
  struct thread *t = thread_current ();
  enum intr_level old_level;
  int64_t ticks;

  /* Update statistics. */
  if (t == idle_thread)
    idle_ticks++;
#ifdef USERPROG
  else if (t->pagedir != NULL)
    user_ticks++;
#endif
  else
    kernel_ticks++;

  // recalcurate priority of all threads per 4 ticks.

  old_level = intr_disable ();

  ticks = timer_ticks ();

  t->recent_cpu = real_add_ri (t->recent_cpu, 1);

  if (ticks % TIMER_FREQ == 0)
    {
      thread_update_load_avg ();
      thread_update_recent_cpu ();
    }

  if (ticks % TIME_SLICE == 0)
    {
      thread_update_priority ();
    }

  intr_set_level (old_level);
  
  /* Enforce preemption. */
  if (++thread_ticks >= TIME_SLICE)
    intr_yield_on_return ();

  // increase current thread's recent_cpu 
  // update all threads' recent_cpu per TIMER_FREQ
  // must be implemented.

#ifndef USERPROG
 
  //thread_wake (ticks); 
  if (thread_prior_aging == true)
    thread_aging ();
#endif
}

/* Prints thread statistics. */
void
thread_print_stats (void) 
{
  printf ("Thread: %lld idle ticks, %lld kernel ticks, %lld user ticks\n",
          idle_ticks, kernel_ticks, user_ticks);
}

/* Creates a new kernel thread named NAME with the given initial
   PRIORITY, which executes FUNCTION passing AUX as the argument,
   and adds it to the ready queue.  Returns the thread identifier
   for the new thread, or TID_ERROR if creation fails.

   If thread_start() has been called, then the new thread may be
   scheduled before thread_create() returns.  It could even exit
   before thread_create() returns.  Contrariwise, the original
   thread may run for any amount of time before the new thread is
   scheduled.  Use a semaphore or some other form of
   synchronization if you need to ensure ordering.

   The code provided sets the new thread's `priority' member to
   PRIORITY, but no actual priority scheduling is implemented.
   Priority scheduling is the goal of Problem 1-3. */
tid_t
thread_create (const char *name, int priority,
               thread_func *function, void *aux) 
{
  struct thread *t;
  struct kernel_thread_frame *kf;
  struct switch_entry_frame *ef;
  struct switch_threads_frame *sf;
  tid_t tid;
  enum intr_level old_level;

  ASSERT (function != NULL);
      
  /* Allocate thread. */
  t = palloc_get_page (PAL_ZERO);
  if (t == NULL)
    return TID_ERROR;

  /* Initialize thread. */
  init_thread (t, name, priority);
  tid = t->tid = allocate_tid ();

  /* Prepare thread for first run by initializing its stack.
     Do this atomically so intermediate values for the 'stack' 
     member cannot be observed. */
  old_level = intr_disable ();

  /* Stack frame for kernel_thread(). */
  kf = alloc_frame (t, sizeof *kf);
  kf->eip = NULL;
  kf->function = function;
  kf->aux = aux;

  /* Stack frame for switch_entry(). */
  ef = alloc_frame (t, sizeof *ef);
  ef->eip = (void (*) (void)) kernel_thread;

  /* Stack frame for switch_threads(). */
  sf = alloc_frame (t, sizeof *sf);
  sf->eip = switch_entry;
  sf->ebp = 0;

  intr_set_level (old_level);

#ifdef USERPROG
  list_push_back (&thread_current()->child_list, (struct list_elem *) &t->child_elem);
#endif

  /* Add to run queue. */
  thread_unblock (t);

  if (priority > thread_current ()->priority)
    thread_yield ();

  return tid;
}

/* Puts the current thread to sleep.  It will not be scheduled
   again until awoken by thread_unblock().

   This function must be called with interrupts turned off.  It
   is usually a better idea to use one of the synchronization
   primitives in synch.h. */
void
thread_block (void) 
{
  ASSERT (!intr_context ());
  ASSERT (intr_get_level () == INTR_OFF);

  thread_current ()->status = THREAD_BLOCKED;
  schedule ();
}

/* Transitions a blocked thread T to the ready-to-run state.
   This is an error if T is not blocked.  (Use thread_yield() to
   make the running thread ready.)

   This function does not preempt the running thread.  This can
   be important: if the caller had disabled interrupts itself,
   it may expect that it can atomically unblock a thread and
   update other data. */
void
thread_unblock (struct thread *t) 
{
  enum intr_level old_level;

  ASSERT (is_thread (t));

  old_level = intr_disable ();
  ASSERT (t->status == THREAD_BLOCKED);
  
  //list_push_back (&ready_list, &t->elem);
  //++ready_cnt;
  
  // modified by taeguk.
  if (t != idle_thread)
    {
      list_push_back (&ready_queue[t->priority], &t->elem);
      ++ready_threads;
    }
  //list_insert_ordered (&ready_queue[t->priority], &t->elem, thread_less_priority, NULL);
  
  t->status = THREAD_READY;
  intr_set_level (old_level);
}

/* Returns the name of the running thread. */
const char *
thread_name (void) 
{
  return thread_current ()->name;
}

/* Returns the running thread.
   This is running_thread() plus a couple of sanity checks.
   See the big comment at the top of thread.h for details. */
struct thread *
thread_current (void) 
{
  struct thread *t = running_thread ();
  
  /* Make sure T is really a thread.
     If either of these assertions fire, then your thread may
     have overflowed its stack.  Each thread has less than 4 kB
     of stack, so a few big automatic arrays or moderate
     recursion can cause stack overflow. */
  ASSERT (is_thread (t));
  ASSERT (t->status == THREAD_RUNNING);

  return t;
}

/* Returns the running thread's tid. */
tid_t
thread_tid (void) 
{
  return thread_current ()->tid;
}

/* Deschedules the current thread and destroys it.  Never
   returns to the caller. */
void
thread_exit (void) 
{
  ASSERT (!intr_context ());

#ifdef USERPROG
  process_exit ();
#endif

  /* Remove thread from all threads list, set our status to dying,
     and schedule another process.  That process will destroy us
     when it calls thread_schedule_tail(). */
  intr_disable ();
  list_remove (&thread_current()->allelem);
  thread_current ()->status = THREAD_DYING;
  schedule ();
  NOT_REACHED ();
}

/* Yields the CPU.  The current thread is not put to sleep and
   may be scheduled again immediately at the scheduler's whim. */
void
thread_yield (void) 
{
  struct thread *cur = thread_current ();
  enum intr_level old_level;
  
  ASSERT (!intr_context ());

  old_level = intr_disable ();
  //if (cur != idle_thread) 
  // list_push_back (&ready_list, &cur->elem);
  // modified by taeguk.
  if (cur != idle_thread)
    {
      list_push_back (&ready_queue[cur->priority], &cur->elem);
      //list_insert_ordered (&ready_queue[cur->priority], &cur->elem, thread_less_priority, NULL);
    }
  cur->status = THREAD_READY;
  schedule ();
  intr_set_level (old_level);
}

/* Invoke function 'func' on all threads, passing along 'aux'.
   This function must be called with interrupts off. */
void
thread_foreach (thread_action_func *func, void *aux)
{
  struct list_elem *e;

  ASSERT (intr_get_level () == INTR_OFF);

  for (e = list_begin (&all_list); e != list_end (&all_list);
       e = list_next (e))
    {
      struct thread *t = list_entry (e, struct thread, allelem);
      func (t, aux);
    }
}

/* Sets the current thread's priority to NEW_PRIORITY. */
void
thread_set_priority (int new_priority) 
{
  thread_current ()->priority = new_priority;
 
  /* added by younjoon */
  thread_yield ();
}

/* Returns the current thread's priority. */
int
thread_get_priority (void) 
{
  return thread_current ()->priority;
}

void thread_update_priority (void)
{
  struct list_elem *e;

  for (e = list_begin (&all_list); e != list_end (&all_list);
     e = list_next (e))
  {
    struct thread *t = list_entry (e, struct thread, allelem);

    // can be problem...20% probablility
    t->priority = PRI_MAX - real_to_int_nearest (real_div_ri (t->recent_cpu, 4))
                    - (t->nice * 2);

    if (t->priority > PRI_MAX)
      t->priority = PRI_MAX;
    else if (t->priority < PRI_MIN)
      t->priority = PRI_MIN;

    list_remove (&t->elem);
    list_push_back (&ready_queue[t->priority], &t->elem);
  }
}

/* Sets the current thread's nice value to NICE. */
void
thread_set_nice (int nice) 
{
  thread_current ()->nice = nice;
}

/* Returns the current thread's nice value. */
int
thread_get_nice (void) 
{
  return thread_current ()->nice;
}

void thread_update_recent_cpu (void)
{
  real load_avg_x2 = real_mul_ri (load_avg, 2);
  real expr1 = real_div_rr (load_avg_x2, real_add_ri (load_avg_x2, 1));
  struct list_elem *e;

  for (e = list_begin (&all_list); e != list_end (&all_list);
     e = list_next (e))
  {
    struct thread *t = list_entry (e, struct thread, allelem);
    t->recent_cpu = real_add_ri (real_mul_rr (expr1, t->recent_cpu), t->nice);
  }
}

/* Returns 100 times the current thread's recent_cpu value. */
int
thread_get_recent_cpu (void) 
{
  struct thread *cur = thread_current ();
  return real_to_int_nearest ( 
            real_mul_ri (cur->recent_cpu, 100) );
}

void thread_update_load_avg (void)
{
  real coef1 = real_div_ri (real_from_int (59), 60);
  real coef2 = real_div_ri (real_from_int (1), 60);
  load_avg = real_add_rr (
                real_mul_rr (coef1, load_avg), 
                real_mul_ri (coef2, ready_threads) );
}

/* Returns 100 times the system load average. */
int
thread_get_load_avg (void) 
{
  return real_to_int_nearest ( 
            real_mul_ri (load_avg, 100) );
}

/* Idle thread.  Executes when no other thread is ready to run.

   The idle thread is initially put on the ready list by
   thread_start().  It will be scheduled once initially, at which
   point it initializes idle_thread, "up"s the semaphore passed
   to it to enable thread_start() to continue, and immediately
   blocks.  After that, the idle thread never appears in the
   ready list.  It is returned by next_thread_to_run() as a
   special case when the ready list is empty. */
static void
idle (void *idle_started_ UNUSED) 
{
  struct semaphore *idle_started = idle_started_;
  idle_thread = thread_current ();
  sema_up (idle_started);

  for (;;) 
    {
      /* Let someone else run. */
      intr_disable ();
      thread_block ();

      /* Re-enable interrupts and wait for the next one.

         The `sti' instruction disables interrupts until the
         completion of the next instruction, so these two
         instructions are executed atomically.  This atomicity is
         important; otherwise, an interrupt could be handled
         between re-enabling interrupts and waiting for the next
         one to occur, wasting as much as one clock tick worth of
         time.

         See [IA32-v2a] "HLT", [IA32-v2b] "STI", and [IA32-v3a]
         7.11.1 "HLT Instruction". */
      asm volatile ("sti; hlt" : : : "memory");
    }
}

/* Function used as the basis for a kernel thread. */
static void
kernel_thread (thread_func *function, void *aux) 
{
  ASSERT (function != NULL);

  intr_enable ();       /* The scheduler runs with interrupts off. */
  function (aux);       /* Execute the thread function. */
  thread_exit ();       /* If function() returns, kill the thread. */
}

/* Returns the running thread. */
struct thread *
running_thread (void) 
{
  uint32_t *esp;

  /* Copy the CPU's stack pointer into `esp', and then round that
     down to the start of a page.  Because `struct thread' is
     always at the beginning of a page and the stack pointer is
     somewhere in the middle, this locates the curent thread. */
  asm ("mov %%esp, %0" : "=g" (esp));
  return pg_round_down (esp);
}

/* Returns true if T appears to point to a valid thread. */
static bool
is_thread (struct thread *t)
{
  return t != NULL && t->magic == THREAD_MAGIC;
}

/* Does basic initialization of T as a blocked thread named
   NAME. */
static void
init_thread (struct thread *t, const char *name, int priority)
{
  ASSERT (t != NULL);
  ASSERT (PRI_MIN <= priority && priority <= PRI_MAX);
  ASSERT (name != NULL);

  printf("init_thread() %s!\n", name);

  memset (t, 0, sizeof *t);
  t->status = THREAD_BLOCKED;
  strlcpy (t->name, name, sizeof t->name);
  t->stack = (uint8_t *) t + PGSIZE;
  t->priority = priority;
  t->magic = THREAD_MAGIC;
  list_push_back (&all_list, &t->allelem);
 
#ifdef USERPROG
  /* For process. Added by taeguk */
  list_init (&t->child_list);
  t->normal_exit = false;
  t->exit_code = -1;
  sema_init (&t->exit_sema, 0);
  sema_init (&t->wait_sema, 0);

  t->self_file = NULL;
  list_init (&t->file_list);
#endif

  /*
   * Initilizing thread variables for project-1
   */
  if (t == initial_thread)
    {
      t->nice = 0;
      t->recent_cpu = real_from_int (0);
    }
  else
    {
      struct thread *cur = thread_current ();
      t->nice = cur->nice;
      // why??
      t->recent_cpu = cur->recent_cpu;
    }
}

/* Allocates a SIZE-byte frame at the top of thread T's stack and
   returns a pointer to the frame's base. */
static void *
alloc_frame (struct thread *t, size_t size) 
{
  /* Stack data is always allocated in word-size units. */
  ASSERT (is_thread (t));
  ASSERT (size % sizeof (uint32_t) == 0);

  t->stack -= size;
  return t->stack;
}

/* Chooses and returns the next thread to be scheduled.  Should
   return a thread from the run queue, unless the run queue is
   empty.  (If the running thread can continue running, then it
   will be in the run queue.)  If the run queue is empty, return
   idle_thread. */
static struct thread *
next_thread_to_run (void) 
{
  /* must be re-implemented by younjoon */
  //enum intr_level old_level;
  int i;

  /*
  if (list_empty (&ready_list))
    return idle_thread;
  else
    {
      old_level = intr_disable ();
      list_sort (&ready_list, thread_less_priority, NULL);
      intr_set_level (old_level);

      return list_entry (list_pop_front (&ready_list), struct thread, elem);
    }
    */

  if (ready_threads == 0)
    return idle_thread;
  else
    {
      for (i = PRI_MAX; i >= PRI_MIN; --i)
        {
          if(list_size (&ready_queue[i]) != 0)
            {
              --ready_threads;
              return list_entry (list_pop_front (&ready_queue[i]), struct thread, elem);
            }
        }
    }
}

/* Completes a thread switch by activating the new thread's page
   tables, and, if the previous thread is dying, destroying it.

   At this function's invocation, we just switched from thread
   PREV, the new thread is already running, and interrupts are
   still disabled.  This function is normally invoked by
   thread_schedule() as its final action before returning, but
   the first time a thread is scheduled it is called by
   switch_entry() (see switch.S).

   It's not safe to call printf() until the thread switch is
   complete.  In practice that means that printf()s should be
   added at the end of the function.

   After this function and its caller returns, the thread switch
   is complete. */
void
thread_schedule_tail (struct thread *prev)
{
  struct thread *cur = running_thread ();
  
  ASSERT (intr_get_level () == INTR_OFF);

  /* Mark us as running. */
  cur->status = THREAD_RUNNING;

  //added by taeguk
  //++ready_cnt;

  /* Start new time slice. */
  thread_ticks = 0;

#ifdef USERPROG
  /* Activate the new address space. */
  process_activate ();
#endif

  /* If the thread we switched from is dying, destroy its struct
     thread.  This must happen late so that thread_exit() doesn't
     pull out the rug under itself.  (We don't free
     initial_thread because its memory was not obtained via
     palloc().) */
  if (prev != NULL && prev->status == THREAD_DYING && prev != initial_thread) 
    {
      ASSERT (prev != cur);
      palloc_free_page (prev);
    }
}

/* Schedules a new process.  At entry, interrupts must be off and
   the running process's state must have been changed from
   running to some other state.  This function finds another
   thread to run and switches to it.

   It's not safe to call printf() until thread_schedule_tail()
   has completed. */
static void
schedule (void) 
{
  struct thread *cur = running_thread ();
  struct thread *next = next_thread_to_run ();
  struct thread *prev = NULL;

  ASSERT (intr_get_level () == INTR_OFF);
  ASSERT (cur->status != THREAD_RUNNING);
  ASSERT (is_thread (next));

  if (cur != next)
    prev = switch_threads (cur, next);
  thread_schedule_tail (prev);
}

/* Returns a tid to use for a new thread. */
static tid_t
allocate_tid (void) 
{
  static tid_t next_tid = 1;
  tid_t tid;

  lock_acquire (&tid_lock);
  tid = next_tid++;
  lock_release (&tid_lock);

  return tid;
}

/* Offset of `stack' member within `struct thread'.
   Used by switch.S, which can't figure it out on its own. */
uint32_t thread_stack_ofs = offsetof (struct thread, stack);

static bool
thread_less_priority (const struct list_elem *a, const struct list_elem *b, void *aux UNUSED)
{
  struct thread *ta = list_entry (a, struct thread, elem);
  struct thread *tb = list_entry (b, struct thread, elem);

  if (ta->priority > tb->priority)
    return true;
  else  
    return false;
}

#ifndef USERPROG

/* coded by younjoon */
static bool
thread_less_ticks (const struct list_elem *a, const struct list_elem *b, void *aux UNUSED)
{
  struct thread *ta = list_entry (a, struct thread, sleep_elem);
  struct thread *tb = list_entry (b, struct thread, sleep_elem);

  if (ta->sleep_ticks < tb->sleep_ticks)
    return true;
  else if (ta->sleep_ticks == tb->sleep_ticks)
    {
      if (ta->priority > tb->priority)
        return true;
      else
        return false;
    }
  else
    return false;
}

void
thread_sleep (int64_t sleep_ticks)
{
  enum intr_level old_level;
  struct thread *cur = thread_current ();

  printf("[Debug] thread_sleep()! \n");
  old_level = intr_disable ();
  if (cur->status != THREAD_BLOCKED)
    {
      cur->sleep_ticks = sleep_ticks;
      list_insert_ordered (&sleep_queue, &cur->sleep_elem, thread_less_ticks, NULL);
      sleep_cnt++;
      thread_block ();
    }
  intr_set_level (old_level);
}

void 
thread_wake (int64_t cur_ticks)
{
  printf("[Debug] thread_wake()! %s %d\n", thread_current()->name, ready_threads);
  while (sleep_cnt != 0)
    {
      printf("[Debug] sleep_cnt != 0! \n");
      struct list_elem *e = list_front (&sleep_queue);
      struct thread *cur = list_entry (e, struct thread, sleep_elem);
      
      if (cur->sleep_ticks <= cur_ticks)
        {
          list_pop_front (&sleep_queue);
          sleep_cnt--;
          thread_unblock (cur);
        }
      else
        break;
    }
}

void thread_aging (void)
{
  /* will be implemented by younjoon */
  // increase threads' age in ready_queues.
  
  /* added by younjoon, for single level queue */
  struct list_elem *e;

  for (e = list_begin (&all_list); e != list_end (&all_list);
       e = list_next (e))
    {
      struct thread *t = list_entry (e, struct thread, allelem);
      ++t->priority;
      if (t->priority > PRI_MAX)
        {
          t->priority = PRI_MAX;
        }
      else
        {
          list_remove (&t->elem);
          list_push_back (&ready_queue[t->priority], &t->elem);
        }
    }
}
#endif

#ifdef USERPROG
/* added by taeguk and coded by younjoon for project 2-2 */

/* get available fd in thread.
 * return value : available fd (success), -1 (fail)
 *  *the reason of failure : no available fd.
 */
static int 
get_avail_fd (struct thread *t)
{
  int avail_fd = 2; /* 0 and 1 are reserved */
  struct list_elem *e;

  for (e = list_begin (&t->file_list); e != list_end (&t->file_list);
       e = list_next (e))
    {
      struct file *f = list_entry (e, struct file, file_elem);
      if(avail_fd == f->fd)
        ++avail_fd;
      else
        return avail_fd;
    }

  if(avail_fd >= 128)
    /* exceeds the limit of 128 open files per process */
    return -1;
  else
    return avail_fd;
}

static bool
less (const struct list_elem *a, const struct list_elem *b, void *aux UNUSED)
{
  return list_entry (a, struct file, file_elem)->fd 
    < list_entry (b, struct file, file_elem)->fd;
}

/* add file to thread 
 * return value : true (success), false (fail)
 */
bool 
thread_add_file (struct thread *t, struct file *file)
{
  int avail_fd = get_avail_fd (t);

  if(avail_fd == -1)
    return false;

  file->fd = avail_fd;
  list_insert_ordered (&t->file_list, &file->file_elem, less, NULL);
  return true;
}

/* get file from fd of thread 
 * return value : file (success), NULL (fail)
 */
struct file *
thread_get_file (struct thread *t, int fd)
{
  struct list_elem *e;

  for (e = list_begin (&t->file_list); e != list_end (&t->file_list);
       e = list_next (e))
    {
      struct file *t = list_entry (e, struct file, file_elem);
      if(t->fd == fd)
        return t;
    }

  /* fd does not exist */
  return NULL; 
}

/* remove file from thread */
void
thread_remove_file (struct thread *t, struct file *file, thread_file_action_func *action_func)
{
  list_remove (&(file->file_elem));
  if (action_func != NULL)
    action_func (file, NULL);
}

/* remove all files from thread. and execute action_func to all files. if
 * action_func is NULL, execute nothing. */
void 
thread_clear_file_list (struct thread *t, thread_file_action_func *action_func)
{
  while(!list_empty (&t->file_list))
    {
      struct file *f = list_entry ( list_pop_front (&t->file_list), struct file, file_elem);
      if (action_func != NULL)
        action_func(f, NULL);
    }
}
#endif
