#include "malloc.h"
#include "chunk.h"
#include "zone.h"
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

malloc_state state;

// TODO: make calloc()

static inline size_t align_to_16(size_t size) {
    return (size + 15) &~ 15;
}

void* my_malloc(size_t size) {
    if (size <= SMALL_MAX_ALLOC_SIZE) {
        zone* zone = get_available_zone(size);

        size_t required_size = align_to_16(size) + CHUNK_HEADER_SIZE;

        chunkptr prev = NULL;
        chunkptr current = find_free_chunk(zone, required_size, prev);

        if (current == NULL) {
            // TODO: we need a new zone
            return NULL; // out of memory
        }

        // If there's excess space, split the chunk
        split_chunk(current, required_size);

        // update the free list head if needed
        if (prev == NULL) {
            zone->free_list = current->next;
        } else {
            prev->next = current->next;
        }
        return (void*)((char*)current + CHUNK_HEADER_SIZE);
    } else {
        return allocate_large_chunk(size);
    }

    return NULL;
}

void* allocate_large_chunk(size_t size) {
    size_t required_size = align_to_16(size) + CHUNK_HEADER_SIZE;

    chunkptr new = mmap(NULL, required_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (new == MAP_FAILED) {
        printf_fd(STDERR, "Error mmap: %s", strerror(errno));
        // TODO: manage error
    }
    new->size = required_size;
    set_chunk_in_use(new);
    new->next = NULL;

    chunk_add_back(&state.large_chunks, new);
    return (void*)((char*)new + CHUNK_HEADER_SIZE);
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
    state.large_chunks = NULL;
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
    chunkptr next;
    while (state.large_chunks != NULL) {
        next = state.large_chunks->next;
        if (munmap(state.large_chunks, state.large_chunks->size) != 0) {
            printf_fd(STDERR, "Error: %s", strerror(errno));
        }
        state.large_chunks = next;
    }
}

zone* get_available_zone(size_t size) {
    // TODO: check if the zone is full and check next one or create it
    if (size <= TINY_MAX_ALLOC_SIZE) {
        return state.tiny;
    } else {
        return state.small;
    }
    return NULL;
}

