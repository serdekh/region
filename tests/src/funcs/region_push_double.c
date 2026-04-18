#include "../include/shared.h"

#define TEST_REGION_PUSH_DOUBLE_CASE1_VALUE 56.44f
#define TEST_REGION_PUSH_DOUBLE_CASE2_VALUE 9822.33f

TestResult test_region_push_double_case_1(RegionAPI *api)
{
    TestResult result = {0};
    
    RegionError error = REGION_ERROR_INIT;
    Region *region = NULL;

    region = api->region_alloc(sizeof(double), &error); UNWRAP;

    api->region_push_double(region, TEST_REGION_PUSH_DOUBLE_CASE1_VALUE, &error);

    double *actual = (double *)(region->data + region->size - sizeof(double));

    TEST_RESULT_WRITE_DOUBLE(result, TEST_REGION_PUSH_DOUBLE_CASE1_VALUE, *actual);

    api->region_free(&region);

    return result;

    TEST_FATAL(if (region) api->region_free(&region));
}

TestResult test_region_push_double_case_2(RegionAPI *api)
{
    TestResult result = {0};
    
    RegionError error = REGION_ERROR_INIT;
    Region *region = NULL;
    
    region = api->region_alloc(sizeof(double) / 2, &error); UNWRAP;

    api->region_push_double(region, TEST_REGION_PUSH_DOUBLE_CASE2_VALUE, &error);

    double *actual = (double *)((region->next)->data + (region->next)->size - sizeof(double));

    TEST_RESULT_WRITE_DOUBLE(result, TEST_REGION_PUSH_DOUBLE_CASE2_VALUE, *actual);

    api->region_free(&region);

    return result;

    TEST_FATAL(if (region) api->region_free(&region));
}

TestResult test_region_push_double_case_3(RegionAPI *api)
{
    TestResult result = {0};
    
    RegionError error = REGION_ERROR_INIT;

    api->region_push_double(NULL, 0, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_PUSH_DOUBLE_NO_REGION, error.code);

    return result;
}

TestResult test_region_push_double_case_4(RegionAPI *api)
{
    TestResult result = {0};
    
    RegionError error = REGION_ERROR_INIT;
    Region *region = NULL;
    
    region = api->region_alloc(sizeof(double) / 2, &error); UNWRAP;

    api->test_set_available_memory(0);

    api->region_push_double(region, 0, &error);

    TEST_RESULT_WRITE_DOUBLE(result, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_DOUBLE_MALLOC_REGION, error.code);

    api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    api->region_free(&region);

    return result;

    TEST_FATAL(if (region) api->region_free(&region));
}

REGISTER_TEST(test_region_push_double_case_1, 1);
REGISTER_TEST(test_region_push_double_case_2, 2);
REGISTER_TEST(test_region_push_double_case_3, 3);
REGISTER_TEST(test_region_push_double_case_4, 4);

EXPORT_AT_TESTS_SECTION