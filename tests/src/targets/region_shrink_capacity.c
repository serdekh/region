#include "../include/rt-shared.h"

#define TEST_REGION_SHRINK_CAPACITY_CASE3_CAPACITY (size_t)10
#define TEST_REGION_SHRINK_CAPACITY_CASE3_CAPACITY_SHRINKED (TEST_REGION_SHRINK_CAPACITY_CASE3_CAPACITY) / 2

#define TEST_REGION_SHRINK_CAPACITY_CASE4_NODE1_CAPACITY (size_t)5
#define TEST_REGION_SHRINK_CAPACITY_CASE4_NODE1_CAPACITY_SHRINKED (TEST_REGION_SHRINK_CAPACITY_CASE4_NODE1_CAPACITY) / 2
#define TEST_REGION_SHRINK_CAPACITY_CASE4_NODE2_CAPACITY (size_t)11
#define TEST_REGION_SHRINK_CAPACITY_CASE4_NODE2_CAPACITY_SHRINKED (TEST_REGION_SHRINK_CAPACITY_CASE4_NODE2_CAPACITY) / 2

TestResult test_region_shrink_capacity_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->region_shrink_capacity(NULL, REGION_SHRINK_CAPACITY_OPTION_ONLY_ROOT, &error);
   
    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_NO_ERROR, error.code);
    
    return result;
}

TestResult test_region_shrink_capacity_case_2(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    Region r = {0};

    r.size = SIZE_MAX - 1;
    r.capacity = SIZE_MAX;

    api->region_shrink_capacity(&r, REGION_SHRINK_CAPACITY_OPTION_ONLY_ROOT, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_SHRINK_CAPACITY_MALLOC, error.code);

    return result;
}

TestResult test_region_shrink_capacity_case_3(RegionAPI *api)
{
    TestResult result = {0};

    Region *r = NULL;
    RegionError error = REGION_ERROR_INIT;

    r = api->region_alloc(TEST_REGION_SHRINK_CAPACITY_CASE3_CAPACITY, &error); RT_TARGET_UNWRAP;

    api->region_push(&r, TEST_REGION_SHRINK_CAPACITY_CASE3_CAPACITY_SHRINKED, &error); RT_TARGET_UNWRAP;

    api->region_shrink_capacity(r, REGION_SHRINK_CAPACITY_OPTION_ONLY_ROOT, &error); RT_TARGET_UNWRAP;

    sprintf(result.expected, "Capacity after shrinking: %zu", TEST_REGION_SHRINK_CAPACITY_CASE3_CAPACITY_SHRINKED);
    sprintf(result.actual, "Actual capacity after shrinking: %zu", r->capacity);

    result.success = r->capacity == TEST_REGION_SHRINK_CAPACITY_CASE3_CAPACITY_SHRINKED;

    api->region_free(&r);

    return result;
    
    RT_TARGET_FATAL_ERROR(if (r) api->region_free(&r););
}

TestResult test_region_shrink_capacity_case_4(RegionAPI *api)
{
    TestResult result = {0};
    
    Region *node_1 = NULL;
    Region *node_2 = NULL;
    RegionError error = REGION_ERROR_INIT;

    node_1 = api->region_alloc(TEST_REGION_SHRINK_CAPACITY_CASE4_NODE1_CAPACITY, &error); RT_TARGET_UNWRAP;
    node_2 = api->region_alloc(TEST_REGION_SHRINK_CAPACITY_CASE4_NODE2_CAPACITY, &error); RT_TARGET_UNWRAP;

    api->region_push(&node_1, TEST_REGION_SHRINK_CAPACITY_CASE4_NODE1_CAPACITY_SHRINKED, &error); RT_TARGET_UNWRAP;
    api->region_push(&node_2, TEST_REGION_SHRINK_CAPACITY_CASE4_NODE2_CAPACITY_SHRINKED, &error); RT_TARGET_UNWRAP;

    node_1->next = node_2;

    api->region_shrink_capacity(node_1, REGION_SHRINK_CAPACITY_OPTION_ALL, &error); RT_TARGET_UNWRAP;

    sprintf(result.expected, "Capacity after shrinking: { First node: %zu, Second node: %zu}", 
        TEST_REGION_SHRINK_CAPACITY_CASE4_NODE1_CAPACITY_SHRINKED,
        TEST_REGION_SHRINK_CAPACITY_CASE4_NODE2_CAPACITY_SHRINKED);

    sprintf(result.actual, "Actual capacity after shrinking: { First node: %zu, Second node: %zu}", 
        node_1->capacity,
        node_2->capacity);

    result.success = 
        node_1->capacity == TEST_REGION_SHRINK_CAPACITY_CASE4_NODE1_CAPACITY_SHRINKED &&
        node_2->capacity == TEST_REGION_SHRINK_CAPACITY_CASE4_NODE2_CAPACITY_SHRINKED;

    api->region_free(&node_1);

    return result;
    
    RT_TARGET_FATAL_ERROR(if (node_1) api->region_free(&node_1););
}

RT_TEST_MODULE_REGISTER(test_region_shrink_capacity_case_1, 1);
RT_TEST_MODULE_REGISTER(test_region_shrink_capacity_case_2, 2);
RT_TEST_MODULE_REGISTER(test_region_shrink_capacity_case_3, 3);
RT_TEST_MODULE_REGISTER(test_region_shrink_capacity_case_4, 4);

RT_TEST_MODULE_EXPORT;