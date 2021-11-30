/* Verifies that lowering a thread's priority so that it is no
   longer the highest-priority thread in the system causes it to
   yield immediately. */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/init.h"
#include "threads/thread.h"

static thread_func changing_thread;

void
test_priority_change_2 (void) 
{
  /* This test does not work with the MLFQS. */
  ASSERT (!thread_mlfqs);

  msg ("Creating a low-priority thread 2.");
  thread_create ("thread 2", PRI_DEFAULT - 2, changing_thread, NULL);
  msg ("Thread 2 should have just highered its priority.");
  thread_set_priority (PRI_DEFAULT - 3);
  msg ("Thread 2 should have just exited.");
}

static void
changing_thread (void *aux UNUSED) 
{
  msg ("Thread 2 now highering priority.");
  thread_set_priority (PRI_DEFAULT + 1);
  msg ("Thread 2 exiting.");
}
