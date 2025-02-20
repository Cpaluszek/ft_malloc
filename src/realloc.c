#include "chunk.h"
#include "malloc.h"
#include "zone.h"

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

    zone* z = get_zone_from_chunk(c);
 
    if (prev_size == required_size) {
        return ptr;
    }

    // Large chunk are completely reallocated
    if (is_chunk_mmap(c) == 1) {
        return realloc_new_memory(ptr, size, prev_size);
    }

    // Chunks size limits, if it belongs to a zone
    size_t max_chunk_size = (z && z->size == TINY_HEAP_SIZE) ? TINY_MAX_ALLOC_SIZE : SMALL_MAX_ALLOC_SIZE;
    size_t min_chunk_size = (z && z->size == TINY_HEAP_SIZE) ? 0 : TINY_MAX_ALLOC_SIZE;

    // If the chunk remains in the same zone
    if (required_size > min_chunk_size && required_size <= max_chunk_size) {
        // If the current chunk is already large enough
        if (0 && prev_size > required_size) {
            split_chunk(c, required_size, z);
            set_chunk_in_use(c);

            insert_chunk_in_free_list(c->next, z);
            return ptr;
        }

        chunkptr next = (chunkptr)((char*)c + get_chunk_size(c));
        // chunkptr next = c->next;
        // Is there space right after the current chunk and the next chunk is large enough
        if (0 && is_chunk_in_zone(next, z) && is_chunk_free(next) && prev_size + get_chunk_size(next) >= required_size) {
            // We can merge the next block - no copy required
            merge_chunk_with_next(c);
            set_chunk_in_use(c);

            // split after the merge
            if (get_chunk_size(c) > required_size + min_chunk_size) {
                split_chunk(c, required_size, z);
                insert_chunk_in_free_list(c->next, z);
            }
           return ptr;
        }
    }
    // Allocate new memory zone
    return realloc_new_memory(ptr, size, prev_size);
}

void* realloc_new_memory(void* ptr, size_t size, size_t prev_size) {
    void* new = malloc(size);

    if (new != NULL) {
        // TODO: create min fucntion
        size_t copy_size = (prev_size < size) ? prev_size : size;
        memory_copy(new, ptr, copy_size);
    }

    free(ptr);
    return new;
}

