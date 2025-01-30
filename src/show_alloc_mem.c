#include "chunk.h"
#include "malloc.h"

void show_zone_mem(zone* z, char* zone_type);
void show_chunks_mem(chunkptr c, int is_large);
void print_chunk_info(chunkptr c, int is_large);

// NOTE: show_alloc_mem print aligned memory and not real allocation
void show_alloc_mem(void) {
    printf_fd(STDOUT, "===== MEMORY LAYOUT =====\n");

    show_zone_mem(state.tiny, "TINY");

    show_zone_mem(state.small, "SMALL");

    if (state.large_chunks != NULL) {
        show_chunks_mem(state.large_chunks, 1);
    }
    // TODO: total allocated
}

void show_zone_mem(zone* z, char* zone_type) {
    while (z != NULL) {
        printf_fd(STDOUT, "%s%s%s:\t%p\n", COLOR_BLUE, zone_type, COLOR_RESET, z);
        chunkptr current = (chunkptr)z->data;
        int chunk_count = 0;
        while (current != NULL && (char*)current != (char*)z + z->size) {
            print_chunk_info(current, 0);
            chunk_count++;
            current = current->next;
        }
        printf_fd(STDOUT, "chunk_count: %d\n", chunk_count);
        z = z->next;
    }
}

void show_chunks_mem(chunkptr c, int is_large) {
    while (c) {
        print_chunk_info(c, is_large);
        c = c->next;
    }
}

void print_chunk_info(chunkptr c, int is_large) {
    void* payload_start = (void*)((char*)c + CHUNK_HEADER_SIZE);
    void* payload_end = (void*)((char*)payload_start + (get_chunk_size(c) - CHUNK_HEADER_SIZE));
    size_t allocated_size = get_chunk_size(c) - CHUNK_HEADER_SIZE;
    if (is_large) {
        printf_fd(STDOUT, "%sLARGE%s:\t%p\n", COLOR_BLUE, COLOR_RESET, c);
    }
    printf_fd(STDOUT, "| %p ", c);

    if (is_chunk_free(c)) {
        // Single-line output for free chunks
        printf_fd(STDOUT, "%sFree%s - [%p - %p] : %z bytes\n", COLOR_GREEN, COLOR_RESET, payload_start, payload_end, allocated_size);
    } else {
        printf_fd(STDOUT, "%sUsed%s - [%p - %p] : %z bytes\n", COLOR_RED, COLOR_RESET, payload_start, payload_end, allocated_size);
    }
}

