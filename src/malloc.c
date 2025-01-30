#include "malloc.h"
#include "chunk.h"
#include "zone.h"
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

malloc_state state;

__attribute__((constructor(101)))
void malloc_state_init(void) {
    state.tiny = init_zone(TINY_HEAP_SIZE);
    if (state.tiny == NULL) {
        exit(1);
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
    free_zone(state.tiny);
    free_zone(state.small);
    chunkptr next;
    while (state.large_chunks != NULL) {
        next = state.large_chunks->next;
        if (munmap(state.large_chunks, get_chunk_size(state.large_chunks)) != 0) {
            printf_fd(STDERR, "Error: %s\n", strerror(errno));
        }
        state.large_chunks = next;
    }
}

// TODO: realloc(0) == free
// TODO: rename my_function() to function()

void* my_malloc(size_t size) {
    if (size > SMALL_MAX_ALLOC_SIZE) {
        return allocate_large_chunk(size);
    }

    size_t required_size = align_to_16(size) + CHUNK_HEADER_SIZE;
    zone* zone_list = get_available_zone(size);
    zone* last_zone = NULL;

    // Search for a free chunk in existing zones
    while (zone_list != NULL) {
        chunkptr current = find_free_chunk(zone_list, required_size);

        if (current != NULL) {
            set_chunk_in_use(current);
            return (void*)((char*)current + CHUNK_HEADER_SIZE);
        }

        last_zone = zone_list;
        zone_list = zone_list->next;
    }

    // No free chunk, allocate a new zone
    zone* new_zone = init_zone(get_zone_size(size));
    if (new_zone == NULL) {
        return NULL;
    }

    if (last_zone != NULL) {
        last_zone->next = new_zone;
    }

    chunkptr current = find_free_chunk(new_zone, required_size);
    if (current == NULL) {
        return NULL;
    }

    set_chunk_in_use(current);
    return (void*)((char*)current + CHUNK_HEADER_SIZE);
}

void *my_calloc(size_t size) {
    char* ptr = my_malloc(size);

    if (ptr != NULL) {
        ft_bzero(ptr, size);
    }

    return ptr;
}

void* allocate_large_chunk(size_t size) {
    size_t required_size = align_to_16(size) + CHUNK_HEADER_SIZE;

    chunkptr new = mmap(NULL, required_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (new == MAP_FAILED) {
        printf_fd(STDERR, "Error mmap: %s", strerror(errno));
        return NULL;
    }
    new->size = required_size;
    set_chunk_in_use(new);
    set_chunk_mmap(new);
    new->next = NULL;

    chunk_add_back(&state.large_chunks, new);
    return (void*)((char*)new + CHUNK_HEADER_SIZE);
}

zone* get_available_zone(size_t size) {
    if (size <= TINY_MAX_ALLOC_SIZE) {
        return state.tiny;
    } else {
        return state.small;
    }
    return NULL;
}

size_t get_zone_size(size_t alloc_size) {
    if (alloc_size <= TINY_MAX_ALLOC_SIZE) {
        return TINY_HEAP_SIZE;
    }
    return SMALL_HEAP_SIZE;
}

