#ifndef MALLOC_H
# define MALLOC_H

#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>
#include "chunk.h"

#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

#define TINY_MAX_ALLOC_SIZE 128
#define TINY_CHUNK_SIZE (TINY_MAX_ALLOC_SIZE + CHUNK_HEADER_SIZE)
#define TINY_HEAP_SIZE (((100 * TINY_CHUNK_SIZE + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE)

#define SMALL_MAX_ALLOC_SIZE 4096
#define SMALL_CHUNK_SIZE (SMALL_MAX_ALLOC_SIZE + CHUNK_HEADER_SIZE)
#define SMALL_HEAP_SIZE (((100 * SMALL_CHUNK_SIZE + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE)

typedef struct s_zone {
    size_t size;            // Total size of the zone
    struct s_zone* next;    // Next zone
    chunkptr free_list;     // Ptr to the free list
    char data[];            // Start of the actual memory for allocations
} zone;

typedef struct s_malloc_state {
    zone*   tiny;
    zone*   small;
    chunkptr large_chunks;
    size_t page_size;
} malloc_state;

extern malloc_state state;

// TODO: do we want a top chunk?

void *my_malloc(size_t size);
void *my_realloc(void *ptr, size_t size);
void my_free(void *ptr);

void mallc_state_init(void);
void malloc_state_deinit(void);

zone* init_zone(uint64_t size);

void show_alloc_mem(void);
void show_zone_mem(zone* zone);

#endif // MALLOC_H
