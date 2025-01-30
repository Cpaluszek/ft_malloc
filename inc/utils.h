#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

void ft_bzero(void* s, size_t n);
size_t align_to_16(size_t size);
int is_aligned_to_16(void* ptr);

#endif
