/* Verifies that memory mappings at address 0 are disallowed. */

#include <syscall.h>
#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
  int handle;
  
  CHECK ((handle = open ("sample.txt")) > 1, "open \"sample.txt\"");
  CHECK (mmap (handle, NULL) == MAP_FAILED, "try to mmap at address 0");
}

