#include "print.h"

static int write_string(int fd, const char* str) {
    size_t len = string_length(str);
    return write(fd, str, len);
}

int printf_fd(int fd, const char *input, ...) {
    va_list args;

    va_start(args, input);
    int char_count = string_length(input);
    if (write(fd, 0, 0) == -1) {
        return -1;
    }

    int i = 0;
    while (input[i]) {
        if (input[i] == '%') {
            char_count += pick_conversion_fd(fd, input[++i], args) - 2;
        } else {
            write(fd, &input[i], 1);
        }
        if (input[i]) {
            i++;
        }
    }
    va_end(args);
    return char_count;
}

int pick_conversion_fd(int fd, const char c, va_list args) {
    if (c == 'd' || c == 'i')
        return print_decimal_fd(va_arg(args, int), fd);
    if (c == 's')
        return print_string_fd(va_arg(args, char *), fd);
    if (c == 'p')
        return print_pointer_fd(va_arg(args, size_t), fd);
    if (c == 'u')
        return print_unsigned_int_fd(va_arg(args, unsigned int), fd);
    if (c == 'z')
        return print_size_t_fd(va_arg(args, size_t), fd);
    if (c == 'x')
        return print_hex_fd(va_arg(args, unsigned int), fd, HEX_BASE_LOWER);
    if (c == 'X')
        return print_hex_fd(va_arg(args, unsigned int), fd, HEX_BASE_UPPER);
    if (c == 'c') {
        char c = va_arg(args, int);
        write(fd, &c, 1);
    }
    else if (c == '%')
        write(fd, "%", 1);
    return (1);
}

int print_decimal_fd(const int nb, int fd) {
    if (nb >= 0)
        return print_unsigned_int_fd(nb, fd);
    write(fd, "-", 1);
    return 1 + print_unsigned_int_fd(-nb, fd);
}

int print_string_fd(const char *str, int fd) {
    if (!str)
        str = NULL_STR;
    return write_string(fd, str);
}

int print_pointer_fd(size_t ptr, int fd) {
    size_t b_len = string_length(HEX_BASE_LOWER);

    write(fd, PTR_PREFIX, string_length(PTR_PREFIX));
    return 2 + print_size_t_base_fd(ptr, HEX_BASE_LOWER, b_len, fd);
}

int print_hex_fd(size_t nb, int fd, char *base) {
    size_t base_len = string_length(base);
    return print_size_t_base_fd(nb, base, base_len, fd);
}

int print_unsigned_int_fd(unsigned int nb, int fd) {
    int len = 1;

    if (nb > 9)
        len += print_unsigned_int_fd(nb / 10, fd);
    char c = '0' + (nb % 10);
    write(fd, &c, 1);
    return len;
}

int print_size_t_fd(size_t nb, int fd) {
    char buffer[26];
    int index = 0;

    do {
        buffer[index++] = '0' + (nb % 10);
        nb /= 10;
    } while (nb > 0);

    for (int i = index - 1; i >= 0; i--) {
        if (write(fd, &buffer[i], 1) == -1) {
            return -1;
        }
    }

    return index;
}

int print_size_t_base_fd(size_t nb, char *base, size_t b_len, int fd) {
    int char_count = 1;

    if (nb >= b_len)
        char_count += print_size_t_base_fd(nb / b_len, base, b_len, fd);
    write(fd, &base[nb % b_len], 1);
    return char_count;
}

size_t string_length(const char *str) {
    size_t len = 0;

    while (str[len]) {
        len++;
    }
    return len;
}
