#include "./common/common.h"

FuncPtr_region_shrink_capacity fn = NULL;

#define TEST_REGION_SHRINK_CAPACITY_CASE3_CAPACITY (size_t)10
#define TEST_REGION_SHRINK_CAPACITY_CASE3_CAPACITY_SHRINKED (TEST_REGION_SHRINK_CAPACITY_CASE3_CAPACITY) / 2

#define TEST_REGION_SHRINK_CAPACITY_CASE4_NODE1_CAPACITY (size_t)5
#define TEST_REGION_SHRINK_CAPACITY_CASE4_NODE1_CAPACITY_SHRINKED (TEST_REGION_SHRINK_CAPACITY_CASE4_NODE1_CAPACITY) / 2
#define TEST_REGION_SHRINK_CAPACITY_CASE4_NODE2_CAPACITY (size_t)11
#define TEST_REGION_SHRINK_CAPACITY_CASE4_NODE2_CAPACITY_SHRINKED (TEST_REGION_SHRINK_CAPACITY_CASE4_NODE2_CAPACITY) / 2

void try_init_test_fn()
{
    if (!_RegionHandle) try_get_region_handle();
    if (!fn) fn = try_get_symbol(SYMBOL_FN_REGION_SHRINK_CAPACITY);
}

TestResult test_region_shrink_capacity_case_1()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    fn(NULL, REGION_SHRINK_CAPACITY_OPTION_ONLY_ROOT, &error);
   
    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_SHRINK_CAPACITY_NO_REGION, error.code);
    
    return result;
}

TestResult test_region_shrink_capacity_case_2()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};
    Region r = {0};

    r.size = SIZE_MAX;

    fn(&r, REGION_SHRINK_CAPACITY_OPTION_ONLY_ROOT, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_SHRINK_CAPACITY_MALLOC, error.code);

    return result;
}

TestResult test_region_shrink_capacity_case_3()
{
    try_init_test_fn();

    TestResult result = {0};

    Region *r = NULL;
    RegionError error = {0};

    FuncPtr_region_push region_push = try_get_symbol(SYMBOL_FN_REGION_PUSH);
    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);
    FuncPtr_region_alloc region_alloc = try_get_symbol(SYMBOL_FN_REGION_ALLOC);

    r = region_alloc(TEST_REGION_SHRINK_CAPACITY_CASE3_CAPACITY, &error); UNWRAP;

    region_push(r, TEST_REGION_SHRINK_CAPACITY_CASE3_CAPACITY_SHRINKED, &error); UNWRAP;

    fn(r, REGION_SHRINK_CAPACITY_OPTION_ONLY_ROOT, &error); UNWRAP;

    sprintf(result.expected, "Capacity after shrinking: %zu", TEST_REGION_SHRINK_CAPACITY_CASE3_CAPACITY_SHRINKED);
    sprintf(result.actual, "Actual capacity after shrinking: %zu", r->capacity);

    result.success = r->capacity == TEST_REGION_SHRINK_CAPACITY_CASE3_CAPACITY_SHRINKED;

    region_free(&r);

    return result;
    
fatal:    
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Failed to perfom a test. Stop.\n");

    if (r) region_free(&r);
    if (_RegionHandle) dlclose(_RegionHandle);

    exit(1);
}

TestResult test_region_shrink_capacity_case_4()
{
    try_init_test_fn();

    TestResult result = {0};
    
    Region *node_1 = NULL;
    Region *node_2 = NULL;
    RegionError error = {0};

    FuncPtr_region_push region_push = try_get_symbol(SYMBOL_FN_REGION_PUSH);
    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);
    FuncPtr_region_alloc region_alloc = try_get_symbol(SYMBOL_FN_REGION_ALLOC);

    node_1 = region_alloc(TEST_REGION_SHRINK_CAPACITY_CASE4_NODE1_CAPACITY, &error); UNWRAP;
    node_2 = region_alloc(TEST_REGION_SHRINK_CAPACITY_CASE4_NODE2_CAPACITY, &error); UNWRAP;

    region_push(node_1, TEST_REGION_SHRINK_CAPACITY_CASE4_NODE1_CAPACITY_SHRINKED, &error); UNWRAP;
    region_push(node_2, TEST_REGION_SHRINK_CAPACITY_CASE4_NODE2_CAPACITY_SHRINKED, &error); UNWRAP;

    node_1->next = node_2;

    fn(node_1, REGION_SHRINK_CAPACITY_OPTION_ALL, &error); UNWRAP;

    sprintf(result.expected, "Capacity after shrinking: { First node: %zu, Second node: %zu}", 
        TEST_REGION_SHRINK_CAPACITY_CASE4_NODE1_CAPACITY_SHRINKED,
        TEST_REGION_SHRINK_CAPACITY_CASE4_NODE2_CAPACITY_SHRINKED);

    sprintf(result.actual, "Actual capacity after shrinking: { First node: %zu, Second node: %zu}", 
        node_1->capacity,
        node_2->capacity);

    result.success = 
        node_1->capacity == TEST_REGION_SHRINK_CAPACITY_CASE4_NODE1_CAPACITY_SHRINKED &&
        node_2->capacity == TEST_REGION_SHRINK_CAPACITY_CASE4_NODE2_CAPACITY_SHRINKED;

    region_free(&node_1);

    return result;
    
fatal:    
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Could not allocate memory for a testing function. Stop.");

    if (node_1) region_free(&node_1);
    if (_RegionHandle) dlclose(_RegionHandle);

    exit(1);
}

REGISTER_TEST(test_region_shrink_capacity_case_1, 1);
REGISTER_TEST(test_region_shrink_capacity_case_2, 2);
REGISTER_TEST(test_region_shrink_capacity_case_3, 3);
REGISTER_TEST(test_region_shrink_capacity_case_4, 4);

EXPOSE(tests);