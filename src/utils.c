#include <stddef.h>
#include <stdint.h>

void ft_bzero(void *ptr, size_t n) {
    char *s = ptr;
    size_t i = 0;
    while (i < n) {
        s[i] = 0;
        i++;
    }
}

size_t align_to_16(size_t size) {
    return (size + 15) &~ 15;
}

int is_aligned_to_16(void *ptr) {
    return ((uintptr_t)ptr & 15) == 0;
}
