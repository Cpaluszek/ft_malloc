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

    z->free_list = initial_chunk;
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
    chunkptr prev = NULL;
    chunkptr current = z->free_list;
    while (current != NULL) {
        if (is_chunk_free(current) && get_chunk_size(current) >= required_size) {
            break;
        }

        prev = current;
        current = current->next;
    }
    if (current == NULL) {
        return NULL;
    }

    split_chunk(current, required_size);

    if (prev == NULL) {
        z->free_list = current->next;
    } else {
        prev->next = current->next;
    }

    return current;
}

void insert_chunk_in_free_list(chunkptr c, zone *z) {
    chunkptr current = z->free_list;
    chunkptr prev = NULL;

    while (current != NULL && current < c) {
        prev = current;
        current = current->next;
    }

    // c->next = current;
    if (prev == NULL) {
        z->free_list = c;
    } else {
        prev->next = c;
        merge_chunk_with_next(prev);
        c = prev;
    }
    merge_chunk_with_next(c);
}

void split_chunk(chunkptr c, size_t size) {
    chunkptr new_chunk = (chunkptr)((char*)c + size);
    new_chunk->size = get_chunk_size(c) - size;
    set_chunk_free(new_chunk);
    new_chunk->next = c->next;

    c->size = size;
    set_chunk_in_use(c);
    c->next = new_chunk;
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
