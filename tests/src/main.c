#include "../include/helpers.h"

#define TESTOUT_FILE_PATH "testout.txt"

TestContext testfn__region_log_error_case_0()
{
    TestContext context = get_test_context(0, TESTOUT_FILE_PATH);

    __region_log_error(context.error, context.out_stream);
    
    fseek(context.out_stream, 0, SEEK_END);

    long error_message_length = ftell(context.out_stream);

    if (error_message_length != 0) {
        char error_message_length_str[128];
        sprintf(error_message_length_str, "%ld", error_message_length);
        LOG_TEST_FAILED("__region_log_error", 0, "0", error_message_length_str);
        context.passed = false;
        return context;
    }

    LOG_TEST_PASSED("__region_log_error", 0);
    return context;
}

TestContext testfn__region_log_error_case_1()
{
    TestContext context = get_test_context(REGION_ERROR_TYPE_INVALID_ARGUMENT, TESTOUT_FILE_PATH);

    const char *expected = "[Region][ERROR](<TEST_FILE_NAME>:0:<TEST_FUNC_NAME>()): Invalid arguments. <TEST_ERROR_MESSAGE_APPENDIX>.\n";

    __region_log_error(context.error, context.out_stream);

    char *actual = get_str_from_file(context);

    if (strcmp(expected, actual) != 0) {
        LOG_TEST_FAILED("__region_log_error", 1, expected, actual);
        free(actual);
        context.passed = false;
        return context;
    }

    free(actual);
    LOG_TEST_PASSED("__region_log_error", 1);

    return context;
}

TestContext testfn__region_log_error_case_2()
{
    TestContext context = get_test_context(REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY, TESTOUT_FILE_PATH);

    const char *expected = "[Region][ERROR](<TEST_FILE_NAME>:0:<TEST_FUNC_NAME>()): Not enough memory to allocate. <TEST_ERROR_MESSAGE_APPENDIX>.\n";

    __region_log_error(context.error, context.out_stream);

    char *actual = get_str_from_file(context);

    if (strcmp(expected, actual) != 0) {
        LOG_TEST_FAILED("__region_log_error", 2, expected, actual);
        free(actual);
        context.passed = false;
        return context;
    }

    free(actual);
    LOG_TEST_PASSED("__region_log_error", 2);

    return context;
}

bool testfn__region_log_error(TestContext *context)
{   
    *context = testfn__region_log_error_case_0(); if (!context->passed) goto failed;
    *context = testfn__region_log_error_case_1(); if (!context->passed) goto failed;
    *context = testfn__region_log_error_case_2(); if (!context->passed) goto failed;

    fprintf(stderr, "[Region][Test][Passed]: for the function `__region_log_error`\n");
    return context->passed;

failed:
    fprintf(stderr, "[Region][Test][FAILED]: for the function `__region_log_error`\n");
    return context->passed;
}

TestContext testfn__region_alloc_case_0()
{
    TestContext context = get_test_context(0, TESTOUT_FILE_PATH);
    
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
    TestContext context = get_test_context(0, TESTOUT_FILE_PATH);
    
    region_alloc(0, &context.error);
    
    const char *expected = "[Region][ERROR](<TEST_FILE_NAME>:0:<TEST_FUNC_NAME>()): Invalid arguments. The `region` cannot have `capacity` equal to zero.\n";

    __region_log_error(context.error, context.out_stream);

    char *actual = get_str_from_file(context);

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

TestContext testfn__region_alloc_item_case_0()
{  
    TestContext context = get_test_context(0, TESTOUT_FILE_PATH);
    
    __region_alloc_item(NULL, 1, &context.error, "<TEST_FILE_NAME>", 0, "<TEST_FUNC_NAME>");
    __region_log_error(context.error, context.out_stream);
    
    const char *expected = "[Region][ERROR](<TEST_FILE_NAME>:0:<TEST_FUNC_NAME>()): Invalid arguments. The `region` holds a null reference.\n";

    char *actual = get_str_from_file(context);
    
    context.passed = strcmp(expected, actual) == 0;
    
    context.passed
    ? LOG_TEST_PASSED("__region_alloc_item", 0)
    : LOG_TEST_FAILED("__region_alloc_item", 0, expected, actual);

    free(actual);

    return context;
}

TestContext testfn__region_alloc_item_case_1()
{
    TestContext context = get_test_context(0, TESTOUT_FILE_PATH);

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

        LOG_TEST_FAILED("__region_alloc_item", 1, e, a);
        region_free(&region);
        context.passed = false;
        return context;
    }

    region_free(&region);
    LOG_TEST_PASSED("__region_alloc_item", 1);
    return context;
}

TestContext testfn__region_alloc_item_case_2()
{
    TestContext context = get_test_context(0, TESTOUT_FILE_PATH);

    Region *region = region_alloc(1, &context.error);

    if (!region) {
        fprintf(stderr, "[Region][Test][Error]: Couldn't allocate memory for testing. Stop.\n");
        exit(1);
    }

    region_alloc_item(region, 10, &context.error);

    if (region->next) {
        LOG_TEST_PASSED("__region_alloc_item", 2);
        region_free(&region);
        return context;
    }

    LOG_TEST_FAILED("__region_alloc_item", 2, "The value of the `next` field not to be NULL due to excessive capacity", "Nullable reference");
    context.passed = false;
    region_free(&region);
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


int main()
{
    TestContext context = {0};

    if (!testfn__region_log_error(&context)) goto exiting;
    if (!testfn__region_alloc(&context)) goto exiting;
    if (!testfn__region_alloc_item(&context)) goto exiting;

    printf("[Region][Test]: All tests have passed!\n");
    return 0;

exiting:
    if (context.out_stream) fclose(context.out_stream);
    return (int)context.passed;
}