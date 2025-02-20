#include "chunk.h"
#include "malloc.h"

void* realloc_new_memory(void* ptr, size_t size, size_t prev_size);

void* realloc(void* ptr, size_t size) {
    // realloc(NULL, size) behaves like malloc(size)
    if (ptr == NULL) {
        return malloc(size);
    }

    // realloc(ptr, 0) is equivalent to free(ptr)
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    chunkptr c = get_chunk_from_ptr(ptr);
    size_t prev_size = get_chunk_size(c);
    size_t required_size = align_to_16(size) + CHUNK_HEADER_SIZE;

    if (prev_size == required_size) {
        return ptr;
    }

    // Allocate new memory zone
    return realloc_new_memory(ptr, size, prev_size);
}

void* realloc_new_memory(void* ptr, size_t size, size_t prev_size) {
    void* new = malloc(size);

    if (new != NULL) {
        size_t copy_size = (prev_size < size) ? prev_size : size;
        memory_copy(new, ptr, copy_size);
    }

    free(ptr);
    return new;
}

