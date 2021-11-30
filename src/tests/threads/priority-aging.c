#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/init.h"
#include "threads/thread.h"

static thread_func aging_thread;

void
test_priority_aging (void) 
{
  /* This test does not work with the MLFQS. */
  ASSERT (!thread_mlfqs);

  msg ("Creating a default-priority thread 2.");
  thread_create ("thread 2", PRI_DEFAULT, aging_thread, NULL);
  msg ("Main thread change its priority and have just lowered thread 2's priority.");
  msg ("From now, main thread can't get CPU without aging.");
  thread_set_priority (PRI_DEFAULT - 2);
  msg ("But main thread exiting.");
  msg ("Success!");
}

static void
aging_thread (void *aux UNUSED)
{
  while (1);
}
