#include <stddef.h>

void ft_bzero(void *ptr, size_t n) {
    char *s = ptr;
    size_t i = 0;
    while (i < n) {
        s[i] = 0;
        i++;
    }
}
