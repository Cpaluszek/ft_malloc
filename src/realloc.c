#include "chunk.h"
#include "malloc.h"
#include "zone.h"

void* my_realloc(void* ptr, size_t size) {
    // realloc(NULL, size) behaves like malloc(size)
    if (ptr == NULL) {
        return my_malloc(size);
    }

    // realloc(ptr, 0) is equivalent to free(ptr)
    if (size == 0) {
        my_free(ptr);
        return NULL;
    }

    chunkptr c = get_chunk_from_ptr(ptr);
    size_t prev_size = get_chunk_size(c);
    size_t required_size = align_to_16(size) + CHUNK_HEADER_SIZE;

    zone* z = get_zone_from_chunk(c);

    size_t max_chunk_size = (z && z->size == TINY_HEAP_SIZE) ? TINY_MAX_ALLOC_SIZE : SMALL_MAX_ALLOC_SIZE;
    size_t min_chunk_size = (z && z->size == TINY_HEAP_SIZE) ? 0 : TINY_MAX_ALLOC_SIZE;

    if (prev_size == required_size) {
        return ptr;
    }
 
    // If the current chunk is already large enough
    if (prev_size > required_size) {
        if (is_chunk_mmap(c) == 0 && (prev_size > required_size + min_chunk_size)) {
            split_chunk(c, required_size);
            insert_chunk_in_free_list(c->next, z);
            return ptr;
        }
    } else if (size <= SMALL_MAX_ALLOC_SIZE) { // Try expanding into the next chunk 
        // Check if the current zone can hold the new allocation
        if (required_size <= max_chunk_size && required_size >= min_chunk_size) {
            // Is there space right after the current chunk
            chunkptr next = (chunkptr)((char*)c + get_chunk_size(c));
            if (is_chunk_in_zone(next, z) == 1 && is_chunk_free(next)) {
                // We can merge the next block - no copy required
                merge_chunk_with_next(c);

                // split after the merge
                if (get_chunk_size(c) > required_size + min_chunk_size) {
                    split_chunk(c, required_size);
                    insert_chunk_in_free_list(c->next, z);
                }

                return ptr;
            }
        }
    }

    // Allocate new memory zone
    void* new = my_malloc(size);

    if (new != NULL) {
        size_t copy_size = (prev_size < size) ? prev_size : size;
        memory_copy(new, ptr, copy_size);
    }

    // Copy
    my_free(ptr);
    return new;
}

