#include "../include/shared.h"

#define TEST_REGION_PUSH_CASE_4_CAPACITY 10

TestResult test_region_push_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    void *pushed = api->region_push(NULL, 10, &error);

    TEST_RESULT_WRITE_PTR(result, NULL, pushed);

    return result;
}

TestResult test_region_push_case_2(RegionAPI *api)
{    
    TestResult result = {0};

    Region *region = NULL;
    RegionError error = REGION_ERROR_INIT;

    region = api->region_alloc(1, &error); UNWRAP;

    size_t size_before_push = region->size;

    api->region_push(&region, 0, &error);

    result.success = (REGION_NO_ERROR(error) && region->size == size_before_push);

    api->region_free(&region);

    return result;

    TEST_FATAL(if (region) api->region_free(&region));
}

TestResult test_region_push_case_3(RegionAPI *api)
{   
    TestResult result = {0};

    Region *region = NULL;
    RegionError error = REGION_ERROR_INIT;
    
    region = api->region_alloc(1, &error); UNWRAP;

    api->region_push(&region, SIZE_MAX, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_PUSH_LARGE_SIZE, error.code);

    api->region_free(&region);

    return result;

    TEST_FATAL(if (region) api->region_free(&region));
}

TestResult test_region_push_case_4(RegionAPI *api)
{    
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    Region *region = NULL;

    region = api->region_alloc(TEST_REGION_PUSH_CASE_4_CAPACITY, &error); UNWRAP;

    api->test_set_available_memory(0);

    api->region_push(&region, TEST_REGION_PUSH_CASE_4_CAPACITY * 2, &error);

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