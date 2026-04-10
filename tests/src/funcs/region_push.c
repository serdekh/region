#include "./common/common.h"

FuncPtr_region_push fn = NULL;

#define TEST_REGION_PUSH_CASE_4_CAPACITY 10

void try_init_test_fn()
{
    if (!_RegionHandle) try_get_region_handle();
    if (!fn) fn = try_get_symbol(SYMBOL_FN_REGION_PUSH);
}

TestResult test_region_push_case_1()
{
    try_init_test_fn();

    RegionError error = {0};
    TestResult result = {0};

    fn(NULL, 10, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_PUSH_NO_REGION, error.code);

    return result;
}

TestResult test_region_push_case_2()
{
    try_init_test_fn();
    
    Region r = {0};
    RegionError error = {0};
    TestResult result = {0};

    fn(&r, 0, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_PUSH_SMALL_SIZE, error.code);

    return result;
}

TestResult test_region_push_case_3()
{
    try_init_test_fn();
    
    Region r = {0};
    RegionError error = {0};
    TestResult result = {0};

    fn(&r, SIZE_MAX, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_PUSH_LARGE_SIZE, error.code);

    return result;
}

TestResult test_region_push_case_4()
{
    try_init_test_fn();
    
    RegionError error = {0};
    TestResult result = {0};
    Region *region = NULL;

    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);
    FuncPtr_region_alloc region_alloc = try_get_symbol(SYMBOL_FN_REGION_ALLOC);

    region = region_alloc(TEST_REGION_PUSH_CASE_4_CAPACITY, &error); UNWRAP;

    set_available_memory(0);

    fn(region, TEST_REGION_PUSH_CASE_4_CAPACITY * 2, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_MALLOC_REGION, error.code);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    region_free(&region);

    return result;
    
fatal:
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Could not perfom a test. Stop.\n");

    if(region) region_free(&region);
    if (_RegionHandle) dlclose(_RegionHandle);

    exit(1);
}

REGISTER_TEST(test_region_push_case_1, 1);
REGISTER_TEST(test_region_push_case_2, 2);
REGISTER_TEST(test_region_push_case_3, 3);
REGISTER_TEST(test_region_push_case_4, 4);

EXPOSE(tests)