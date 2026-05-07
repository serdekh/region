#include "../include/rt-shared.h"

#define TEST_STACK_REGION_GET_CAPACITY_CASE_2_CAPACITY 1
#define TEST_STACK_REGION_GET_SIZE_CASE_2_CAPACITY 2
#define TEST_STACK_REGION_GET_SIZE_CASE_2_SIZE 1
#define TEST_STACK_REGION_GET_COUNT_CASE_3_CAPACITY 10
#define TEST_STACK_REGION_GET_COUNT_CASE_3_ITEM_SIZE 1

TestResult test_stack_region_get_capacity_case_1(RegionAPI *api)
{
    TestResult result = {0};

    size_t nullptr_capacity = api->stack_region_get_capacity(NULL);

    RT_TEST_RESULT_WRITE_INT(result, 0, (int)nullptr_capacity);

    return result;
}

TestResult test_stack_region_get_capacity_case_2(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegion *stack = api->stack_region_alloc(TEST_STACK_REGION_GET_CAPACITY_CASE_2_CAPACITY, &error); RT_TARGET_UNWRAP;

    size_t capacity = api->stack_region_get_capacity(stack);

    RT_TEST_RESULT_WRITE_INT(result, TEST_STACK_REGION_GET_CAPACITY_CASE_2_CAPACITY + sizeof(size_t), (int)capacity);

    api->stack_region_free(&stack);

    return result;

    RT_TARGET_FATAL_ERROR(if (stack) api->stack_region_free(&stack));
}

TestResult test_stack_region_get_size_case_1(RegionAPI *api)
{
    TestResult result = {0};

    size_t nullptr_size = api->stack_region_get_size(NULL);

    RT_TEST_RESULT_WRITE_INT(result, 0, (int)nullptr_size);
    
    return result;
}

TestResult test_stack_region_get_size_case_2(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegion *stack = api->stack_region_alloc(TEST_STACK_REGION_GET_SIZE_CASE_2_CAPACITY, &error); RT_TARGET_UNWRAP;

    api->stack_region_push(&stack, TEST_STACK_REGION_GET_SIZE_CASE_2_SIZE, &error); RT_TARGET_UNWRAP;

    size_t size = api->stack_region_get_size(stack);

    api->stack_region_free(&stack);

    RT_TEST_RESULT_WRITE_INT(result, TEST_STACK_REGION_GET_SIZE_CASE_2_SIZE + sizeof(size_t), (int)size);

    return result;

    RT_TARGET_FATAL_ERROR(if (stack) api->stack_region_free(&stack));
}

TestResult test_stack_region_get_count_case_1(RegionAPI *api)
{
    TestResult result = {0};

    size_t nullptr_count = api->stack_region_get_count(NULL);

    RT_TEST_RESULT_WRITE_INT(result, 0, nullptr_count);

    return result;
}

TestResult test_stack_region_get_count_case_2(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegion *stack = api->stack_region_alloc(0, &error); RT_TARGET_UNWRAP;

    size_t nulldata_count = api->stack_region_get_count(stack);

    api->stack_region_free(&stack);

    RT_TEST_RESULT_WRITE_INT(result, 0, (int)nulldata_count);

    return result;

    RT_TARGET_FATAL_ERROR(if (stack) api->stack_region_free(&stack));
}

TestResult test_stack_region_get_count_case_3(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegion *stack = api->stack_region_alloc(TEST_STACK_REGION_GET_COUNT_CASE_3_CAPACITY, &error); RT_TARGET_UNWRAP;

    api->stack_region_push(&stack, TEST_STACK_REGION_GET_COUNT_CASE_3_ITEM_SIZE, &error); RT_TARGET_UNWRAP;

    size_t count = api->stack_region_get_count(stack);

    api->stack_region_free(&stack);

    RT_TEST_RESULT_WRITE_INT(result, 1, (int)count);

    return result;

    RT_TARGET_FATAL_ERROR(if (stack) api->stack_region_free(&stack));
}


RT_TEST_MODULE_REGISTER(test_stack_region_get_capacity_case_1, 1);
RT_TEST_MODULE_REGISTER(test_stack_region_get_capacity_case_2, 2);

RT_TEST_MODULE_REGISTER(test_stack_region_get_size_case_1, 1);
RT_TEST_MODULE_REGISTER(test_stack_region_get_size_case_2, 2);

RT_TEST_MODULE_REGISTER(test_stack_region_get_count_case_1, 1);
RT_TEST_MODULE_REGISTER(test_stack_region_get_count_case_2, 2);
RT_TEST_MODULE_REGISTER(test_stack_region_get_count_case_3, 3);

RT_TEST_MODULE_EXPORT;
