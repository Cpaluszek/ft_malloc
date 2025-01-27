#include "zone.h"

zone* init_zone(uint64_t size) {
    zone* z = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (z == MAP_FAILED) {
        fprintf(stderr, "Error mmap: %s", strerror(errno));
        return NULL;
    }

    chunkptr initial_chunk = (chunkptr)z->data;
    initial_chunk->size = z->size - sizeof(zone);
    initial_chunk->is_free = 1;
    initial_chunk->next = NULL;

    z->size = size;
    z->next = NULL;
    z->free_list = initial_chunk;
    return z;
}

void free_zone(zone* z) {
    zone* next = NULL;
    while (z != NULL) {
        next = z->next;
        int res = munmap(z, z->size);
        if (res != 0) {
            fprintf(stderr, "Error: %s", strerror(errno));
        }
        z = next;
    }
}
