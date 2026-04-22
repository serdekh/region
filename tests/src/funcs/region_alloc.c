#include "../include/shared.h"

TestResult region_alloc_case_1(RegionAPI *api) 
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    Region *region = api->region_alloc(0, &error); UNWRAP;

    sprintf(result.expected, REGION_STRING_EMPTY);

    REGION_TO_STRING(result.actual, region);

    result.success = REGION_IS_EMPTY(region);

    return result;

    TEST_FATAL(if (region) api->region_free(&region));
}

TestResult region_alloc_case_2(RegionAPI *api) 
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->region_alloc(SIZE_MAX, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_LARGE_CAPACITY, error.code);

    return result;
}

TestResult region_alloc_case_3(RegionAPI *api) 
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->test_set_available_memory(sizeof(Region) / 2);

    api->region_alloc(1, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_REGION, error.code);

    api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

TestResult region_alloc_case_4(RegionAPI *api) 
{
    TestResult result = {0};

    size_t capacity = 10;

    RegionError error = REGION_ERROR_INIT;

    api->test_set_available_memory(sizeof(Region) + capacity / 2);

    api->region_alloc(capacity, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_CAPACITY, error.code);

    api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

REGISTER_TEST(region_alloc_case_1, 1);
REGISTER_TEST(region_alloc_case_2, 2);
REGISTER_TEST(region_alloc_case_3, 3);
REGISTER_TEST(region_alloc_case_4, 4);

EXPORT_AT_TESTS_SECTION
