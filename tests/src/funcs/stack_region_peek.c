#include "./common/common.h"

FuncPtr_stack_region_peek fn = NULL;

void try_init_test_fn()
{
    if (!_RegionHandle) try_get_region_handle();
    if (!fn) fn = try_get_symbol(SYMBOL_FN_STACK_REGION_PEEK);
}

#define TEST_STACK_REGION_PEEK_CAPACITY sizeof(int) + sizeof(size_t)
#define TEST_STACK_REGION_PEEK_RANDOM_VALUE 42

TestResult test_stack_region_peek_case_1()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    fn(NULL, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_PEEK_NO_STACK_REGION, error.code);

    return result;
}

TestResult test_stack_region_peek_case_2()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};
    StackRegion *stack = NULL;

    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);

    FuncPtr_stack_region_free  stack_region_free  = try_get_symbol(SYMBOL_FN_STACK_REGION_FREE);
    FuncPtr_stack_region_push  stack_region_push  = try_get_symbol(SYMBOL_FN_STACK_REGION_PUSH);
    FuncPtr_stack_region_peek  stack_region_peek  = try_get_symbol(SYMBOL_FN_STACK_REGION_PEEK);
    FuncPtr_stack_region_alloc stack_region_alloc = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);

    stack = stack_region_alloc(TEST_STACK_REGION_PEEK_CAPACITY, &error); UNWRAP;

    StackRegionFrame frame = stack_region_push(stack, sizeof(int), &error); UNWRAP;
    
    *(int *)frame.data = TEST_STACK_REGION_PEEK_RANDOM_VALUE;
    
    StackRegionFrame peeked_frame = stack_region_peek(stack, &error); UNWRAP;
  
    TEST_RESULT_WRITE_INT(result, TEST_STACK_REGION_PEEK_RANDOM_VALUE, *(int *)peeked_frame.data);

    stack_region_free(&stack);

    return result;
    
fatal:
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Could not perfom a test. Stop.\n");

    if (stack) stack_region_free(&stack);
    if (_RegionHandle) dlclose(_RegionHandle);

    exit(1);
}

REGISTER_TEST(test_stack_region_peek_case_1, 1);
REGISTER_TEST(test_stack_region_peek_case_2, 2);

EXPOSE(tests);