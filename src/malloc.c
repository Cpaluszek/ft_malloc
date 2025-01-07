#include "malloc.h"
#include <unistd.h>
#include <stdio.h>

void* my_malloc(size_t size) {
    (void) size;

    return malloc(size);
    // void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // if (ptr == MAP_FAILED) {
        // return NULL;
    // }
    // return ptr;
}

void print_infos(void) {
    int tiny_capability = TINY_ZONE_SIZE / TINY_BLOCK_SIZE;
    int small_capability = SMALL_ZONE_SIZE / SMALL_BLOCK_SIZE;

    printf("Zones:\n");
    printf("Tiny: block size=%d - zone size=%ld - count=%d\n", TINY_BLOCK_SIZE, TINY_ZONE_SIZE, tiny_capability);
    printf("Small: block size=%d - zone size=%ld - count=%d\n", SMALL_BLOCK_SIZE, SMALL_ZONE_SIZE, small_capability);
}
