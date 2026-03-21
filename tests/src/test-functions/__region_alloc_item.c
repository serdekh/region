#include  "../../include/test-functions/__region_alloc_item.h"

TestContext testfn__region_alloc_item_case_0()
{  
    TestContext context = tctx_get(0, 0, TESTOUT_FILE_PATH);
    
    __region_alloc_item(NULL, 1, &context.error, context.error.location);
    __region_log_error(context.error, context.out_stream);
    
    const char *expected = "[Region][ERROR]("
                            TEST_CONTEXT_ERROR_DEFAULT_FILE_NAME":"
                            TEST_CONTEXT_ERROR_DEFAULT_LINE_STR":"
                            TEST_CONTEXT_ERROR_DEFAULT_FUNC_NAME
                            "()): Invalid arguments. The `region` holds a null reference.\n";

    char *actual = tctx_get_str_from_file(context);
    
    context.passed = strcmp(expected, actual) == 0;
    
    context.passed
    ? LOG_TEST_PASSED("__region_alloc_item", context)
    : LOG_TEST_FAILED("__region_alloc_item", context, expected, actual);

    free(actual);

    return context;
}

TestContext testfn__region_alloc_item_case_1()
{
    TestContext context = tctx_get(0, 1, TESTOUT_FILE_PATH);

    Region *region = region_alloc(5 * sizeof(int), &context.error);

    if (!region) {
        fprintf(stderr, "[Region][Test][Error]: Couldn't allocate memory for testing. Stop.\n");
        exit(1);
    }

    int *actual = (int*)(region->data);
    
    actual[0] = 1;
    actual[1] = 2;
    actual[2] = 3;
    actual[3] = 4;
    actual[4] = 5;

    int expected[5] = {1,2,3,4,5};

    for (int i = 0; i < 5; i++) {
        if (actual[i] == expected[i]) continue;

        char e[2];           char a[2];
        sprintf(e, "%d", 1); sprintf(a, "%d", 1);

        LOG_TEST_FAILED("__region_alloc_item", context, e, a);
        region_free(&region);
        context.passed = false;
        return context;
    }

    region_free(&region);
    LOG_TEST_PASSED("__region_alloc_item", context);
    return context;
}

TestContext testfn__region_alloc_item_case_2()
{
    TestContext context = tctx_get(0, 2, TESTOUT_FILE_PATH);

    Region *region = region_alloc(1, &context.error);

    if (!region) {
        fprintf(stderr, "[Region][Test][Error]: Couldn't allocate memory for testing. Stop.\n");
        exit(1);
    }

    region_alloc_item(region, 10, &context.error);

    if (region->next) {
        LOG_TEST_PASSED("__region_alloc_item", context);
        region_free(&region);
        return context;
    }

    LOG_TEST_FAILED("__region_alloc_item", context, "The value of the `next` field not to be NULL due to excessive capacity", "Nullable reference");
    context.passed = false;
    region_free(&region);
    return context;

}

bool testfn__region_alloc_item(TestContext *context)
{   
    *context = testfn__region_alloc_item_case_0(); if (!context->passed) goto failed;
    *context = testfn__region_alloc_item_case_1(); if (!context->passed) goto failed;
    *context = testfn__region_alloc_item_case_2(); if (!context->passed) goto failed;

    fprintf(stderr, "[Region][Test][Passed]: for the function `__region_alloc_item`\n");
    return context->passed;

failed:
    fprintf(stderr, "[Region][Test][FAILED]: for the function `__region_alloc_item`\n");
    return context->passed;
}