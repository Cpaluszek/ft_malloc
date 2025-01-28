#include "chunk.h"
#include "malloc.h"
#include <stdio.h>

// NOTE: show_alloc_mem print aligned memory and not real allocation
void show_alloc_mem(void) {
    printf("===== MEMORY LAYOUT =====\n");
    printf("TINY:\t%p\n", state.tiny);
    show_zone_mem(state.tiny);
    printf("SMALL:\t%p\n", state.small);
    show_zone_mem(state.small);
    chunkptr large = state.large_chunks;
    while (large) {
        printf("LARGE:\t%p\n", large);
        void* payload_start = (void*)((char*)large+ CHUNK_HEADER_SIZE);
        void* payload_end = (void*)((char*)payload_start + (large->size - CHUNK_HEADER_SIZE));
        uint64_t allocated_size = get_chunk_size(large) - CHUNK_HEADER_SIZE;
        printf("  | Payload: [%p - %p] | Size:  %lu bytes\n", payload_start, payload_end, allocated_size);

        large = large->next;
    }
}

void show_zone_mem(zone* zone) {
    // TODO: iterate on zones
    if (zone == NULL) {
        return;
    }

    chunkptr current = (chunkptr)zone->data;
    while (current != NULL && (char*)current != (char*)zone + zone->size) {
        if (!is_chunk_free(current)) {
            void* payload_start = (void*)((char*)current + CHUNK_HEADER_SIZE);
            void* payload_end = (void*)((char*)payload_start + (current->size - CHUNK_HEADER_SIZE));
            uint64_t allocated_size = get_chunk_size(current) - CHUNK_HEADER_SIZE;
            printf("> Chunk: %p\n", current);
            printf("  | Payload: [%p - %p] | Size:  %lu bytes\n", payload_start, payload_end, allocated_size);
        }
        current = current->next;
    }
}
