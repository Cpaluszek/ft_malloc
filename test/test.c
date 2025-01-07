#include "malloc.h"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>

Test(malloc_test, basic_allocation) {
    void *ptr = my_malloc(10);
    cr_assert_not_null(ptr, "malloc failed to allocate memory");
    free(ptr);
}

Test(malloc_test, zero_allocation) {
    void *ptr = my_malloc(0);
    cr_assert_not_null(ptr, "malloc failed to allocate memory");
    free(ptr);
}

Test(print, infos) {
    print_infos();
}
