#include "common/common.h"

FuncPtr_region_collect fn = NULL;

#define test_fn(region, collected_size, error) fn((region), (collected_size), (error), REGION_GET_CURRENT_FILE_LOCATION)

void try_init_test_fn()
{
    if (!_RegionHandle) try_get_region_handle();
    if (!fn) fn = try_get_symbol(SYMBOL_FN_REGION_COLLECT);
}

TestResult test_region_collect_case_1()
{
    try_init_test_fn();

    RegionError error = {0};
    TestResult result = {0};
    size_t collected_size = 0;

    test_fn(NULL, &collected_size, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_COLLECT_NO_REGION, error.code);

    return result;
}

TestResult test_region_collect_case_2()
{
    try_init_test_fn();

    Region region = {0};
    RegionError error = {0};
    TestResult result = {0};

    test_fn(&region, NULL, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_COLLECT_NO_COLLECTED_SIZE, error.code);

    return result;
}

TestResult test_region_collect_case_3()
{
    try_init_test_fn();

    Region region = {0};
    RegionError error = {0};
    TestResult result = {0};
    size_t collected_size = {0};

    set_available_memory(0);

    test_fn(&region, &collected_size, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_COLLECT_MALLOC_COLLECTION, error.code);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

REGISTER_TEST(test_region_collect_case_1, 1);
REGISTER_TEST(test_region_collect_case_2, 2);
REGISTER_TEST(test_region_collect_case_3, 3);

EXPOSE(tests);