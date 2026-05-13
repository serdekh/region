#include "../include/rt-shared.h"

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

    Region *merged = api->region_merge(NULL, 0, &error);

    RT_TEST_RESULT_WRITE_PTR(result, NULL, merged);

    return result;
}

TestResult test_region_merge_case_2(RegionAPI *api)
{
    TestResult result = {0};

    Region region = {0};
    RegionError error = REGION_ERROR_INIT;

    api->test_set_available_memory(0);

    api->region_merge(&region, 0, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION_ARRAY, error.message);

    api->test_set_default_memory();

    return result;
}


TestResult test_region_merge_case_3(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    Region *first = NULL;
    Region *second = NULL;
    Region *merged = NULL;

    first =  api->region_alloc(sizeof(int), &error); RT_TARGET_UNWRAP;
    second = api->region_alloc(sizeof(int), &error); RT_TARGET_UNWRAP;

    *(int *)(first->data)  = TEST_REGION_MERGE_CASE_4_RVALUE_INT1;
    *(int *)(second->data) = TEST_REGION_MERGE_CASE_4_RVALUE_INT2;

    first->next = second;

    merged = api->region_merge(first, REGION_MERGE_OPTION_DEFAULT, &error); RT_TARGET_UNWRAP;

    if (!(result.success = *(int *)(merged->data) != TEST_REGION_MERGE_CASE_4_RVALUE_INT1)) {
        RT_TEST_RESULT_WRITE_INT(result, TEST_REGION_MERGE_CASE_4_RVALUE_INT1, *(int *)(merged->data));
        goto cleanup;
    }

    if (!(result.success = *(int *)(merged->data + sizeof(int)) != TEST_REGION_MERGE_CASE_4_RVALUE_INT2)) {
        RT_TEST_RESULT_WRITE_INT(result, TEST_REGION_MERGE_CASE_4_RVALUE_INT2, *(int *)(merged->data + sizeof(int)));
        goto cleanup;
    }

cleanup:
    api->region_free(&first);
    api->region_free(&merged);

    return result;

    RT_TARGET_FATAL_ERROR(
        if (first)  api->region_free(&first);
        if (merged) api->region_free(&merged);
    );
}

TestResult test_region_merge_case_4(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    Region *first = NULL;
    Region *second = NULL;
    Region *merged = NULL;

    first  = api->region_alloc(sizeof(int) + TEST_REGION_MERGE_CASE_5_GARBAGE_SPACE, &error); RT_TARGET_UNWRAP;
    second = api->region_alloc(sizeof(int) + TEST_REGION_MERGE_CASE_5_GARBAGE_SPACE, &error); RT_TARGET_UNWRAP;

    int *first_v  = (int *)api->region_push(&first, sizeof(int), &error); RT_TARGET_UNWRAP;
    int *second_v = (int *)api->region_push(&second, sizeof(int), &error); RT_TARGET_UNWRAP;

    *first_v  = TEST_REGION_MERGE_CASE_5_RVALUE_INT1;
    *second_v = TEST_REGION_MERGE_CASE_5_RVALUE_INT2;

    first->next = second;

    merged = api->region_merge(first, REGION_MERGE_OPTION_CONDENSE, &error); RT_TARGET_UNWRAP;

    if (!(result.success = *(int *)(merged->data) != TEST_REGION_MERGE_CASE_5_RVALUE_INT1)) {
        RT_TEST_RESULT_WRITE_INT(result, TEST_REGION_MERGE_CASE_5_RVALUE_INT1, *(int *)(merged->data));
        goto cleanup;
    }

    if (!(result.success = *(int *)(merged->data + sizeof(int)) != TEST_REGION_MERGE_CASE_5_RVALUE_INT2)) {
        RT_TEST_RESULT_WRITE_INT(result, TEST_REGION_MERGE_CASE_5_RVALUE_INT2, *(int *)(merged->data + sizeof(int)));
        goto cleanup;
    }

cleanup:
    api->region_free(&first);
    api->region_free(&merged);

    return result;

    RT_TARGET_FATAL_ERROR(
        if (first)  api->region_free(&first);
        if (merged) api->region_free(&merged);
    );
}

RT_TEST_MODULE_REGISTER(test_region_merge_case_1, 1);
RT_TEST_MODULE_REGISTER(test_region_merge_case_2, 2);
RT_TEST_MODULE_REGISTER(test_region_merge_case_3, 3);
RT_TEST_MODULE_REGISTER(test_region_merge_case_4, 4);

RT_TEST_MODULE_EXPORT;