#include "malloc.h"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/logging.h>
#include <criterion/parameterized.h>
#include <criterion/new/assert.h>
#include <stddef.h>
#include <stdint.h>

static size_t params[] = {
    1,
    32,
    128,
    256,
    1024,
    4096,
    5555,
    66666,
};

size_t nb_params = sizeof(params) / sizeof(size_t);

ParameterizedTestParameters(malloc, simple) {
    return cr_make_param_array(size_t, params, nb_params);
}
ParameterizedTestParameters(malloc, read_write) {
    return cr_make_param_array(size_t, params, nb_params);
}
ParameterizedTestParameters(malloc, repeat_alloc_and_free) {
    return cr_make_param_array(size_t, params, nb_params);
}
ParameterizedTestParameters(calloc, calloc) {
    return cr_make_param_array(size_t, params, nb_params);
}

int is_memory_algigned(void *ptr) {
    return ((uintptr_t)ptr % (2 * sizeof(size_t))) == 0;
}

Test(malloc, zero) {
    void *ptr = my_malloc(0);
    cr_assert_not_null(ptr, "malloc failed to allocate memory (0)");
    cr_assert(is_memory_algigned(ptr), "address %p is not aligned for size: 0", ptr);
    my_free(ptr);
}

ParameterizedTest(size_t* size, malloc, simple) {
    void *ptr = my_malloc(*size);
    cr_assert_not_null(ptr, "malloc failed to allocate memory (%ld)", *size);
    cr_assert(is_memory_algigned(ptr), "address %p is not aligned for size: %ld", ptr, *size);
    show_alloc_mem();
    my_free(ptr);
}

ParameterizedTest(size_t* size, malloc, read_write) {
    char* ptr = my_malloc(*size);
    cr_assert_not_null(ptr, "malloc failed to allocate memory (%ld)", *size);
    memset(ptr, 'A', *size);
    for (size_t i = 0; i < *size; i++) {
        cr_assert_eq(ptr[i], 'A', "Memory content mismatch at index %zu", i);
    }
    my_free(ptr);
}

ParameterizedTest(size_t* size, malloc, repeat_alloc_and_free) {
    int repetition_count = 10;
    for (int i = 0; i < repetition_count; i++) {
        char* ptr = my_malloc(*size);
        cr_assert_not_null(ptr, "malloc failed to allocate memory (%ld)", *size);
        // show_alloc_mem();
        my_free(ptr);
    }
}

ParameterizedTest(size_t* size, calloc, calloc) {
    char* ptr = my_calloc(*size);
    cr_assert_not_null(ptr, "malloc failed to allocate memory (%ld)", *size);
    for (size_t i = 0; i < *size; i++) {
        cr_assert_eq(ptr[i], 0, "Memory content mismatch at index %zu", i);
    }
    my_free(ptr);
}

Test(malloc, alloc_all) {
    for (size_t i = 0; i < nb_params; i++) {
        char *ptr = my_malloc(params[i]);
        cr_assert_not_null(ptr, "malloc failed to allocate memory (%ld)", params[i]);
    }
    show_alloc_mem();
}

Test(malloc, saturate_tiny_page) {
    for (int i = 0; i < 120; i++) {
        char *ptr = my_malloc(TINY_MAX_ALLOC_SIZE);
        cr_assert_not_null(ptr, "malloc failed to allocate memory (%d)", TINY_MAX_ALLOC_SIZE);
    }
    show_alloc_mem();
}

Test(malloc, saturate_small_page) {
    for (int i = 0; i < 101; i++) {
        char *ptr = my_malloc(SMALL_MAX_ALLOC_SIZE);
        cr_assert_not_null(ptr, "malloc failed to allocate memory (%d)", SMALL_MAX_ALLOC_SIZE);
    }
    show_alloc_mem();
}

Test(malloc, merge_free_chunks_tiny) {
    void* ptr1 = my_malloc(TINY_MAX_ALLOC_SIZE);
    cr_assert_not_null(ptr1, "malloc failed to allocate memory");

    void* ptr2 = my_malloc(TINY_MAX_ALLOC_SIZE);
    cr_assert_not_null(ptr2, "malloc failed to allocate memory");

    void* ptr3 = my_malloc(TINY_MAX_ALLOC_SIZE);
    cr_assert_not_null(ptr3, "malloc failed to allocate memory");
    show_alloc_mem();

    my_free(ptr2);
    show_alloc_mem();

    my_free(ptr3);
    show_alloc_mem();

    my_free(ptr1);
    show_alloc_mem();
}

