#include "malloc.h"
#include "chunk.h"
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

malloc_state state;

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

__attribute__((constructor))
void mallooc_state_init(void) {
    state.tiny = init_zone(TINY_HEAP_SIZE);
    if (state.tiny == NULL) {
        exit(1);  // TODO: manage error properly
    }

    state.small = init_zone(SMALL_HEAP_SIZE);
    if (state.small == NULL) {
        munmap(state.tiny, state.tiny->size);
        exit(1);
    }
}

__attribute__((destructor))
void malloc_state_deinit(void) {
}

zone* init_zone(uint64_t size) {
    zone* z = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (z == NULL) {
        return NULL;
    }

    chunkptr initial_chunk = (chunkptr)z->data;
    initial_chunk->size = z->size - sizeof(zone);   // NOTE: sizeof() might not work because of char data[]
    initial_chunk->is_free = 1;
    initial_chunk->next = NULL;

    z->size = size;
    z->next = NULL;
    z->free_list = initial_chunk;
    return z;
}


