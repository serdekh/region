#include "../include/__region_log_error.h"

TestContext testfn__region_log_error_case_0()
{
    TestContext context = tctx_get(0, 0, TESTOUT_FILE_PATH);

    __region_log_error(context.error, context.out_stream);
    
    fseek(context.out_stream, 0, SEEK_END);

    long error_message_length = ftell(context.out_stream);

    if (error_message_length != 0) {
        char error_message_length_str[128];
        sprintf(error_message_length_str, "%ld", error_message_length);
        LOG_TEST_FAILED("__region_log_error", context, "0", error_message_length_str);
        context.passed = false;
        return context;
    }

    LOG_TEST_PASSED("__region_log_error", context);
    return context;
}

TestContext testfn__region_log_error_case_1()
{
    TestContext context = tctx_get(REGION_ERROR_TYPE_INVALID_ARGUMENT, 1, TESTOUT_FILE_PATH);

    const char *expected = "[Region][ERROR](<TEST_FILE_NAME>:0:<TEST_FUNC_NAME>()): Invalid arguments. <TEST_ERROR_MESSAGE_APPENDIX>.\n";

    __region_log_error(context.error, context.out_stream);

    char *actual = tctx_get_str_from_file(context);

    if (strcmp(expected, actual) != 0) {
        LOG_TEST_FAILED("__region_log_error", context, expected, actual);
        free(actual);
        context.passed = false;
        return context;
    }

    free(actual);
    LOG_TEST_PASSED("__region_log_error", context);

    return context;
}

TestContext testfn__region_log_error_case_2()
{
    TestContext context = tctx_get(REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY, 2, TESTOUT_FILE_PATH);

    const char *expected = "[Region][ERROR](<TEST_FILE_NAME>:0:<TEST_FUNC_NAME>()): Not enough memory to allocate. <TEST_ERROR_MESSAGE_APPENDIX>.\n";

    __region_log_error(context.error, context.out_stream);

    char *actual = tctx_get_str_from_file(context);

    if (strcmp(expected, actual) != 0) {
        LOG_TEST_FAILED("__region_log_error", context, expected, actual);
        free(actual);
        context.passed = false;
        return context;
    }

    free(actual);
    LOG_TEST_PASSED("__region_log_error", context);

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