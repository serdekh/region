#include "common/common.h"

FuncPtr_stack_region_alloc fn = NULL;

#define TEST_STACK_REGION_ALLOC_CAPACITY 1

void try_init_test_fn()
{
    if (!_RegionHandle) try_get_region_handle();
    if (!fn) fn = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);
}

TestResult test_stack_region_alloc_case_1()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    fn(0, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_ALLOC_SMALL_CAPACITY, error.code);

    return result;
}

TestResult test_stack_region_alloc_case_2()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    fn(SIZE_MAX, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_ALLOC_LARGE_CAPACITY, error.code);

    return result;
}


TestResult test_stack_region_alloc_case_3()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    set_available_memory(sizeof(StackRegion) / 2);

    fn(TEST_STACK_REGION_ALLOC_CAPACITY, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_STACK_REGION_ALLOC_MALLOC_REGION, error.code);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

TestResult test_stack_region_alloc_case_4()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    set_available_memory(sizeof(StackRegion));

    fn(TEST_STACK_REGION_ALLOC_CAPACITY, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_STACK_REGION_ALLOC_MALLOC_CAPACITY, error.code);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

REGISTER_TEST(test_stack_region_alloc_case_1, 1);
REGISTER_TEST(test_stack_region_alloc_case_2, 2);
REGISTER_TEST(test_stack_region_alloc_case_3, 3);
REGISTER_TEST(test_stack_region_alloc_case_4, 4);

EXPOSE(tests);