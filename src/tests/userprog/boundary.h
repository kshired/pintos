#ifndef TESTS_USERPROG_BOUNDARY_H
#define TESTS_USERPROG_BOUNDARY_H

void *get_boundary_area (void);
char *copy_string_across_boundary (const char *);
void *get_bad_boundary (void);

#endif /* tests/userprog/boundary.h */
