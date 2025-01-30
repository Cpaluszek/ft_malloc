#ifndef ZONE_H
# define ZONE_H

#include "chunk.h"
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "print.h"
#include <sys/mman.h>

typedef struct s_zone {
    size_t size;            // Total size of the zone
    struct s_zone* next;    // Next zone
    chunkptr free_list;     // Ptr to the free list
    uint64_t _padding;       // Data for alignment
    char data[];            // Start of the actual memory for allocations
} zone;

void free_zone(zone* zone);
zone* init_zone(uint64_t size);

chunkptr find_free_chunk(zone* zone, size_t required_size);
void split_chunk(chunkptr c, size_t size);
void merge_chunk(chunkptr c);

#endif
