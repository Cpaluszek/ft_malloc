#include "malloc.h"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <stddef.h>
#include <stdint.h>

// Allocations
Test(malloc_test, tiny_allocation) {
    void *ptr = my_malloc(1);
    cr_assert_not_null(ptr, "malloc failed to allocate memory (1)");
    // my_free(ptr);
}
Test(malloc_test, small_allocation) {
    void *ptr = my_malloc(1);
    cr_assert_not_null(ptr, "malloc failed to allocate memory (1)");
    // my_free(ptr);
}
Test(malloc_test, large_allocation) {
    void *ptr = my_malloc(1);
    cr_assert_not_null(ptr, "malloc failed to allocate memory (1)");
    // my_free(ptr);
}

// Alignment test
int is_memory_aligned(void* ptr) {
    return ((uintptr_t)ptr % (2 * sizeof(size_t))) == 0;
}

Test(malloc_test, is_tiny_aligned) {
    void* ptr = my_malloc(61);
    cr_assert_not_null(ptr, "malloc failed to allocate memory (61)");
    cr_assert(is_memory_aligned(ptr));
    // my_free(ptr);
}

Test(malloc_test, is_small_aligned) {
    void* ptr = my_malloc(455);
    cr_assert_not_null(ptr, "malloc failed to allocate memory (455)");
    cr_assert(is_memory_aligned(ptr));
    // my_free(ptr);
}

Test(malloc_test, is_large_aligned) {
    void* ptr = my_malloc(5125);
    cr_assert_not_null(ptr, "malloc failed to allocate memory (5125)");
    cr_assert(is_memory_aligned(ptr));
    // my_free(ptr);
}

Test(show_mem, show_alloc_mem) {
    void* ptr = my_malloc(12);
    cr_assert_not_null(ptr, "malloc failed to allocate memory (12)");
    void* ptr2 = my_malloc(17);
    cr_assert_not_null(ptr2, "malloc failed to allocate memory (17)");
    show_alloc_mem();
}

// TODO: free
// TODO: zero alloc
// TODO: write
// TODO: read
/*
#define M (1024 * 1024)

int main()
{
    void* a = malloc(1);
    void* b = malloc(2);
    void* c = malloc(4);
    void* d = malloc(8);
    void* e = malloc(16);
    void* f = malloc(32);
    void* g = malloc(64);
    void* h = malloc(128);
    void* i = malloc(256);
    void* j = malloc(512);
    void* k = malloc(1024);
    void* l = malloc(1024 * 2);
    void* m = malloc(1024 * 4); 
    void* n = malloc(1024 * 32);
    void* o = malloc(M);
    void* p = malloc(16*M);
    void* q = malloc(128*M);
    show_alloc_mem(); 
    return (0); 
}
*/
