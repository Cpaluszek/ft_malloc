#include "zone.h"
#include "chunk.h"
#include "malloc.h"

zone* init_zone(uint64_t size) {
    zone* z = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (z == MAP_FAILED) {
        printf_fd(STDERR, "Error mmap: %s", strerror(errno));
        return NULL;
    }
    z->size = size;
    z->next = NULL;

    chunkptr initial_chunk = (chunkptr)z->data;
    initial_chunk->size = z->size - sizeof(zone);
    initial_chunk->next = NULL;
    set_chunk_free(initial_chunk);

    z->chunk_list = initial_chunk;
    return z;
}

void free_zone(zone* z) {
    zone* next = NULL;
    while (z != NULL) {
        next = z->next;
        if (munmap(z, z->size) != 0) {
            printf_fd(STDERR, "Error: %s", strerror(errno));
        }
        z = next;
    }
}

chunkptr find_free_chunk(zone* z, size_t required_size) {
    chunkptr current = z->chunk_list;
    while (current != NULL) {
        if (is_chunk_free(current) && get_chunk_size(current) >= required_size) {
            break;
        }

        current = current->next;
    }
    if (current == NULL) {
        return NULL;
    }

    split_chunk(current, required_size, z);

    return current;
}

void split_chunk(chunkptr c, size_t size, zone* z) {
    chunkptr new_chunk = (chunkptr)((char*)c + size);

    if (is_chunk_in_zone(new_chunk, z) == 0) {
        return ;
    }

    // Check if the chunk is large enought to be split
    size_t new_chunk_size = get_chunk_size(c) - size;
    if (new_chunk_size < CHUNK_HEADER_SIZE) {
        return ;
    }

    // We can split the chunk
    new_chunk->size = new_chunk_size;
    set_chunk_free(new_chunk);

    // Add the new chunk in the linked list
    new_chunk->next = c->next;
    c->next = new_chunk;

    c->size = size;
}

void insert_chunk_in_free_list(chunkptr c, zone *z) {
    chunkptr current = z->chunk_list;
    chunkptr prev = NULL;

    // Search for the previous chunk
    while (current != NULL && current < c) {
        prev = current;
        current = current->next;
    }


    if (prev != NULL && is_chunk_free(prev)) {
        merge_chunk_with_next(prev);
        c = prev;
    }
    if (c->next != NULL && is_chunk_free(c->next)) {
        merge_chunk_with_next(c);
    }
}

void merge_chunk_with_next(chunkptr c) {
    chunkptr next = c->next;
    if (next != NULL && is_chunk_free(next)) {
        c->size += get_chunk_size(next);
        c->next = next->next;
    }
}

int is_chunk_in_zone(chunkptr c, zone* z) {
    void* zone_start = (void*)((char*)z + sizeof(zone));
    void* zone_end = (void*)((char*)z + z->size);

    return (void*)c >= zone_start && (void*)c < zone_end;
}

zone* get_zone_from_chunk(chunkptr c) {
    zone* z = get_available_zone(get_chunk_size(c) - CHUNK_HEADER_SIZE);
    while (z != NULL) {
        if (is_chunk_in_zone(c, z) == 1) {
            break ;
        }
        z = z->next;
    }
    return z;
}
