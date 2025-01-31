#include <stddef.h>
#include <stdint.h>

void ft_bzero(void *ptr, size_t n) {
    char *s = ptr;
    for (size_t i = 0; i < n; i++) {
        s[i] = 0;
    }
}

void ft_memset(void *ptr, int c, size_t n) {
    char *s = ptr;
    for (size_t i = 0; i < n; i++) {
        s[i] = c;
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

int ft_isprint(int c) {
    return c >= 32 && c <= 126;
}

int ft_isdigit(int c) {
    return c >= '0' && c <= '9';
}

int ft_isspace(const int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

int ft_atoi(const char *nptr) {
    int i = 0;
    int sign = 1;
    long long int res = 0;

    while (nptr[i] && ft_isspace(nptr[i])) {
        i++;
    }

    if (nptr[i] == '-') {
        sign = -1;
        i++;
    } else if (nptr[i] == '+') {
        i++;
    }

    while (ft_isdigit(nptr[i])) {
        res = res * 10 + (nptr[i++] - '0');
    }
    return (int)res * sign;
}
