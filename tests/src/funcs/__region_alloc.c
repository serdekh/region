#include "./common/common.h"

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

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_SMALL_CAPACITY, error.code);

    return result;
}

TestResult region_alloc_case_2() 
{
    try_init_test_fn();

    RegionError error = {0};
    TestResult result = {0};

    fn(SIZE_MAX, &error, REGION_GET_CURRENT_FILE_LOCATION);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_LARGE_CAPACITY, error.code);

    return result;
}

TestResult region_alloc_case_3() 
{
    try_init_test_fn();

    RegionError error = {0};
    TestResult result = {0};

    // Emulate a system that cannot allocate a complete Region struct
    set_available_memory(sizeof(Region) / 2);

    fn(1, &error, REGION_GET_CURRENT_FILE_LOCATION);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_REGION, error.code);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

TestResult region_alloc_case_4() 
{
    try_init_test_fn();

    RegionError error = {0};
    TestResult result = {0};

    // Emulate a system that cannot allocate a complete Region struct + capacity for the data
    size_t capacity = 10;
    set_available_memory(sizeof(Region) + capacity / 2);

    fn(capacity, &error, REGION_GET_CURRENT_FILE_LOCATION);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_CAPACITY, error.code);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

REGISTER_TEST(region_alloc_case_1, 1);
REGISTER_TEST(region_alloc_case_2, 2);
REGISTER_TEST(region_alloc_case_3, 3);
REGISTER_TEST(region_alloc_case_4, 4);

EXPOSE(tests)
