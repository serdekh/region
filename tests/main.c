#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "../src/region.h"

#define TESTOUT_FILE_PATH "testout.txt"

typedef struct {
    const char *test_file_path;
    FILE *out_stream;
    RegionError error;
    bool passed;
} TestContext;

char *get_str_from_file(FILE *file)
{
    if (!file) return NULL; 

    fclose(file);
    file = fopen(TESTOUT_FILE_PATH, "r");

    if (!file) {
        fprintf(stderr, "[Region][Test][Error]: Could not create a temporary file stream: %s\n", strerror(errno));
        exit(1);
    }
    
    fseek(file, 0, SEEK_END); 
    size_t size = ftell(file); 
    rewind(file); 

    char *buffer = malloc(size + 1); 

    if (!buffer) {
        fprintf(stderr, "[Region][Test][Error]: Could not read a string from a temporary file stream: %s\n", strerror(errno));
        exit(1);
    }

    fread(buffer, 1, size, file); 
    buffer[size] = '\0';

    fclose(file);
    file = fopen(TESTOUT_FILE_PATH, "w");

    if (!file) {
        fprintf(stderr, "[Region][Test][Error]: Could not create a temporary file stream: %s\n", strerror(errno));
        exit(1);
    }

    return buffer;
}

RegionError get_region_error_for_testing(ErrorCode code)
{
    RegionError error = {0};

    error.line = 0;
    error.code = code;
    strcpy(error.file_name, "<TEST_FILE_NAME>");
    strcpy(error.func_name, "<TEST_FUNC_NAME>");
    strcpy(error.message, "<TEST_ERROR_MESSAGE_APPENDIX>");

    return error;
}

void print_test_failed(char *func_name, int case_n, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stderr,
        "[Region][Test][FAILED]: for the function `%s` (case %d):\n",
        func_name, case_n);

    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);
}

char *error_code_to_str(ErrorCode code)
{
    switch (code) {
        case REGION_ERROR_TYPE_NO_ERROR:
            return "No error";
        case REGION_ERROR_TYPE_INVALID_ARGUMENT:
            return "Invalid argument";
        case REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY:
            return "Not enough memory";
        default:
            return "Unreachable";
    }
}

void print_test_passed(char *func_name, int case_n)
{
    printf("[Region][Test][Passed]: for the function `%s` (case %d).\n",
        func_name, case_n);
}

#define LOG_TEST_FAILED(func_name, case_n, expected, actual) print_test_failed((func_name), 1, "\tExpected:\n\t\t`%s`\n\tBut got:\n\t\t`%s`\n", (expected), (actual))
#define LOG_TEST_PASSED(func_name, case_n) print_test_passed((func_name), (case_n))

TestContext get_test_context(ErrorCode code, const char *file_path)
{
    TestContext context = {0};

    if(!context.out_stream) context.out_stream = fopen(file_path, "w");

    if (!context.out_stream) goto fatal;

    context.error = get_region_error_for_testing(code);
    context.test_file_path = file_path;
    context.passed = true;
    
    return context;

fatal:
    fprintf(stderr, "[Region][Test][Error]: Could not initialize the test system: %s\n", strerror(errno));
    exit(1);
}

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

    char *actual = get_str_from_file(context.out_stream);

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

    char *actual = get_str_from_file(context.out_stream);

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
        return context;
    }
    
    LOG_TEST_FAILED("__region_alloc", 0, 
        error_code_to_str(REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY), 
        error_code_to_str(context.error.code));
        
    if (region) free(region);
    
    return context;
}

TestContext testfn__region_alloc_item_case_0()
{  
    TestContext context = get_test_context(0, TESTOUT_FILE_PATH);
    
    __region_alloc_item(NULL, 1, &context.error, "<TEST_FILE_NAME>", 0, "<TEST_FUNC_NAME>");
    __region_log_error(context.error, context.out_stream);
    
    const char *expected = "[Region][ERROR](<TEST_FILE_NAME>:0:<TEST_FUNC_NAME>()): Invalid arguments. The `region` holds a null reference.\n";

    char *actual = get_str_from_file(context.out_stream);
    
    context.passed = strcmp(expected, actual) == 0;
    
    context.passed
    ? LOG_TEST_PASSED("__region_alloc_item", 0)
    : LOG_TEST_FAILED("__region_alloc_item", 0, expected, actual);

    free(actual);

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