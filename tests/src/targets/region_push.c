#include "../include/rt-shared.h"

#define TEST_REGION_PUSH_CASE_4_CAPACITY 10

TestResult test_region_push_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    void *pushed = api->region_push(NULL, 10, &error);

    RT_TEST_RESULT_WRITE_PTR(result, NULL, pushed);

    return result;
}

TestResult test_region_push_case_2(RegionAPI *api)
{    
    TestResult result = {0};

    Region *region = NULL;
    RegionError error = REGION_ERROR_INIT;

    region = api->region_alloc(1, &error); RT_TARGET_UNWRAP;

    size_t size_before_push = region->size;

    api->region_push(&region, 0, &error);

    result.success = (REGION_NO_ERROR(error) && region->size == size_before_push);

    api->region_free(&region);

    return result;

    RT_TARGET_FATAL_ERROR(if (region) api->region_free(&region));
}

TestResult test_region_push_case_3(RegionAPI *api)
{   
    TestResult result = {0};

    Region *region = NULL;
    RegionError error = REGION_ERROR_INIT;
    
    region = api->region_alloc(1, &error); RT_TARGET_UNWRAP;

    api->region_push(&region, SIZE_MAX, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_MESSAGE_ARG_LARGE_SIZE, error.message);

    api->region_free(&region);

    return result;

    RT_TARGET_FATAL_ERROR(if (region) api->region_free(&region));
}

TestResult test_region_push_case_4(RegionAPI *api)
{    
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    Region *region = NULL;

    region = api->region_alloc(TEST_REGION_PUSH_CASE_4_CAPACITY, &error); RT_TARGET_UNWRAP;

    api->test_set_available_memory(0);

    api->region_push(&region, TEST_REGION_PUSH_CASE_4_CAPACITY * 2, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION, error.message);

    api->test_set_default_memory();

    api->region_free(&region);

    return result;
    
    RT_TARGET_FATAL_ERROR(if (region) api->region_free(&region));
}

RT_TEST_MODULE_REGISTER(test_region_push_case_1, 1);
RT_TEST_MODULE_REGISTER(test_region_push_case_2, 2);
RT_TEST_MODULE_REGISTER(test_region_push_case_3, 3);
RT_TEST_MODULE_REGISTER(test_region_push_case_4, 4);

RT_TEST_MODULE_EXPORT;