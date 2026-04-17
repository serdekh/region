#include "../include/shared.h"

#define TEST_REGION_MERGE_CAPACITY 1
#define TEST_REGION_MERGE_CASE_4_RVALUE_INT1 19
#define TEST_REGION_MERGE_CASE_4_RVALUE_INT2 88
#define TEST_REGION_MERGE_CASE_5_RVALUE_INT1 765
#define TEST_REGION_MERGE_CASE_5_RVALUE_INT2 555
#define TEST_REGION_MERGE_CASE_5_GARBAGE_SPACE 5

TestResult test_region_merge_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->region_merge(NULL, 0, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_MERGE_NO_REGION, error.code);

    return result;
}

TestResult test_region_merge_case_2(RegionAPI *api)
{
    TestResult result = {0};

    Region region = {0};
    RegionError error = REGION_ERROR_INIT;

    api->test_set_available_memory(0);

    api->region_merge(&region, 0, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_MERGE_MALLOC_COLLECTION, error.code);

    api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

TestResult test_region_merge_case_3(RegionAPI *api)
{
    TestResult result = {0};

    Region region = {.capacity = TEST_REGION_MERGE_CAPACITY };
    RegionError error = REGION_ERROR_INIT;

    api->test_set_available_memory(TEST_REGION_MERGE_CAPACITY + sizeof(Region **));

    api->region_merge(&region, 0, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_MERGE_MALLOC_REGION, error.code);

    api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

TestResult test_region_merge_case_4(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    Region *first = NULL;
    Region *second = NULL;
    Region *merged = NULL;

    first =  api->region_alloc(sizeof(int), &error); UNWRAP;
    second = api->region_alloc(sizeof(int), &error); UNWRAP;

    *(int *)(first->data)  = TEST_REGION_MERGE_CASE_4_RVALUE_INT1;
    *(int *)(second->data) = TEST_REGION_MERGE_CASE_4_RVALUE_INT2;

    first->next = second;

    merged = api->region_merge(first, REGION_MERGE_OPTION_DEFAULT, &error); UNWRAP;

    if (!(result.success = *(int *)(merged->data) != TEST_REGION_MERGE_CASE_4_RVALUE_INT1)) {
        TEST_RESULT_WRITE_INT(result, TEST_REGION_MERGE_CASE_4_RVALUE_INT1, *(int *)(merged->data));
        goto cleanup;
    }

    if (!(result.success = *(int *)(merged->data + sizeof(int)) != TEST_REGION_MERGE_CASE_4_RVALUE_INT2)) {
        TEST_RESULT_WRITE_INT(result, TEST_REGION_MERGE_CASE_4_RVALUE_INT2, *(int *)(merged->data + sizeof(int)));
        goto cleanup;
    }

cleanup:
    api->region_free(&first);
    api->region_free(&merged);

    return result;

    TEST_FATAL(
        if (first)  api->region_free(&first);
        if (merged) api->region_free(&merged);
    );
}

TestResult test_region_merge_case_5(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    Region *first = NULL;
    Region *second = NULL;
    Region *merged = NULL;

    first  = api->region_alloc(sizeof(int) + TEST_REGION_MERGE_CASE_5_GARBAGE_SPACE, &error); UNWRAP;
    second = api->region_alloc(sizeof(int) + TEST_REGION_MERGE_CASE_5_GARBAGE_SPACE, &error); UNWRAP;

    int *first_v  = (int *)api->region_push(first, sizeof(int), &error); UNWRAP;
    int *second_v = (int *)api->region_push(second, sizeof(int), &error); UNWRAP;

    *first_v  = TEST_REGION_MERGE_CASE_5_RVALUE_INT1;
    *second_v = TEST_REGION_MERGE_CASE_5_RVALUE_INT2;

    first->next = second;

    merged = api->region_merge(first, REGION_MERGE_OPTION_CONDENSE, &error); UNWRAP;

    if (!(result.success = *(int *)(merged->data) != TEST_REGION_MERGE_CASE_5_RVALUE_INT1)) {
        TEST_RESULT_WRITE_INT(result, TEST_REGION_MERGE_CASE_5_RVALUE_INT1, *(int *)(merged->data));
        goto cleanup;
    }

    if (!(result.success = *(int *)(merged->data + sizeof(int)) != TEST_REGION_MERGE_CASE_5_RVALUE_INT2)) {
        TEST_RESULT_WRITE_INT(result, TEST_REGION_MERGE_CASE_5_RVALUE_INT2, *(int *)(merged->data + sizeof(int)));
        goto cleanup;
    }

cleanup:
    api->region_free(&first);
    api->region_free(&merged);

    return result;

    TEST_FATAL(
        if (first)  api->region_free(&first);
        if (merged) api->region_free(&merged);
    );
}

REGISTER_TEST(test_region_merge_case_1, 1);
REGISTER_TEST(test_region_merge_case_2, 2);
REGISTER_TEST(test_region_merge_case_3, 3);
REGISTER_TEST(test_region_merge_case_4, 4);
REGISTER_TEST(test_region_merge_case_5, 5);

EXPOSE(tests);