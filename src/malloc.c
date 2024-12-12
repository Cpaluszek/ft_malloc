#include "malloc.h"

void* malloc(size_t size) {
    printf("Custom malloc: size = %zu\n", size);

    void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        return NULL;
    }
    return ptr;
}
