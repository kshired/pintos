/* Executes itself recursively to the depth indicated by the
   first command-line argument. */

#include <debug.h>
#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include "tests/lib.h"

int
main (int argc UNUSED, char *argv[]) 
{
  int n = atoi (argv[1]);

  test_name = "multi-recurse";

  msg ("begin %d", n);
  if (n != 0) 
    {
      char child_cmd[128];
      pid_t child_pid;
      int code;
      
      snprintf (child_cmd, sizeof child_cmd, "multi-recurse %d", n - 1);
      CHECK ((child_pid = exec (child_cmd)) != -1, "exec(\"%s\")", child_cmd);

      code = wait (child_pid);
      if (code != n - 1)
        fail ("wait(exec(\"%s\")) returned %d", child_cmd, code);
    }
  
  msg ("end %d", n);
  return n;
}
