#include "malloc.h"
#include <stdio.h>

// NOTE: show_alloc_mem print aligned memory and not real allocation
void show_alloc_mem(void) {
    printf("TINY:\t%p\n", state.tiny);
    show_zone_mem(state.tiny);
    printf("SMALL:\t%p\n", state.small);
    show_zone_mem(state.small);
}

void show_zone_mem(zone* zone) {
    if (zone == NULL) {
        return;
    }

    chunkptr current = (chunkptr)zone->data;
    while (current != NULL && (char*)current != (char*)zone + zone->size) {
        if (!current->is_free) {
            void* payload_start = (void*)((char*)current + CHUNK_HEADER_SIZE);
            void* payload_end = (void*)((char*)payload_start + (current->size - CHUNK_HEADER_SIZE));
            uint64_t allocated_size = current->size - CHUNK_HEADER_SIZE;
            printf("%p - %p : %lu bytes\n", payload_start, payload_end, allocated_size);
        }
        current = current->next;
    }
}
