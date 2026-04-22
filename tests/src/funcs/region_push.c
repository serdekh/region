#include "../include/shared.h"

#define TEST_REGION_PUSH_CASE_4_CAPACITY 10

TestResult test_region_push_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->region_push(NULL, 10, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_PUSH_NO_REGION, error.code);

    return result;
}

TestResult test_region_push_case_2(RegionAPI *api)
{    
    TestResult result = {0};

    Region r = {0};
    RegionError error = REGION_ERROR_INIT;
    size_t size_before_push = r.size;

    api->region_push(&r, 0, &error);

    result.success = (REGION_NO_ERROR(error) && r.size == size_before_push);

    return result;
}

TestResult test_region_push_case_3(RegionAPI *api)
{   
    TestResult result = {0};

    Region r = {0};
    RegionError error = REGION_ERROR_INIT;

    api->region_push(&r, SIZE_MAX, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_PUSH_LARGE_SIZE, error.code);

    return result;
}

TestResult test_region_push_case_4(RegionAPI *api)
{    
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    Region *region = NULL;

    region = api->region_alloc(TEST_REGION_PUSH_CASE_4_CAPACITY, &error); UNWRAP;

    api->test_set_available_memory(0);

    api->region_push(region, TEST_REGION_PUSH_CASE_4_CAPACITY * 2, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_MALLOC_REGION, error.code);

    api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    api->region_free(&region);

    return result;
    
    TEST_FATAL(if (region) api->region_free(&region););
}

REGISTER_TEST(test_region_push_case_1, 1);
REGISTER_TEST(test_region_push_case_2, 2);
REGISTER_TEST(test_region_push_case_3, 3);
REGISTER_TEST(test_region_push_case_4, 4);

EXPORT_AT_TESTS_SECTION