#include "malloc.h"
#include "chunk.h"
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

malloc_state state;

// TODO: make calloc()

void* my_malloc(size_t size) {
    if (size <= 0) {
        return NULL;
    }

    if (size <= TINY_MAX_ALLOC_SIZE) {
        // create a chunk
        size_t aligned_size = (size + 15) & ~15;
        size_t required_size = aligned_size + CHUNK_HEADER_SIZE; // TODO: need to align the size with 16
        // search a free block
        chunkptr prev = NULL;
        chunkptr current = state.tiny->free_list;
        while (current != NULL) {
            if (current->is_free && current->size >= required_size) {
                break;
            }
            prev = current;
            current = current->next;
        }

        if (current == NULL) {
            return NULL; // out of memory
        }

        current->is_free = 0;

        // If there's excess space, split the chunk
        if (current->size > required_size + CHUNK_HEADER_SIZE) {
            chunkptr new_chunk = (chunkptr)((char*)current + required_size);
            new_chunk->size = current->size - required_size;
            new_chunk->is_free = 1;
            new_chunk->next = current->next;

            current->size = required_size;
            current->next = new_chunk;
        }

        // update the free list head if needed
        if (prev == NULL) {
            state.tiny->free_list = current->next;
        } else {
            prev->next = current->next;
        }
        return (void*)((char*)current + CHUNK_HEADER_SIZE);
    }

    return NULL;
}

__attribute__((constructor(101)))
void malloc_state_init(void) {
    state.tiny = init_zone(TINY_HEAP_SIZE);
    if (state.tiny == NULL) {
        exit(1);  // TODO: manage error properly
    }

    state.small = init_zone(SMALL_HEAP_SIZE);
    if (state.small == NULL) {
        free_zone(state.tiny);
        exit(1);
    }
}

__attribute__((destructor(101)))
void malloc_state_deinit(void) {
    // TODO: check error
    if (state.tiny) {
        free_zone(state.tiny);
    }
    if (state.small) {
        free_zone(state.small);
    }
}

