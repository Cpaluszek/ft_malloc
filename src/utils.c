#include <stddef.h>
#include <stdint.h>

void ft_bzero(void *ptr, size_t n) {
    char *s = ptr;
    for (size_t i = 0; i < n; i++) {
        s[i] = 0;
    }
}

void memory_copy(void *dst, const void *src, size_t n) {
    uint8_t* dst_array = dst;
    const uint8_t* src_array = src;
    for (size_t i = 0; i < n; i++) {
        dst_array[i] = src_array[i];
    }
}

size_t align_to_16(size_t size) {
    return (size + 15) &~ 15;
}

int is_aligned_to_16(void *ptr) {
    return ((uintptr_t)ptr & 15) == 0;
}
