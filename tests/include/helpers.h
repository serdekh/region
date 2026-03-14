#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <errno.h>

#include "../../src/region.h"

typedef struct {
    const char *test_file_path;
    FILE *out_stream;
    RegionError error;
    bool passed;
} TestContext;

char *get_str_from_file(TestContext context);
char *error_code_to_str(ErrorCode code);

RegionError get_region_error_for_testing(ErrorCode code);

void print_test_failed(char *func_name, int case_n, const char *fmt, ...);
void print_test_passed(char *func_name, int case_n);

#define LOG_TEST_FAILED(func_name, case_n, expected, actual) print_test_failed((func_name), 1, "\tExpected:\n\t\t`%s`\n\tBut got:\n\t\t`%s`\n", (expected), (actual))
#define LOG_TEST_PASSED(func_name, case_n) print_test_passed((func_name), (case_n))

TestContext get_test_context(ErrorCode code, const char *file_path);