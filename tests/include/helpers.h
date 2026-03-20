#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <assert.h>

#include <errno.h>

#include "../../src/region.h"

#define TESTOUT_FILE_PATH ".build/testout.txt"

#define TEST_CONTEXT_ERROR_DEFAULT_LINE      0
#define TEST_CONTEXT_ERROR_DEFAULT_LINE_STR  "0"
#define TEST_CONTEXT_ERROR_DEFAULT_FILE_NAME "<file>"
#define TEST_CONTEXT_ERROR_DEFAULT_FUNC_NAME "<func>"
#define TEST_CONTEXT_ERROR_DEFAULT_MESSAGE   "<message>"

// Represents the data structure that the helpers and the
// testing functions work with. It contains essential
// fields for managing the output stream for tests and
// the error struct for checking a result of a function
// that may fail. 
typedef struct {
    int current_case;
    const char *test_file_path;
    FILE *out_stream;
    RegionError error;
    bool passed;
} TestContext;

TestContext tctx_get(ErrorCode code, int current_case, const char *file_path);

char *tctx_get_str_from_file(TestContext context);
void  tctx_init_field_error(TestContext *context, ErrorCode code);

char *error_code_to_str(ErrorCode code);

void log_test_failed(char *func_name, TestContext context, const char *fmt, ...);
void log_test_passed(char *func_name, TestContext context);

#define LOG_TEST_FAILED(func_name, context, expected, actual) log_test_failed((func_name), (context), "\tExpected:\n\t\t`%s`\n\tBut got:\n\t\t`%s`\n", (expected), (actual))
#define LOG_TEST_PASSED(func_name, context) log_test_passed((func_name), (context))