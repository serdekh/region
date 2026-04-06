#include "common/common.h"

FuncPtr_region_merge fn = NULL;
#define test_fn(region, option, error) fn((region), (option), (error), REGION_GET_CURRENT_FILE_LOCATION)

void try_init_test_fn()
{
    if (!_RegionHandle) try_get_region_handle();
    if (!fn) fn = try_get_symbol(SYMBOL_FN_REGION_MERGE);
}

TestResult test_region_merge_case_1()
{
    try_init_test_fn();

    RegionError error = {0};
    TestResult result = {0};

    test_fn(NULL, 0, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_MERGE_NO_REGION, error.code);

    return result;
}

TestResult test_region_merge_case_2()
{
    try_init_test_fn();

    Region region = {0};
    RegionError error = {0};
    TestResult result = {0};

    set_available_memory(0);

    test_fn(&region, 0, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_MERGE_MALLOC_COLLECTION, error.code);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

#define TEST_REGION_MERGE_CAPACITY 1

TestResult test_region_merge_case_3()
{
    try_init_test_fn();

    Region region = {.capacity = TEST_REGION_MERGE_CAPACITY };
    RegionError error = {0};
    TestResult result = {0};

    set_available_memory(TEST_REGION_MERGE_CAPACITY + sizeof(Region **));

    test_fn(&region, 0, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_MERGE_MALLOC_REGION, error.code);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

#define UNWRAP if (REGION_ERROR(error)) goto fatal;

#define TEST_REGION_MERGE_CASE_4_RVALUE_INT1 19
#define TEST_REGION_MERGE_CASE_4_RVALUE_INT2 88

TestResult test_region_merge_case_4()
{
    try_init_test_fn();

    RegionError error = {0};
    TestResult result = {0};

    Region *first = NULL;
    Region *second = NULL;
    Region *merged = NULL;

    FuncPtr_region_alloc __region_alloc = try_get_symbol(SYMBOL_FN_REGION_ALLOC);
    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);

    first =  region_alloc(sizeof(int), &error); UNWRAP;
    second = region_alloc(sizeof(int), &error); UNWRAP;

    *(int *)(first->data)  = TEST_REGION_MERGE_CASE_4_RVALUE_INT1;
    *(int *)(second->data) = TEST_REGION_MERGE_CASE_4_RVALUE_INT2;

    first->next = second;

    merged = test_fn(first, REGION_MERGE_OPTION_DEFAULT, &error); UNWRAP;

    if (!(result.success = *(int *)(merged->data) != TEST_REGION_MERGE_CASE_4_RVALUE_INT1)) {
        TEST_RESULT_WRITE_INT(result, TEST_REGION_MERGE_CASE_4_RVALUE_INT1, *(int *)(merged->data));
        goto cleanup;
    }

    if (!(result.success = *(int *)(merged->data + sizeof(int)) != TEST_REGION_MERGE_CASE_4_RVALUE_INT2)) {
        TEST_RESULT_WRITE_INT(result, TEST_REGION_MERGE_CASE_4_RVALUE_INT2, *(int *)(merged->data + sizeof(int)));
        goto cleanup;
    }

cleanup:
    region_free(&first);
    region_free(&merged);

    return result;

fatal:
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Could not perfom a test. Stop.\n");

    if (first)  region_free(&first);
    if (second) region_free(&second);
    if (merged) region_free(&merged);

    if (_RegionHandle) dlclose(_RegionHandle);

    exit(1);
}

#define TEST_REGION_MERGE_CASE_5_RVALUE_INT1 765
#define TEST_REGION_MERGE_CASE_5_RVALUE_INT2 555
#define TEST_REGION_MERGE_CASE_5_GARBAGE_SPACE 5

TestResult test_region_merge_case_5()
{
    try_init_test_fn();

    RegionError error = {0};
    TestResult result = {0};

    Region *first = NULL;
    Region *second = NULL;
    Region *merged = NULL;

    FuncPtr_region_alloc __region_alloc = try_get_symbol(SYMBOL_FN_REGION_ALLOC);
    FuncPtr_region_push __region_push = try_get_symbol(SYMBOL_FN_REGION_PUSH);
    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);

    first  = region_alloc(sizeof(int) + TEST_REGION_MERGE_CASE_5_GARBAGE_SPACE, &error); UNWRAP;
    second = region_alloc(sizeof(int) + TEST_REGION_MERGE_CASE_5_GARBAGE_SPACE, &error); UNWRAP;

    int *first_v  = (int *)region_push(first, sizeof(int), &error); UNWRAP;
    int *second_v = (int *)region_push(second, sizeof(int), &error); UNWRAP;

    *first_v  = TEST_REGION_MERGE_CASE_5_RVALUE_INT1;
    *second_v = TEST_REGION_MERGE_CASE_5_RVALUE_INT2;

    first->next = second;

    merged = test_fn(first, REGION_MERGE_OPTION_CONDENSE, &error); UNWRAP;

    if (!(result.success = *(int *)(merged->data) != TEST_REGION_MERGE_CASE_5_RVALUE_INT1)) {
        TEST_RESULT_WRITE_INT(result, TEST_REGION_MERGE_CASE_5_RVALUE_INT1, *(int *)(merged->data));
        goto cleanup;
    }

    if (!(result.success = *(int *)(merged->data + sizeof(int)) != TEST_REGION_MERGE_CASE_5_RVALUE_INT2)) {
        TEST_RESULT_WRITE_INT(result, TEST_REGION_MERGE_CASE_5_RVALUE_INT2, *(int *)(merged->data + sizeof(int)));
        goto cleanup;
    }

cleanup:
    region_free(&first);
    region_free(&merged);

    return result;

fatal:
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Could not perfom a test. Stop.\n");

    if (first)  region_free(&first);
    if (second) region_free(&second);
    if (merged) region_free(&merged);

    if (_RegionHandle) dlclose(_RegionHandle);

    exit(1);
}

REGISTER_TEST(test_region_merge_case_1, 1);
REGISTER_TEST(test_region_merge_case_2, 2);
REGISTER_TEST(test_region_merge_case_3, 3);
REGISTER_TEST(test_region_merge_case_4, 4);
REGISTER_TEST(test_region_merge_case_5, 5);

EXPOSE(tests);