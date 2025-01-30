#include "malloc.h"

malloc_state state;

__attribute__((constructor(101)))
void malloc_state_init(void) {
    state.tiny = init_zone(TINY_HEAP_SIZE);
    if (state.tiny == NULL) {
        exit(1);
    }

    state.small = init_zone(SMALL_HEAP_SIZE);
    if (state.small == NULL) {
        free_zone(state.tiny);
        exit(1);
    }
    state.large_chunks = NULL;
}

__attribute__((destructor(101)))
void malloc_state_deinit(void) {
    free_zone(state.tiny);
    free_zone(state.small);
    chunkptr next;
    while (state.large_chunks != NULL) {
        next = state.large_chunks->next;
        if (munmap(state.large_chunks, get_chunk_size(state.large_chunks)) != 0) {
            printf_fd(STDERR, "Error: %s\n", strerror(errno));
        }
        state.large_chunks = next;
    }
}
