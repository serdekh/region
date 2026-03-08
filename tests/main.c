#include <stdio.h>
#include "../src/region.h"

bool test_region_log_error__NOT_ENOUGH_MEMORY(ErrorCode expected, ErrorCode *actual)
{
    RegionError error = {0};

    Region *r = region_alloc(__SIZE_MAX__, &error);

    *actual = error.code;

    if (r) region_free(&r);

    return error.code == REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY;
}

int main()
{
    ErrorCode expected = REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY;
    ErrorCode actual;

    bool r = test_region_log_error__NOT_ENOUGH_MEMORY(expected, &actual);

    if (!r) {
        fprintf(stderr, "[Region][Test]: region_log_error failed\n\tExpected: %d\n\tActual: %d\n",
            expected, actual);
        return 1;
    }

    printf("Passed!\n");
    return 0;
}