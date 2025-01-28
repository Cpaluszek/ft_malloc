#include "chunk.h"
#include "malloc.h"
#include <stdio.h>

void show_zone_mem(zone* z);
void show_chunks_mem(chunkptr c, int is_large);
void print_chunk_info(chunkptr c, int is_large);

// NOTE: show_alloc_mem print aligned memory and not real allocation
void show_alloc_mem(void) {
    printf("===== MEMORY LAYOUT =====\n");

    printf("TINY:\t%p\n", state.tiny);
    show_zone_mem(state.tiny);

    printf("SMALL:\t%p\n", state.small);
    show_zone_mem(state.small);

    if (state.large_chunks != NULL) {
        show_chunks_mem(state.large_chunks, 1);
    }
}

void show_zone_mem(zone* z) {
    zone* current_zone = z;
    while (current_zone != NULL) {
        chunkptr current = (chunkptr)z->data;
        while (current != NULL && (char*)current != (char*)z + z->size) {
            print_chunk_info(current, 0);
            current = current->next;
        }
        current_zone = current_zone->next;
    }
}

void show_chunks_mem(chunkptr c, int is_large) {
    while (c) {
        print_chunk_info(c, is_large);
        c = c->next;
    }
}

void print_chunk_info(chunkptr c, int is_large) {
    if (is_chunk_free(c)) {
        return;
    }

    void* payload_start = (void*)((char*)c + CHUNK_HEADER_SIZE);
    void* payload_end = (void*)((char*)payload_start + (c->size - CHUNK_HEADER_SIZE));
    uint64_t allocated_size = get_chunk_size(c) - CHUNK_HEADER_SIZE;
    if (is_large) {
        printf("LARGE:\t%p\n", c);
    } else {
        printf("> Chunk: %p\n", c);
    }
    printf("  | Payload: [%p - %p] | Size:  %lu bytes\n", payload_start, payload_end, allocated_size);
}

