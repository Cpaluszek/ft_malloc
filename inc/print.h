#ifndef PRINT_H
#define PRINT_H

#include <unistd.h>
#include <stdarg.h>

#define STDOUT 1
#define STDERR 2

#define NULL_STR "(null)"
#define HEX_BASE_LOWER "0123456789abcdef"
#define HEX_BASE_UPPER "0123456789ABCDEF"
#define PTR_PREFIX "0x"

int printf_fd(int fd, const char *input, ...);
int pick_conversion_fd(int fd, const char c, va_list args);
int print_decimal_fd(const int nb, int fd);
int print_string_fd(const char *str, int fd);
int print_pointer_fd(size_t ptr, int fd);
int print_hex_fd(size_t nb, int fd, char* base);
int print_unsigned_int_fd(unsigned int nb, int fd);
int print_size_t_fd(size_t nb, int fd);
int print_size_t_base_fd(size_t nb, char *base, size_t b_len, int fd);

size_t string_length(const char *str);

#endif
