#include "../include/rt-shared.h"

#define TEST_REGION_GET_CAPACITY_CASE_2_CAPACITY 10
#define TEST_REGION_GET_SIZE_CASE_2_SIZE 5

typedef size_t (*FuncPtr_region_get_member)(Region *region);

TestResult test_region_get_member_case_1(RegionAPI *api, FuncPtr_region_get_member region_get_member)
{
    TestResult result = {0};

    size_t nullptr_capacity = region_get_member(NULL);

    RT_TEST_RESULT_WRITE_INT(result, 0, (int)nullptr_capacity);

    return result;
}

TestResult test_region_get_capacity_case_1(RegionAPI *api)
{
    return test_region_get_member_case_1(api, api->region_get_capacity);
}

TestResult test_region_get_capacity_case_2(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    Region *region = api->region_alloc(TEST_REGION_GET_CAPACITY_CASE_2_CAPACITY, &error); RT_TARGET_UNWRAP;

    size_t actual = api->region_get_capacity(region);

    api->region_free(&region);

    RT_TEST_RESULT_WRITE_INT(result, TEST_REGION_GET_CAPACITY_CASE_2_CAPACITY, (int)actual);

    return result;

    RT_TARGET_FATAL_ERROR(if (region) api->region_free(&region));
}

TestResult test_region_get_size_case_1(RegionAPI *api)
{
    return test_region_get_member_case_1(api, api->region_get_size);
}

TestResult test_region_get_size_case_2(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    Region *region = api->region_alloc(TEST_REGION_GET_SIZE_CASE_2_SIZE, &error); RT_TARGET_UNWRAP;

    api->region_push(&region, TEST_REGION_GET_SIZE_CASE_2_SIZE, &error); RT_TARGET_UNWRAP;

    size_t actual = api->region_get_size(region);

    api->region_free(&region);

    RT_TEST_RESULT_WRITE_INT(result, TEST_REGION_GET_SIZE_CASE_2_SIZE, (int)actual);

    return result;

    RT_TARGET_FATAL_ERROR(if (region) api->region_free(&region));
}

RT_TEST_MODULE_REGISTER(test_region_get_capacity_case_1, 1);
RT_TEST_MODULE_REGISTER(test_region_get_capacity_case_2, 2);
RT_TEST_MODULE_REGISTER(test_region_get_size_case_1, 1);
RT_TEST_MODULE_REGISTER(test_region_get_size_case_2, 2);

RT_TEST_MODULE_EXPORT;