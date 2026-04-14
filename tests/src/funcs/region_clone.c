#include "common/common.h"

FuncPtr_region_clone fn = NULL;

#define REGION_TEST_REGION_CLONE_STATIC_DATA "Hello from tests!"

void try_init_test_fn()
{
    if (!_RegionHandle) try_get_region_handle();
    if (!fn) fn = try_get_symbol(SYMBOL_FN_REGION_CLONE);
}

TestResult test_region_clone_case_1()
{
    try_init_test_fn();
    
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    fn(NULL, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_CLONE_NO_REGION, error.code);

    return result;
}

TestResult test_region_clone_case_2()
{
    try_init_test_fn();

    TestResult result = {0};

    Region region     = {0};
    RegionError error = REGION_ERROR_INIT;

    set_available_memory(sizeof(Region *) / 2);

    fn(&region, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_CLONE_MALLOC_ROOT, error.code);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

TestResult test_region_clone_case_3()
{
    try_init_test_fn();

    TestResult  result = {0};

    Region      root   = { .capacity = 1 };
    Region      second = { .capacity = 1 };
    RegionError error = REGION_ERROR_INIT;
    
    root.next = &second;

    set_available_memory(sizeof(root) + root.capacity);

    fn(&root, &error); 

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_CLONE_MALLOC_NODE, error.code);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

TestResult test_region_clone_case_4()
{
    try_init_test_fn();

    TestResult  result = {0};
    
    Region region = {0};
    Region *clone = NULL;
    RegionError error  = REGION_ERROR_INIT;

    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);

    size_t static_data_len = strlen(REGION_TEST_REGION_CLONE_STATIC_DATA);

    region.capacity = region.size = static_data_len;
    region.data = REGION_TEST_REGION_CLONE_STATIC_DATA;

    clone = fn(&region, &error); UNWRAP;

    memcpy(result.actual, clone->data, TEST_RESULT_MAX_STRING_SIZE);
    memcpy(result.expected, REGION_TEST_REGION_CLONE_STATIC_DATA, TEST_RESULT_MAX_STRING_SIZE);
    result.success = strcmp(clone->data, REGION_TEST_REGION_CLONE_STATIC_DATA) == 0;

    region_free(&clone);

    return result;

    TEST_FATAL(
        if (clone) region_free(&clone);
        if (_RegionHandle) dlclose(_RegionHandle);
    );
}

REGISTER_TEST(test_region_clone_case_1, 1);
REGISTER_TEST(test_region_clone_case_2, 2);
REGISTER_TEST(test_region_clone_case_3, 3);
REGISTER_TEST(test_region_clone_case_4, 4);

EXPOSE(tests);