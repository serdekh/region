#include "../include/shared.h"

#define TEST_REGION_PUSH_FLOAT_CASE1_VALUE 12.3f
#define TEST_REGION_PUSH_FLOAT_CASE2_VALUE 4553.0f

TestResult test_region_push_float_case_1(RegionAPI *api)
{
    TestResult result = {0};
    
    RegionError error = REGION_ERROR_INIT;
    Region *region = NULL;

    region = api->region_alloc(sizeof(float), &error); UNWRAP;

    api->region_push_float(region, TEST_REGION_PUSH_FLOAT_CASE1_VALUE, &error);

    float *actual = (float *)(region->data + region->size - sizeof(float));

    TEST_RESULT_WRITE_FLOAT(result, TEST_REGION_PUSH_FLOAT_CASE1_VALUE, *actual);

    api->region_free(&region);

    return result;

    TEST_FATAL(if (region) api->region_free(&region));
}

TestResult test_region_push_float_case_2(RegionAPI *api)
{
    TestResult result = {0};
    
    RegionError error = REGION_ERROR_INIT;
    Region *region = NULL;
    
    region = api->region_alloc(sizeof(float) / 2, &error); UNWRAP;

    api->region_push_float(region, TEST_REGION_PUSH_FLOAT_CASE2_VALUE, &error);

    float *actual = (float *)((region->next)->data + (region->next)->size - sizeof(float));

    TEST_RESULT_WRITE_FLOAT(result, TEST_REGION_PUSH_FLOAT_CASE2_VALUE, *actual);

    api->region_free(&region);

    return result;

    TEST_FATAL(if (region) api->region_free(&region));
}

TestResult test_region_push_float_case_3(RegionAPI *api)
{
    TestResult result = {0};
    
    RegionError error = REGION_ERROR_INIT;

    api->region_push_float(NULL, 0, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_PUSH_FLOAT_NO_REGION, error.code);

    return result;
}

TestResult test_region_push_float_case_4(RegionAPI *api)
{
    TestResult result = {0};
    
    RegionError error = REGION_ERROR_INIT;
    Region *region = NULL;
    
    region = api->region_alloc(sizeof(float) / 2, &error); UNWRAP;

    api->test_set_available_memory(0);

    api->region_push_float(region, 0, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_FLOAT_MALLOC_REGION, error.code);

    api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    api->region_free(&region);

    return result;

    TEST_FATAL(if (region) api->region_free(&region));
}

REGISTER_TEST(test_region_push_float_case_1, 1);
REGISTER_TEST(test_region_push_float_case_2, 2);
REGISTER_TEST(test_region_push_float_case_3, 3);
REGISTER_TEST(test_region_push_float_case_4, 4);

EXPOSE(tests);