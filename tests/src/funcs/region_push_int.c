#include "../include/shared.h"

#define TEST_REGION_PUSH_INT_CASE1_VALUE 34
#define TEST_REGION_PUSH_INT_CASE2_VALUE 44

TestResult test_region_push_int_case_1(RegionAPI *api)
{
    TestResult result = {0};
    
    RegionError error = REGION_ERROR_INIT;
    Region *region = NULL;

    region = api->region_alloc(sizeof(int), &error); UNWRAP;

    api->region_push_int(region, TEST_REGION_PUSH_INT_CASE1_VALUE, &error);

    int *actual = (int *)(region->data + region->size - sizeof(int));

    TEST_RESULT_WRITE_INT(result, TEST_REGION_PUSH_INT_CASE1_VALUE, *actual);

    api->region_free(&region);

    return result;

    TEST_FATAL(if (region) api->region_free(&region););
}

TestResult test_region_push_int_case_2(RegionAPI *api)
{
    TestResult result = {0};
    
    RegionError error = REGION_ERROR_INIT;
    Region *region = NULL;
    
    region = api->region_alloc(sizeof(int) / 2, &error); UNWRAP;

    api->region_push_int(region, TEST_REGION_PUSH_INT_CASE2_VALUE, &error);

    int *actual = (int *)((region->next)->data + (region->next)->size - sizeof(int));

    TEST_RESULT_WRITE_INT(result, TEST_REGION_PUSH_INT_CASE2_VALUE, *actual);

    api->region_free(&region);

    return result;

    TEST_FATAL(if (region) api->region_free(&region););
}

TestResult test_region_push_int_case_3(RegionAPI *api)
{
    TestResult result = {0};
    
    RegionError error = REGION_ERROR_INIT;

    api->region_push_int(NULL, TEST_REGION_PUSH_INT_CASE1_VALUE, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_PUSH_INT_NO_REGION, error.code);

    return result;

    TEST_FATAL();
}

TestResult test_region_push_int_case_4(RegionAPI *api)
{
    TestResult result = {0};
    
    RegionError error = REGION_ERROR_INIT;
    Region *region = NULL;
    
    region = api->region_alloc(sizeof(int) / 2, &error); UNWRAP;

    api->test_set_available_memory(0);

    api->region_push_int(region, TEST_REGION_PUSH_INT_CASE2_VALUE, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_INT_MALLOC_REGION, error.code);

    api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    api->region_free(&region);

    return result;

    TEST_FATAL(if (region) api->region_free(&region););
}

REGISTER_TEST(test_region_push_int_case_1, 1);
REGISTER_TEST(test_region_push_int_case_2, 2);
REGISTER_TEST(test_region_push_int_case_3, 3);
REGISTER_TEST(test_region_push_int_case_4, 4);

EXPORT_AT_TESTS_SECTION