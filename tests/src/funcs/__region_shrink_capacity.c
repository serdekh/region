#include "./common/common.h"

FuncPtr_region_shrink_capacity fn = NULL;

void try_init_test_fn()
{
    if (_RegionHandle && fn) return;

    try_get_region_handle();
    
    fn = try_get_symbol(SYMBOL_FN_REGION_SHRINK_CAPACITY);
}

TestResult test_region_shrink_capacity_case_1()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    fn(NULL, &error, REGION_GET_CURRENT_FILE_LOCATION);
   
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

    fn(&r, &error, REGION_GET_CURRENT_FILE_LOCATION);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_SHRINK_CAPACITY_MALLOC, error.code);

    return result;
}

#define CAPACITY (size_t)10
#define SHRINKED_CAPACITY (CAPACITY) / 2

TestResult test_region_shrink_capacity_case_3()
{
    try_init_test_fn();

    FuncPtr_region_alloc __region_alloc = try_get_symbol(SYMBOL_FN_REGION_ALLOC);
    FuncPtr_region_push __region_push = try_get_symbol(SYMBOL_FN_REGION_PUSH);
    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);

    TestResult result = {0};
    RegionError error = {0};

    Region *r = __region_alloc(CAPACITY, &error, REGION_GET_CURRENT_FILE_LOCATION);
        if (REGION_ERROR(error)) goto fatal;

    __region_push(r, SHRINKED_CAPACITY, &error, REGION_GET_CURRENT_FILE_LOCATION);
        if (REGION_ERROR(error)) goto fatal;

    fn(r, &error, REGION_GET_CURRENT_FILE_LOCATION);
        if (REGION_ERROR(error)) goto fatal;

    sprintf(result.expected, "Capacity after shrinking: %zu", SHRINKED_CAPACITY);
    sprintf(result.expected, "Actual capacity after shrinking: %zu", r->capacity);

    result.success = r->capacity == SHRINKED_CAPACITY;

    region_free(&r);

    return result;
    
fatal:    
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Could not allocate memory for a testing function. Stop.");
    exit(1);
}

REGISTER_TEST(test_region_shrink_capacity_case_1, 1);
REGISTER_TEST(test_region_shrink_capacity_case_2, 2);
REGISTER_TEST(test_region_shrink_capacity_case_3, 3);

EXPOSE(tests);