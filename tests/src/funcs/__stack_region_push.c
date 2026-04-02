#include "./common/common.h"

FuncPtr_stack_region_push fn = NULL;

void try_init_test_fn()
{
    try_get_region_handle();
    
    if (!fn) fn = try_get_symbol(SYMBOL_FN_STACK_REGION_PUSH);
}

TestResult test_stack_region_push_case_1()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    fn(NULL, 1, &error, REGION_GET_CURRENT_FILE_LOCATION);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_NO_STACK_REGION, error.code);

    return result;
}

#define TEST_STACK_REGION_CAPACITY 1

TestResult test_stack_region_push_case_2()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    FuncPtr_stack_region_alloc __stack_region_alloc = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);
    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);

    StackRegion *stack = __stack_region_alloc(TEST_STACK_REGION_CAPACITY, &error, REGION_GET_CURRENT_FILE_LOCATION);

    if (!stack) {
        REGION_LOG_ERROR(error);
        fprintf(stderr, "[Test][Error]: Could not allocate a stack region to perfom a test.\n");
        exit(1);
    }

    fn(stack, 0, &error, REGION_GET_CURRENT_FILE_LOCATION);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_SMALL_SIZE, error.code);

    stack_region_free(&stack);

    return result;
}

TestResult test_stack_region_push_case_3()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    FuncPtr_stack_region_alloc __stack_region_alloc = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);
    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);

    StackRegion *stack = __stack_region_alloc(TEST_STACK_REGION_CAPACITY, &error, REGION_GET_CURRENT_FILE_LOCATION);

    if (!stack) {
        REGION_LOG_ERROR(error);
        fprintf(stderr, "[Test][Error]: Could not allocate a stack region to perfom a test.\n");
        exit(1);
    }

    fn(stack, SIZE_MAX, &error, REGION_GET_CURRENT_FILE_LOCATION);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_LARGE_SIZE, error.code);

    stack_region_free(&stack);

    return result;
}

TestResult test_stack_region_push_case_4()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    FuncPtr_stack_region_alloc __stack_region_alloc = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);
    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);

    StackRegion *stack = __stack_region_alloc(TEST_STACK_REGION_CAPACITY, &error, REGION_GET_CURRENT_FILE_LOCATION);

    if (!stack) {
        REGION_LOG_ERROR(error);
        fprintf(stderr, "[Test][Error]: Could not allocate a stack region to perfom a test.\n");
        exit(1);
    }

    set_available_memory(TEST_STACK_REGION_CAPACITY / 2);

    fn(stack, TEST_STACK_REGION_CAPACITY, &error, REGION_GET_CURRENT_FILE_LOCATION);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_MALLOC_REGION, error.code);

    stack_region_free(&stack);
    
    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

REGISTER_TEST(test_stack_region_push_case_1, 1);
REGISTER_TEST(test_stack_region_push_case_2, 2);
REGISTER_TEST(test_stack_region_push_case_3, 3);
REGISTER_TEST(test_stack_region_push_case_4, 4);

EXPOSE(tests);