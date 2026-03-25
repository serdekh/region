#include "common.h"

#define SYMBOL_FN_REGION_ALLOC_ITEM "__region_alloc_item"

bool test_region_alloc_item_case_1()
{
    try_get_region_handle();

    FuncPtr_region_alloc_item fn = try_get_symbol(SYMBOL_FN_REGION_ALLOC_ITEM);

    RegionError error = {0};

    fn(NULL, 10, &error, REGION_GET_CURRENT_FILE_LOCATION);

    return error.code == REGION_ERROR_CODE_EINVAL_REGION_ALLOC_ITEM_NO_REGION;
}

bool test_region_alloc_item_case_2()
{
    try_get_region_handle();

    FuncPtr_region_alloc_item fn = try_get_symbol(SYMBOL_FN_REGION_ALLOC_ITEM);
    
    Region r = {0};
    RegionError error = {0};

    fn(&r, 0, &error, REGION_GET_CURRENT_FILE_LOCATION);

    return error.code == REGION_ERROR_CODE_EINVAL_REGION_ALLOC_ITEM_SMALL_SIZE;
}

bool test_region_alloc_item_case_3()
{
    try_get_region_handle();

    FuncPtr_region_alloc_item fn = try_get_symbol(SYMBOL_FN_REGION_ALLOC_ITEM);
    
    Region r = {0};
    RegionError error = {0};

    fn(&r, SIZE_MAX, &error, REGION_GET_CURRENT_FILE_LOCATION);

    return error.code == REGION_ERROR_CODE_EINVAL_REGION_ALLOC_ITEM_LARGE_SIZE;
}

REGISTER_TEST(test_region_alloc_item_case_1, 1);
REGISTER_TEST(test_region_alloc_item_case_2, 2);
REGISTER_TEST(test_region_alloc_item_case_3, 3);

EXPOSE(tests)