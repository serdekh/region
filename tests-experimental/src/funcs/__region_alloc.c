#include "common.h"

#define SYMBOL_FN_REGION_ALLOC "__region_alloc"

FuncPtr_region_alloc fn = NULL;

void try_init_test_fn()
{
    try_get_region_handle();
    
    fn = try_get_symbol(SYMBOL_FN_REGION_ALLOC);
}

TestResult region_alloc_case_1() 
{
    try_init_test_fn();

    RegionError error = {0};
    TestResult result = {0};

    fn(0, &error, REGION_GET_CURRENT_FILE_LOCATION);

    INT_TO_STR(result.expected, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_SMALL_CAPACITY);
    INT_TO_STR(result.actual, error.code);

    result.success = error.code == REGION_ERROR_CODE_EINVAL_REGION_ALLOC_SMALL_CAPACITY;

    return result;
}

TestResult region_alloc_case_2() 
{
    try_init_test_fn();

    RegionError error = {0};
    TestResult result = {0};

    fn(SIZE_MAX, &error, REGION_GET_CURRENT_FILE_LOCATION);

    INT_TO_STR(result.expected, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_LARGE_CAPACITY);
    INT_TO_STR(result.actual, error.code);

    result.success = error.code == REGION_ERROR_CODE_EINVAL_REGION_ALLOC_LARGE_CAPACITY;

    return result;
}

REGISTER_TEST(region_alloc_case_1, 1);
REGISTER_TEST(region_alloc_case_2, 2);

EXPOSE(tests)
