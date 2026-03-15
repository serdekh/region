#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <assert.h>

#include <errno.h>

#include "../../src/region.h"

#define TESTOUT_FILE_PATH ".build/testout.txt"

// Represents the data structure that the helpers and the
// testing functions work with. It contains essential
// fields for managing the output stream for tests and
// the error struct for checking a result of a function
// that may fail. 
typedef struct {
    const char *test_file_path;
    FILE *out_stream;
    RegionError error;
    bool passed;
} TestContext;

TestContext tctx_get(ErrorCode code, const char *file_path);

char *tctx_get_str_from_file(TestContext context);
void  tctx_init_field_error(TestContext *context, ErrorCode code);

char *error_code_to_str(ErrorCode code);

void log_test_failed(char *func_name, int case_n, const char *fmt, ...);
void log_test_passed(char *func_name, int case_n);

#define LOG_TEST_FAILED(func_name, case_n, expected, actual) log_test_failed((func_name), 1, "\tExpected:\n\t\t`%s`\n\tBut got:\n\t\t`%s`\n", (expected), (actual))
#define LOG_TEST_PASSED(func_name, case_n) log_test_passed((func_name), (case_n))