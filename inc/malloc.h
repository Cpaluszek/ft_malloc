#ifndef MALLOC_H
# define MALLOC_H

#include <stdlib.h>
#include <sys/mman.h>

#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

#define TINY_BLOCK_SIZE 128
#define TINY_ZONE_SIZE (PAGE_SIZE * 32)
#define SMALL_BLOCK_SIZE 4096
#define SMALL_ZONE_SIZE (PAGE_SIZE * 128)

void my_free(void *ptr);
void *my_malloc(size_t size);
void *my_realloc(void *ptr, size_t size);

void print_infos(void);

#endif // MALLOC_H
