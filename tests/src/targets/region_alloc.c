#include "../include/rt-shared.h"

TestResult region_alloc_case_1(RegionAPI *api) 
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    Region *region = api->region_alloc(0, &error); RT_TARGET_UNWRAP;

    sprintf(result.expected, REGION_STRING_EMPTY);

    REGION_TO_STRING(result.actual, region);

    result.success = REGION_IS_EMPTY(region);

    return result;

    RT_TARGET_FATAL_ERROR(if (region) api->region_free(&region));
}

TestResult region_alloc_case_2(RegionAPI *api) 
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->region_alloc(SIZE_MAX, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_LARGE_CAPACITY, error.code);

    return result;
}

TestResult region_alloc_case_3(RegionAPI *api) 
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->test_set_available_memory(sizeof(Region) / 2);

    api->region_alloc(1, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_REGION, error.code);

    api->test_set_default_memory();

    return result;
}

TestResult region_alloc_case_4(RegionAPI *api) 
{
    TestResult result = {0};

    size_t capacity = 10;

    RegionError error = REGION_ERROR_INIT;

    api->test_set_available_memory(sizeof(Region) + capacity / 2);

    api->region_alloc(capacity, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_CAPACITY, error.code);

    api->test_set_default_memory();

    return result;
}

RT_TEST_MODULE_REGISTER(region_alloc_case_1, 1);
RT_TEST_MODULE_REGISTER(region_alloc_case_2, 2);
RT_TEST_MODULE_REGISTER(region_alloc_case_3, 3);
RT_TEST_MODULE_REGISTER(region_alloc_case_4, 4);

RT_TEST_MODULE_EXPORT
