#include "./common/common.h"

FuncPtr_stack_region_push fn = NULL;

void try_init_test_fn()
{
    if (!_RegionHandle) try_get_region_handle();
    if (!fn) fn = try_get_symbol(SYMBOL_FN_STACK_REGION_PUSH);
}

TestResult test_stack_region_push_case_1()
{
    try_init_test_fn();

    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    fn(NULL, 1, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_NO_STACK_REGION, error.code);

    return result;
}

#define TEST_STACK_REGION_CAPACITY 1

TestResult test_stack_region_push_case_2()
{
    try_init_test_fn();

    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    StackRegion *stack = NULL;

    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);

    FuncPtr_stack_region_free  stack_region_free  = try_get_symbol(SYMBOL_FN_STACK_REGION_FREE);
    FuncPtr_stack_region_alloc stack_region_alloc = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);

    stack = stack_region_alloc(TEST_STACK_REGION_CAPACITY, &error); UNWRAP;

    fn(stack, 0, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_SMALL_SIZE, error.code);

    stack_region_free(&stack);

    return result;

    TEST_FATAL(
        if (stack) stack_region_free(&stack);
        if (_RegionHandle) dlclose(_RegionHandle);
    );
}

TestResult test_stack_region_push_case_3()
{
    try_init_test_fn();

    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    StackRegion *stack = NULL;

    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);

    FuncPtr_stack_region_free  stack_region_free  = try_get_symbol(SYMBOL_FN_STACK_REGION_FREE);
    FuncPtr_stack_region_alloc stack_region_alloc = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);

    stack = stack_region_alloc(TEST_STACK_REGION_CAPACITY, &error); UNWRAP;

    fn(stack, SIZE_MAX, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_LARGE_SIZE, error.code);

    stack_region_free(&stack);

    return result;

    TEST_FATAL(
        if (stack) stack_region_free(&stack);
        if (_RegionHandle) dlclose(_RegionHandle);
    );
}

TestResult test_stack_region_push_case_4()
{
    try_init_test_fn();

    TestResult result = {0};
    
    RegionError error = REGION_ERROR_INIT;
    StackRegion *stack = NULL;

    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE); 

    FuncPtr_stack_region_free  stack_region_free  = try_get_symbol(SYMBOL_FN_STACK_REGION_FREE);
    FuncPtr_stack_region_alloc stack_region_alloc = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);

    stack = stack_region_alloc(TEST_STACK_REGION_CAPACITY, &error); UNWRAP;

    set_available_memory(TEST_STACK_REGION_CAPACITY / 2);

    fn(stack, TEST_STACK_REGION_CAPACITY * 2, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_MALLOC_REGION, error.code);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    stack_region_free(&stack);

    return result;

    TEST_FATAL(
        if (stack) stack_region_free(&stack);
        if (_RegionHandle) dlclose(_RegionHandle);
    );
}

REGISTER_TEST(test_stack_region_push_case_1, 1);
REGISTER_TEST(test_stack_region_push_case_2, 2);
REGISTER_TEST(test_stack_region_push_case_3, 3);
REGISTER_TEST(test_stack_region_push_case_4, 4);

EXPOSE(tests);