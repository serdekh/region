#include "common/common.h"

FuncPtr_stack_region_pop fn = NULL;
#define test_fn(stack, error) fn((stack), (error), REGION_GET_CURRENT_FILE_LOCATION)

void try_init_test_fn()
{
    if (!_RegionHandle) try_get_region_handle();
    if (!fn) fn = try_get_symbol(SYMBOL_FN_STACK_REGION_POP);
}

TestResult test_stack_region_pop_case_1()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    test_fn(NULL, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_POP_NO_STACK_REGION, error.code);

    return result;
}

#define TEST_STACK_REGION_POP_CAPACITY sizeof(int) * 2 + sizeof(size_t) * 2
#define TEST_STACK_REGION_POP_RANDOM_VALUE 512
#define UNWRAP_IF_ERROR if (REGION_ERROR(error)) goto fatal

TestResult test_stack_region_pop_case_2()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);
    FuncPtr_stack_region_free stack_region_free = try_get_symbol(SYMBOL_FN_STACK_REGION_FREE);

    FuncPtr_stack_region_alloc __stack_region_alloc = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);
    FuncPtr_stack_region_push __stack_region_push = try_get_symbol(SYMBOL_FN_STACK_REGION_PUSH);
    
    StackRegion *stack = stack_region_alloc(TEST_STACK_REGION_POP_CAPACITY, &error); UNWRAP_IF_ERROR;

    stack_region_push(stack, sizeof(int), &error); UNWRAP_IF_ERROR;
    stack_region_push(stack, sizeof(int), &error); UNWRAP_IF_ERROR;
    
    test_fn(stack, &error); UNWRAP_IF_ERROR;
    
    TEST_RESULT_WRITE_INT(result, (int)(sizeof(int) + sizeof(size_t)), (int)stack->size);
    
    stack_region_free(&stack);

    return result;

fatal:
    if (stack) stack_region_free(&stack);
    if (_RegionHandle) dlclose(_RegionHandle);
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Failed to perform a test for the `__stack_region_pop` function. Stop.\n");
    exit(1);
}

REGISTER_TEST(test_stack_region_pop_case_1, 1);
REGISTER_TEST(test_stack_region_pop_case_2, 2);

EXPOSE(tests);