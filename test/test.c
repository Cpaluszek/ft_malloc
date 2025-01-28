#include "malloc.h"
#include <criterion/criterion.h>
#include <criterion/parameterized.h>
#include <criterion/new/assert.h>
#include <stddef.h>
#include <stdint.h>

static size_t params[] = {
    0,
    1,
    32,
    128,
    256,
    1024,
    5555,
};

size_t nb_params = sizeof(params) / sizeof(size_t);

ParameterizedTestParameters(malloc, simple) {
    return cr_make_param_array(size_t, params, nb_params);
}
ParameterizedTestParameters(malloc, read_write) {
    return cr_make_param_array(size_t, params, nb_params);
}

int is_memory_algigned(void *ptr) {
    return ((uintptr_t)ptr % (2 * sizeof(size_t))) == 0;
}

ParameterizedTest(size_t* size, malloc, simple) {
    void *ptr = my_malloc(*size);
    cr_assert_not_null(ptr, "malloc failed to allocate memory (%ld)", *size);
    show_alloc_mem();
    cr_assert(is_memory_algigned(ptr), "address %p is not aligned for size: %ld", ptr, *size);
    // my_free(ptr);
}

ParameterizedTest(size_t* size, malloc, read_write) {
    char* ptr = my_malloc(*size);
    cr_assert_not_null(ptr, "malloc failed to allocate memory (%ld)", *size);
    show_alloc_mem();
    memset(ptr, 'A', *size);
    for (size_t i = 0; i < *size; i++) {
        cr_assert_eq(ptr[i], 'A', "Memory content mismatch at index %zu", i);
    }
    // my_free(ptr);
}

// // TODO: free
// // TODO: zero alloc
// /*
// #define M (1024 * 1024)
//
// int main()
// {
//     void* a = malloc(1);
//     void* b = malloc(2);
//     void* c = malloc(4);
//     void* d = malloc(8);
//     void* e = malloc(16);
//     void* f = malloc(32);
//     void* g = malloc(64);
//     void* h = malloc(128);
//     void* i = malloc(256);
//     void* j = malloc(512);
//     void* k = malloc(1024);
//     void* l = malloc(1024 * 2);
//     void* m = malloc(1024 * 4); 
//     void* n = malloc(1024 * 32);
//     void* o = malloc(M);
//     void* p = malloc(16*M);
//     void* q = malloc(128*M);
//     show_alloc_mem(); 
//     return (0); 
// }
// */
