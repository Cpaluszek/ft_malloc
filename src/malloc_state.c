#include "malloc.h"
#include "print.h"
#include <stdint.h>

malloc_state state;

__attribute__((constructor(101)))
void malloc_state_init(void) {
    if (state.tiny != NULL) {
        return ;
    }
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

    char *env = getenv("MALLOC_PERTURB_");
    if (env) {
        state.perturb = (uint8_t)ft_atoi(env);
    } else {
        state.perturb = 0;
    }
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
