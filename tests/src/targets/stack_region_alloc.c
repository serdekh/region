#include "../include/rt-shared.h"

#define TEST_STACK_REGION_ALLOC_CAPACITY 1

TestResult test_stack_region_alloc_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegion *stack = api->stack_region_alloc(0, &error);

    result.success = stack->capacity == api->stack_region_get_count(stack) && stack->data != NULL;

    api->stack_region_free(&stack);

    return result;
}

TestResult test_stack_region_alloc_case_2(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->stack_region_alloc(SIZE_MAX, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_ALLOC_LARGE_CAPACITY, error.code);

    return result;
}


TestResult test_stack_region_alloc_case_3(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->test_set_available_memory(sizeof(StackRegion) / 2);

    api->stack_region_alloc(TEST_STACK_REGION_ALLOC_CAPACITY, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_STACK_REGION_ALLOC_MALLOC_REGION, error.code);

    api->test_set_default_memory();

    return result;
}

TestResult test_stack_region_alloc_case_4(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->test_set_available_memory(sizeof(StackRegion));

    api->stack_region_alloc(TEST_STACK_REGION_ALLOC_CAPACITY, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_STACK_REGION_ALLOC_MALLOC_CAPACITY, error.code);

    api->test_set_default_memory();

    return result;
}

RT_TEST_MODULE_REGISTER(test_stack_region_alloc_case_1, 1);
RT_TEST_MODULE_REGISTER(test_stack_region_alloc_case_2, 2);
RT_TEST_MODULE_REGISTER(test_stack_region_alloc_case_3, 3);
RT_TEST_MODULE_REGISTER(test_stack_region_alloc_case_4, 4);

RT_TEST_MODULE_EXPORT;