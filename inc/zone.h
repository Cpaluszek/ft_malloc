#ifndef ZONE_H
# define ZONE_H

#include "chunk.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

typedef struct s_zone {
    size_t size;            // Total size of the zone
    struct s_zone* next;    // Next zone
    chunkptr free_list;     // Ptr to the free list
    char data[];            // Start of the actual memory for allocations
} zone;

void free_zone(zone* zone);
zone* init_zone(uint64_t size);

chunkptr find_free_chunk(zone* zone, size_t required_size, chunkptr prev);
void split_chunk(chunkptr chunk, size_t size);

#endif
