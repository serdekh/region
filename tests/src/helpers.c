#include "../include/helpers.h"

char *get_str_from_file(TestContext context)
{
    if (!context.out_stream) return NULL; 

    fclose(context.out_stream);
    context.out_stream = fopen(context.test_file_path, "r");

    if (!context.out_stream) {
        fprintf(stderr, "[Region][Test][Error]: Could not create a temporary file stream: %s\n", strerror(errno));
        exit(1);
    }
    
    fseek(context.out_stream, 0, SEEK_END); 
    size_t size = ftell(context.out_stream); 
    rewind(context.out_stream); 

    char *buffer = (char *)malloc(size + 1); 

    if (!buffer) {
        fprintf(stderr, "[Region][Test][Error]: Could not read a string from a temporary file stream: %s\n", strerror(errno));
        exit(1);
    }

    fread(buffer, 1, size, context.out_stream); 
    buffer[size] = '\0';

    fclose(context.out_stream);
    context.out_stream = fopen(context.test_file_path, "w");

    if (!context.out_stream) {
        fprintf(stderr, "[Region][Test][Error]: Could not create a temporary file stream: %s\n", strerror(errno));
        free(buffer);
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
    fprintf(stderr, "[Region][Test][Error]: Could not initialize the test context: %s\n", strerror(errno));
    exit(1);
}