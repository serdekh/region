#include "../include/rt-shared.h"

#define TEST_STACK_REGION_POP_CAPACITY sizeof(int) * 2 + sizeof(size_t) * 2
#define TEST_STACK_REGION_POP_RANDOM_VALUE 512

TestResult test_stack_region_pop_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegionFrame frame = api->stack_region_pop(NULL, &error);

    RT_TEST_RESULT_WRITE_PTR(result, NULL, frame.data);

    return result;
}

TestResult test_stack_region_pop_case_2(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    
    StackRegion *stack = api->stack_region_alloc(TEST_STACK_REGION_POP_CAPACITY, &error); RT_TARGET_UNWRAP;

    api->stack_region_push(&stack, sizeof(int), &error); RT_TARGET_UNWRAP;
    api->stack_region_push(&stack, sizeof(int), &error); RT_TARGET_UNWRAP;
    
    api->stack_region_pop(stack, &error); RT_TARGET_UNWRAP;
    
    RT_TEST_RESULT_WRITE_INT(result, (int)(sizeof(int) + sizeof(size_t)), (int)stack->size);
    
    api->stack_region_free(&stack);

    return result;

    RT_TARGET_FATAL_ERROR(if (stack) api->stack_region_free(&stack));
}

RT_TEST_MODULE_REGISTER(test_stack_region_pop_case_1, 1);
RT_TEST_MODULE_REGISTER(test_stack_region_pop_case_2, 2);

RT_TEST_MODULE_EXPORT;