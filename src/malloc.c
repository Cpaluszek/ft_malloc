#include "malloc.h"
#include "chunk.h"

extern malloc_state state;

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
            void *ptr = (void*)((char*)current + CHUNK_HEADER_SIZE);
            if (state.perturb != 0) {
                ft_memset(ptr, state.perturb, get_chunk_size(current) - CHUNK_HEADER_SIZE); 
            }
            return ptr;
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

    void *ptr = (void*)((char*)current + CHUNK_HEADER_SIZE);
    set_chunk_in_use(current);
    if (state.perturb != 0) {
        ft_memset(ptr, state.perturb, get_chunk_size(current) - CHUNK_HEADER_SIZE); 
    }

    return ptr;
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

    void *ptr = (void*)((char*)new + CHUNK_HEADER_SIZE);
    if (state.perturb != 0) {
        ft_memset(ptr, state.perturb, get_chunk_size(new) - CHUNK_HEADER_SIZE); 
    }

    chunk_add_back(&state.large_chunks, new);
    return ptr;
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

