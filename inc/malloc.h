#ifndef MALLOC_H
# define MALLOC_H

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include "chunk.h"
#include "zone.h"
#include "utils.h"

#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

#define TINY_MAX_ALLOC_SIZE 128
#define TINY_CHUNK_SIZE (TINY_MAX_ALLOC_SIZE + CHUNK_HEADER_SIZE)
#define TINY_HEAP_SIZE (((100 * TINY_CHUNK_SIZE + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE)

#define SMALL_MAX_ALLOC_SIZE 4096
#define SMALL_CHUNK_SIZE (SMALL_MAX_ALLOC_SIZE + CHUNK_HEADER_SIZE)
#define SMALL_HEAP_SIZE (((100 * SMALL_CHUNK_SIZE + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE)

typedef struct s_malloc_state {
    zone*   tiny;
    zone*   small;
    chunkptr large_chunks;
    size_t page_size;
    uint8_t perturb;
} malloc_state;

extern malloc_state state;

void *malloc(size_t size);
void *realloc(void* ptr, size_t size);
void *calloc(size_t nnemb, size_t size);
void free(void* ptr);

void show_alloc_mem(void);
void show_alloc_mem_ex(void);

void malloc_state_init(void);
void malloc_state_deinit(void);

zone* get_available_zone(size_t size);
size_t get_zone_size(size_t alloc_size);
void* allocate_large_chunk(size_t size);

#endif // MALLOC_H
