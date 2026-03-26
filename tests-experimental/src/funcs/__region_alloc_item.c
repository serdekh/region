#include "common.h"

#define SYMBOL_FN_REGION_ALLOC_ITEM "__region_alloc_item"

FuncPtr_region_alloc_item fn = NULL;

void try_init_test_fn()
{
    try_get_region_handle();
    
    fn = try_get_symbol(SYMBOL_FN_REGION_ALLOC_ITEM);
}

TestResult test_region_alloc_item_case_1()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    fn(NULL, 10, &error, REGION_GET_CURRENT_FILE_LOCATION);

    INT_TO_STR(result.expected, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_ITEM_NO_REGION);
    INT_TO_STR(result.actual, error.code);

    result.success = error.code == REGION_ERROR_CODE_EINVAL_REGION_ALLOC_ITEM_NO_REGION;

    return result;
}

TestResult test_region_alloc_item_case_2()
{
    try_init_test_fn();
    
    Region r = {0};
    RegionError error = {0};

    TestResult result = {0};

    fn(&r, 0, &error, REGION_GET_CURRENT_FILE_LOCATION);

    INT_TO_STR(result.expected, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_ITEM_SMALL_SIZE);
    INT_TO_STR(result.actual, error.code);

    result.success = error.code == REGION_ERROR_CODE_EINVAL_REGION_ALLOC_ITEM_SMALL_SIZE;

    return result;
}

TestResult test_region_alloc_item_case_3()
{
    try_init_test_fn();
    
    Region r = {0};
    RegionError error = {0};

    TestResult result = {0};

    fn(&r, SIZE_MAX, &error, REGION_GET_CURRENT_FILE_LOCATION);

    INT_TO_STR(result.expected, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_ITEM_LARGE_SIZE);
    INT_TO_STR(result.actual, error.code);

    result.success = error.code == REGION_ERROR_CODE_EINVAL_REGION_ALLOC_ITEM_LARGE_SIZE;

    return result;
}

REGISTER_TEST(test_region_alloc_item_case_1, 1);
REGISTER_TEST(test_region_alloc_item_case_2, 2);
REGISTER_TEST(test_region_alloc_item_case_3, 3);

EXPOSE(tests)