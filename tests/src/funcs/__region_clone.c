#include "common/common.h"

FuncPtr_region_clone fn = NULL;

#define test_fn(region, error) fn((region), (error), (REGION_GET_CURRENT_FILE_LOCATION))

void try_init_test_fn()
{
    if (!_RegionHandle) try_get_region_handle();
    if (!fn) fn = try_get_symbol(SYMBOL_FN_REGION_CLONE);
}

TestResult test_region_clone_case_1()
{
    try_init_test_fn();

    RegionError error = {0};
    TestResult result = {0};

    test_fn(NULL, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_CLONE_NO_REGION, error.code);

    return result;
}

TestResult test_region_clone_case_2()
{
    try_init_test_fn();

    Region region     = {0};
    RegionError error = {0};
    TestResult result = {0};

    set_available_memory(sizeof(Region *) / 2);

    test_fn(&region, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_CLONE_MALLOC_ROOT, error.code);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

TestResult test_region_clone_case_3()
{
    try_init_test_fn();

    Region      root   = { .capacity = 1 };
    Region      second = { .capacity = 1 };
    RegionError error  = {0};
    TestResult  result = {0};

    root.next = &second;

    set_available_memory(sizeof(root) + root.capacity);

    test_fn(&root, &error); 

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_CLONE_MALLOC_NODE, error.code);

    set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

#define REGION_TEST_REGION_CLONE_STATIC_DATA "Hello from tests!"
#define UNWRAP if(REGION_ERROR(error)) goto fatal

TestResult test_region_clone_case_4()
{
    try_init_test_fn();

    Region region = {0};    
    Region *clone = NULL;

    RegionError error  = {0};
    TestResult  result = {0};

    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);

    size_t static_data_len = strlen(REGION_TEST_REGION_CLONE_STATIC_DATA);

    region.capacity = region.size = static_data_len;
    region.data = REGION_TEST_REGION_CLONE_STATIC_DATA;

    clone = test_fn(&region, &error); UNWRAP;

    memcpy(result.actual, clone->data, TEST_RESULT_MAX_STRING_SIZE);
    memcpy(result.expected, REGION_TEST_REGION_CLONE_STATIC_DATA, TEST_RESULT_MAX_STRING_SIZE);
    result.success = strcmp(clone->data, REGION_TEST_REGION_CLONE_STATIC_DATA) == 0;

    region_free(&clone);

    return result;

fatal:
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Could not perfom a test. Stop.\n");

    if (clone) region_free(&clone);
    if (_RegionHandle) dlclose(_RegionHandle);

    exit(1);
}

REGISTER_TEST(test_region_clone_case_1, 1);
REGISTER_TEST(test_region_clone_case_2, 2);
REGISTER_TEST(test_region_clone_case_3, 3);
REGISTER_TEST(test_region_clone_case_4, 4);

EXPOSE(tests);