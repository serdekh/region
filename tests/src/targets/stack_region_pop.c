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
    
    api->stack_region_pop(stack, &error); 
    
    RT_TEST_RESULT_WRITE_INT(result, (int)(sizeof(int) + sizeof(size_t)), (int)api->stack_region_get_size(stack));
    
    api->stack_region_free(&stack);

    return result;

    RT_TARGET_FATAL_ERROR(if (stack) api->stack_region_free(&stack));
}

TestResult test_stack_region_pop_case_3(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegion *stack = NULL;

    stack = api->stack_region_alloc(1, &error); RT_TARGET_UNWRAP;

    stack->size = 1;
    *(size_t *)(stack->data - sizeof(size_t)) = 1;

    api->stack_region_pop(stack, &error);
    api->stack_region_free(&stack);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_MESSAGE_CORRUPTED_STACK_REGION_DATA, error.message);

    return result;

    RT_TARGET_FATAL_ERROR(if (stack) api->stack_region_free(&stack));
}

RT_TEST_MODULE_REGISTER(test_stack_region_pop_case_1, 1);
RT_TEST_MODULE_REGISTER(test_stack_region_pop_case_2, 2);
RT_TEST_MODULE_REGISTER(test_stack_region_pop_case_3, 3);

RT_TEST_MODULE_EXPORT;