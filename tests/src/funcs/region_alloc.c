#include "./common/common.h"

TestResult region_alloc_case_1() 
{
    RegionAPI *api = try_get_region_api_handle();

    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->region_alloc(0, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_SMALL_CAPACITY, error.code);

    return result;
}

TestResult region_alloc_case_2() 
{
    RegionAPI *api = try_get_region_api_handle();

    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->region_alloc(SIZE_MAX, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_LARGE_CAPACITY, error.code);

    return result;
}

TestResult region_alloc_case_3() 
{
    RegionAPI *api = try_get_region_api_handle();

    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    set_available_memory(sizeof(Region) / 2);

    api->region_alloc(1, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_REGION, error.code);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

TestResult region_alloc_case_4() 
{
    RegionAPI *api = try_get_region_api_handle();

    TestResult result = {0};

    size_t capacity = 10;

    RegionError error = REGION_ERROR_INIT;

    set_available_memory(sizeof(Region) + capacity / 2);

    api->region_alloc(capacity, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_CAPACITY, error.code);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

REGISTER_TEST(region_alloc_case_1, 1);
REGISTER_TEST(region_alloc_case_2, 2);
REGISTER_TEST(region_alloc_case_3, 3);
REGISTER_TEST(region_alloc_case_4, 4);

EXPOSE(tests)
