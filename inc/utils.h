#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

void ft_bzero(void* s, size_t n);
void ft_memset(void *ptr, int c, size_t n);
void memory_copy(void *dst, const void *src, size_t n);

size_t align_to_16(size_t size);
int is_aligned_to_16(void* ptr);

int ft_isprint(int c);
int ft_isdigit(int c);
int ft_isspace(const int c);

int ft_atoi(const char *nptr);

#endif
