#include "chunk.h"

void chunk_add_back(chunkptr* lst, chunkptr new) {
    if (new == NULL) {
        return;
    }

    chunkptr last;

    last = *lst;
    while (last && last->next) {
        last = last->next;
    }

    if (last) {
        last->next = new;
    } else {
        *lst = new;
    }
}

chunkptr get_chunk_from_ptr(void *ptr) {
    return (chunkptr) ((char*)ptr - CHUNK_HEADER_SIZE);
}

size_t get_chunk_size(chunkptr c) {
    return c->size & SIZE_MASK;
}

void set_chunk_in_use(chunkptr c) {
    c->size |= FLAG_IN_USE;
}

void set_chunk_free(chunkptr c) {
    c-> size &= ~FLAG_IN_USE;
}

int is_chunk_free(chunkptr c) {
    return (c->size & FLAG_IN_USE) == 0;
}

void set_chunk_mmap(chunkptr c) {
    c->size |= FLAG_IS_MMAP;
}

int is_chunk_mmap(chunkptr c) {
    return (c->size & FLAG_IS_MMAP);
}
