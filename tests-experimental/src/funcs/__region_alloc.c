#include "common.h"

#define SYMBOL_FN_REGION_ALLOC "__region_alloc"

bool region_alloc_case_1() 
{
    FuncPtr_region_alloc fptr_region_alloc = try_get_symbol(SYMBOL_FN_REGION_ALLOC);

    RegionError error = {0};

    fptr_region_alloc(0, &error, REGION_GET_CURRENT_FILE_LOCATION);

    return error.code == REGION_ERROR_CODE_EINVAL_REGION_ALLOC_SMALL_CAPACITY;
}

bool region_alloc_case_2() 
{
    FuncPtr_region_alloc fptr_region_alloc = try_get_symbol(SYMBOL_FN_REGION_ALLOC);

    RegionError error = {0};

    fptr_region_alloc(__SIZE_MAX__, &error, REGION_GET_CURRENT_FILE_LOCATION);

    return error.code == REGION_ERROR_CODE_EINVAL_REGION_ALLOC_LARGE_CAPACITY;
}

REGISTER_TEST(region_alloc_case_1, 1);
REGISTER_TEST(region_alloc_case_2, 2);

EXPOSE(tests)
