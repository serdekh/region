#include "common/common.h"

FuncPtr_stack_region_alloc fn = NULL;

void try_init_test_fn()
{
    try_get_region_handle();

    if (!fn) fn = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);
}

TestResult test_stack_region_alloc_case_1()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    fn(0, &error, REGION_GET_CURRENT_FILE_LOCATION);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_ALLOC_SMALL_CAPACITY, error.code);

    return result;
}

TestResult test_stack_region_alloc_case_2()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    fn(SIZE_MAX, &error, REGION_GET_CURRENT_FILE_LOCATION);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_ALLOC_LARGE_CAPACITY, error.code);

    return result;
}

REGISTER_TEST(test_stack_region_alloc_case_1, 1);
REGISTER_TEST(test_stack_region_alloc_case_2, 2);
EXPOSE(tests);