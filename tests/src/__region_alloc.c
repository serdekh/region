#include "../include/__region_log_error.h"

TestContext testfn__region_alloc_case_0()
{
    TestContext context = tctx_get(0, TESTOUT_FILE_PATH);
    
    Region *region = region_alloc(__SIZE_MAX__, &context.error);
    
    if (context.error.code == REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY) {
        LOG_TEST_PASSED("__region_alloc", 0);
        if (region) free(region);
        return context;
    }
    
    LOG_TEST_FAILED("__region_alloc", 0, 
        error_code_to_str(REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY), 
        error_code_to_str(context.error.code));
        
    if (region) free(region);
    context.passed = false;
    
    return context;
}

TestContext testfn__region_alloc_case_1()
{
    TestContext context = tctx_get(0, TESTOUT_FILE_PATH);
    
    region_alloc(0, &context.error);
    
    const char *expected = "[Region][ERROR](<TEST_FILE_NAME>:0:<TEST_FUNC_NAME>()): Invalid arguments. The `region` cannot have `capacity` equal to zero.\n";

    __region_log_error(context.error, context.out_stream);

    char *actual = tctx_get_str_from_file(context);

    if (strcmp(expected, actual) != 0) {
        LOG_TEST_FAILED("__region_alloc", 1, expected, actual);
        free(actual);
        context.passed = false;
        return context;
    }
    
    free(actual);
    LOG_TEST_PASSED("__region_alloc", 1);
    return context;
}

bool testfn__region_alloc(TestContext *context)
{   
    *context = testfn__region_alloc_case_0(); if (!context->passed) goto failed;

    fprintf(stderr, "[Region][Test][Passed]: for the function `__region_alloc`\n");
    return context->passed;

failed:
    fprintf(stderr, "[Region][Test][FAILED]: for the function `__region_alloc`\n");
    return context->passed;
}