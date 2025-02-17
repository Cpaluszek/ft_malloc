#include "chunk.h"
#include "malloc.h"
#include "print.h"

void free_large_chunk(chunkptr c);
void free_chunk_in_zone(chunkptr c, zone* z);

void free(void* ptr) {
    if (ptr == NULL) return;
    if (is_aligned_to_16(ptr) == 0) return;

    chunkptr c = get_chunk_from_ptr(ptr);

    if (state.perturb != 0) {
        ft_memset(ptr, state.perturb ^ 0xFF, get_chunk_size(c) - CHUNK_HEADER_SIZE); 
    }

    if (is_chunk_free(c)) return;

    size_t chunk_size = get_chunk_size(c);

    set_chunk_free(c);

    if (is_chunk_mmap(c)) {
        free_large_chunk(c);
        return;
    }

    free_chunk_in_zone(c, get_available_zone(chunk_size - CHUNK_HEADER_SIZE));
}

void free_chunk_in_zone(chunkptr c, zone* z) {
    while (z != NULL) {
        if (is_chunk_in_zone(c, z) == 1) {
            break ;
        }
        z = z->next;
    }

    if (z == NULL) {
        printf_fd(STDERR, "Error: Chunk at address %p does not belong to any zone\n", c);
        return;
    }

    insert_chunk_in_free_list(c, z);
}

void free_large_chunk(chunkptr c) {
    chunkptr prev = NULL;
    chunkptr current = state.large_chunks;
    while (current != NULL) {

        if (current == c) {
            break;
        }
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf_fd(STDERR, "Error: Chunk %p not found\n", c);
        return;
    }

    if (prev != NULL) {
        prev->next = current->next;
    } else {
        state.large_chunks = current->next;
    }

    if (munmap(c, get_chunk_size(c)) != 0) {
        printf_fd(STDERR, "Error: %s\n", strerror(errno));
    }
}
