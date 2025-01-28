#include "chunk.h"

void chunk_add_back(chunkptr* lst, chunkptr new) {
    if (new == NULL) {
        return;
    }

    chunkptr last;

    last = *lst;
    while (last && last->next)
        last = last->next;
    if (last)
        last->next = new;
    else
        *lst = new;
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