// Realloc
Test(realloc, realloc_basic) {
    for (size_t i = 1; i < nb_params; i++) {
        // Allocate initial memory
        cr_log_error("> %ld\n", params[i]);
        char *ptr = my_malloc(params[i]);
        cr_assert_not_null(ptr, "malloc failed to allocate memory (%ld)", params[i]);
        show_alloc_mem();

        // Fill with known pattern
        for (size_t j = 0; j < params[i]; j++) {
            ptr[j] = (j % 256);
        }

        // // Copy original data before realloc
        // char *backup = my_malloc(params[i]);  // Standard malloc to avoid interference
        // cr_assert_not_null(backup, "Failed to allocate backup memory");
        // memcpy(backup, ptr, params[i]);

        // Reallocate to a larger size
        size_t new_size = params[i] * 2;
        char *new_ptr = my_realloc(ptr, new_size);
        cr_assert_not_null(new_ptr, "realloc failed to reallocate memory to larger size (%ld)", new_size);
        show_alloc_mem();

        // Ensure memory contents are preserved
        // for (size_t j = 0; j < params[i]; j++) {
        //     cr_assert_eq(backup[j], new_ptr[j], "Memory content mismatch after realloc at index %zu: %d != %d", j, backup[j], new_ptr[j]);
        // }
        my_free(new_ptr);
    }
}

Test(realloc, realloc_shrink) {
    for (size_t i = 1; i < nb_params; i++) {
        cr_log_error("> %ld\n", params[i]);
        // Allocate initial memory
        char *ptr = my_malloc(params[i]);
        cr_assert_not_null(ptr, "malloc failed to allocate memory (%ld)", params[i]);
        show_alloc_mem();

        // Reallocate to a smaller size
        size_t new_size = params[i] / 2;
        char *new_ptr = my_realloc(ptr, new_size);
        cr_assert_not_null(new_ptr, "realloc failed to reallocate memory to smaller size (%ld)", new_size);
        show_alloc_mem();
        //
        my_free(new_ptr);
    }
}

Test(realloc, realloc_zero_size) {
    // Allocate initial memory
    char *ptr = my_malloc(128);
    cr_assert_not_null(ptr, "malloc failed to allocate memory");

    // Reallocate to 0, should behave like free
    char *new_ptr = my_realloc(ptr, 0);
    cr_assert_null(new_ptr, "realloc with 0 should return NULL");

    // Ensure memory was freed
    // show_alloc_mem();  // Ensure no memory is left allocated
}

Test(realloc, realloc_null_ptr) {
    // Reallocate with a NULL pointer, should behave like malloc
    char *new_ptr = my_realloc(NULL, 128);
    cr_assert_not_null(new_ptr, "realloc(NULL, size) should behave like malloc");

    my_free(new_ptr);
}

Test(bonus, hex_dump) {
    void* str = my_malloc(32);
    cr_assert_not_null(str, "malloc failed to allocate memory (32)");
    if (str) {
        strcpy(str, "Hello, memory allocation!");
    }
    show_alloc_mem_ex();
    my_free(str);
}

Test(bonus, perturb) {
    void* ptr = my_malloc(TINY_MAX_ALLOC_SIZE);
    cr_assert_not_null(ptr, "malloc failed to allocate memory");
    show_alloc_mem();
    show_alloc_mem_ex();

    my_free(ptr);
    show_alloc_mem();
    show_alloc_mem_ex();
}

Test(multiple, multiple) {
    size_t M = 1024 * 1024;
    void* a = my_malloc(1);
    a = my_malloc(2);
    a = my_malloc(4);
    a = my_malloc(8);
    a = my_malloc(16);
    a = my_malloc(32);
    a = my_malloc(64);
    a = my_malloc(128);
    a = my_malloc(256);
    a = my_malloc(512);
    a = my_malloc(1024);
    a = my_malloc(1024 * 2);
    a = my_malloc(1024 * 4); 
    a = my_malloc(1024 * 32);
    a = my_malloc(M);
    a = my_malloc(16*M);
    a = my_malloc(128*M);
    show_alloc_mem();
    my_free(a);
}

