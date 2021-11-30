/* Mmaps a 128 kB file "sorts" the bytes in it, using quick sort,
   a multi-pass divide and conquer algorithm.  */

#include <debug.h>
#include <syscall.h>
#include "tests/lib.h"
#include "tests/main.h"
#include "tests/vm/qsort.h"

int
main (int argc UNUSED, char *argv[]) 
{
  int handle;
  unsigned char *p = (unsigned char *) 0x10000000;

  test_name = "child-qsort-mm";
  quiet = true;

  CHECK ((handle = open (argv[1])) > 1, "open \"%s\"", argv[1]);
  CHECK (mmap (handle, p) != MAP_FAILED, "mmap \"%s\"", argv[1]);
  qsort_bytes (p, 1024 * 128);
  
  return 80;
}
