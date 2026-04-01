#include "./common/common.h"

FuncPtr_region_push fn = NULL;

void try_init_test_fn()
{
    try_get_region_handle();
    
    fn = try_get_symbol(SYMBOL_FN_REGION_PUSH);
}

TestResult test_region_push_case_1()
{
    try_init_test_fn();

    RegionError error = {0};
    TestResult result = {0};

    fn(NULL, 10, &error, REGION_GET_CURRENT_FILE_LOCATION);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_PUSH_NO_REGION, error.code);

    return result;
}

TestResult test_region_push_case_2()
{
    try_init_test_fn();
    
    Region r = {0};
    RegionError error = {0};
    TestResult result = {0};

    fn(&r, 0, &error, REGION_GET_CURRENT_FILE_LOCATION);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_PUSH_SMALL_SIZE, error.code);

    return result;
}

TestResult test_region_push_case_3()
{
    try_init_test_fn();
    
    Region r = {0};
    RegionError error = {0};
    TestResult result = {0};

    fn(&r, SIZE_MAX, &error, REGION_GET_CURRENT_FILE_LOCATION);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_PUSH_LARGE_SIZE, error.code);

    return result;
}

#define TEST_REGION_PUSH_CASE_4_CAPACITY 10

TestResult test_region_push_case_4()
{
    try_init_test_fn();
    
    RegionError error = {0};
    TestResult result = {0};

    FuncPtr_region_alloc __region_alloc = try_get_symbol(SYMBOL_FN_REGION_ALLOC);
    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);

    Region *r = __region_alloc(TEST_REGION_PUSH_CASE_4_CAPACITY, &error, REGION_GET_CURRENT_FILE_LOCATION);

    if (!r) {
        REGION_LOG_ERROR(error);
        fprintf(stderr, "[Test][Error]: Could not allocate memory to perform a test for the __region_push function. Stop.\n");
        exit(1);
    }

    set_available_memory(0);

    fn(r, TEST_REGION_PUSH_CASE_4_CAPACITY * 2, &error, REGION_GET_CURRENT_FILE_LOCATION);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_MALLOC_REGION, error.code);

    region_free(&r);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

REGISTER_TEST(test_region_push_case_1, 1);
REGISTER_TEST(test_region_push_case_2, 2);
REGISTER_TEST(test_region_push_case_3, 3);
REGISTER_TEST(test_region_push_case_4, 4);

EXPOSE(tests)