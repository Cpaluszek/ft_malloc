#include "chunk.h"
#include "malloc.h"
#include "print.h"

void free_large_chunk(chunkptr c);
void free_chunk_in_zone(chunkptr c, zone* z);

void my_free(void* ptr) {
    if (ptr == NULL) return;
    if (is_aligned_to_16(ptr) == 0) return;

    chunkptr c = (chunkptr) ((char*)ptr - CHUNK_HEADER_SIZE);
    if (is_chunk_free(c)) return;

    size_t chunk_size = get_chunk_size(c);

    set_chunk_free(c);

    if (is_chunk_mmap(c)) {
        free_large_chunk(c);
        return;
    }

    if (chunk_size <= TINY_CHUNK_SIZE) {
        free_chunk_in_zone(c, state.tiny);
    } else {
        free_chunk_in_zone(c, state.small);
    }
}

int is_chunk_in_zone(chunkptr c, zone* z) {
    void* zone_start = (void*)((char*)z + sizeof(zone));
    void* zone_end = (void*)((char*)z + z->size);

    return (void*)c >= zone_start && (void*)c < zone_end;
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

    chunkptr current = z->free_list;
    chunkptr prev = NULL;

    // Find the correct place in the free_list
    while (current != NULL && current < c) {
        prev = current;
        current = current->next;
    }

    c->next = current;
    if (prev == NULL) {
        z->free_list = c;
    } else {
        prev->next = c;
        merge_chunk(prev);
    }
    merge_chunk(c);
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
