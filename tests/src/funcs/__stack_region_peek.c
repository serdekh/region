#include "./common/common.h"

FuncPtr_stack_region_peek fn = NULL;

#define UNWRAP if (REGION_ERROR(error)) goto fatal

void try_init_test_fn()
{
    if (!_RegionHandle) try_get_region_handle();
    if (!fn) fn = try_get_symbol(SYMBOL_FN_STACK_REGION_PEEK);
}

#define TEST_STACK_REGION_CAPACITY sizeof(int) + sizeof(size_t)
#define TEST_STACK_REGION_RANDOM_VALUE 42

TestResult test_stack_region_peek_case_1()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    fn(NULL, &error, REGION_GET_CURRENT_FILE_LOCATION);

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
    FuncPtr_stack_region_free stack_region_free = try_get_symbol(SYMBOL_FN_STACK_REGION_FREE);
    FuncPtr_stack_region_alloc __stack_region_alloc = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);
    FuncPtr_stack_region_push __stack_region_push = try_get_symbol(SYMBOL_FN_STACK_REGION_PUSH);
    FuncPtr_stack_region_peek __stack_region_peek = try_get_symbol(SYMBOL_FN_STACK_REGION_PEEK);

    stack = __stack_region_alloc(TEST_STACK_REGION_CAPACITY, &error, REGION_GET_CURRENT_FILE_LOCATION); UNWRAP;
    int *data = (int *)__stack_region_push(stack, sizeof(int), &error, REGION_GET_CURRENT_FILE_LOCATION); UNWRAP;
    
    *data = TEST_STACK_REGION_RANDOM_VALUE;
    
    int *peeked_data = (int *)__stack_region_peek(stack, &error, REGION_GET_CURRENT_FILE_LOCATION); UNWRAP;
  
    TEST_RESULT_WRITE_INT(result, TEST_STACK_REGION_RANDOM_VALUE, *peeked_data);
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