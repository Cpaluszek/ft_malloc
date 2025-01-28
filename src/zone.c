#include "zone.h"

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
        int res = munmap(z, z->size);
        if (res != 0) {
            printf_fd(STDERR, "Error: %s", strerror(errno));
        }
        z = next;
    }
}

chunkptr find_free_chunk(zone* zone, size_t required_size, chunkptr prev) {
    (void) prev;
    chunkptr current = zone->free_list;
    while (current != NULL) {
        if (is_chunk_free(current) && get_chunk_size(current) >= required_size) {
            break;
        }

        prev = current;
        current = current->next;
    }
    return current;
}

void split_chunk(chunkptr chunk, size_t size) {
    if (get_chunk_size(chunk) > size + CHUNK_HEADER_SIZE) {
        chunkptr new_chunk = (chunkptr)((char*)chunk + size);
        new_chunk->size = get_chunk_size(chunk) - size;
        set_chunk_free(new_chunk);
        new_chunk->next = chunk->next;

        chunk->size = size;
        set_chunk_in_use(chunk);
        chunk->next = new_chunk;
    }

}
