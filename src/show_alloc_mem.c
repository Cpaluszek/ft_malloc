#include "chunk.h"
#include "malloc.h"

size_t show_zone_mem(zone* z, char* zone_type);
size_t show_chunks_mem(chunkptr c, int is_large);
size_t print_chunk_info(chunkptr c, int is_large);

// NOTE: show_alloc_mem print aligned memory and not real allocation
void show_alloc_mem(void) {
    size_t total_allocated_memory = 0;

    printf_fd(STDOUT, "===== MEMORY LAYOUT =====\n");

    total_allocated_memory += show_zone_mem(state.tiny, "TINY");

    total_allocated_memory += show_zone_mem(state.small, "SMALL");

    if (state.large_chunks != NULL) {
        total_allocated_memory += show_chunks_mem(state.large_chunks, 1);
    }
    printf_fd(STDOUT, "Total allocated memory: %z\n", total_allocated_memory);
}

size_t show_zone_mem(zone* z, char* zone_type) {
    size_t allocated_memory = 0;

    while (z != NULL) {
        printf_fd(STDOUT, "%s%s%s:\t%p\n", COLOR_BLUE, zone_type, COLOR_RESET, z);
        chunkptr current = (chunkptr)z->data;
        while (current != NULL && (char*)current != (char*)z + z->size) {
            allocated_memory += print_chunk_info(current, 0);
            current = current->next;
        }
        z = z->next;
    }
    return allocated_memory;
}

size_t show_chunks_mem(chunkptr c, int is_large) {
    size_t allocated_memory = 0;
    while (c) {
        allocated_memory += print_chunk_info(c, is_large);
        c = c->next;
    }
    return allocated_memory;
}

size_t print_chunk_info(chunkptr c, int is_large) {
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
        return 0;
    }
    printf_fd(STDOUT, "%sUsed%s - [%p - %p] : %z bytes\n", COLOR_RED, COLOR_RESET, payload_start, payload_end, allocated_size);
    return get_chunk_size(c);
}

