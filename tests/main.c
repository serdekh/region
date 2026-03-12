#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "../src/region.h"

FILE *TEST_OUT_STREAM = NULL;

char *get_str_from_file(FILE *file)
{
    if (!file) return NULL; 
    
    fseek(file, 0, SEEK_END); 
    size_t size = ftell(file); 
    rewind(file); 

    char *buffer = malloc(size + 1); 
    fread(buffer, 1, size, file); 
    buffer[size] = '\0';

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

bool testfn__region_log_error_case_0()
{
    RegionError error = {0};

    __region_log_error(error, TEST_OUT_STREAM);
    
    fseek(TEST_OUT_STREAM, 0, SEEK_END);
    long error_message_length = ftell(TEST_OUT_STREAM);
    if (error_message_length != 0) {
        char error_message_length_str[128];
        sprintf(error_message_length_str, "%ld", error_message_length);
        LOG_TEST_FAILED("__region_log_error", 0, "0", error_message_length_str);
        return false;
    }

    LOG_TEST_PASSED("__region_log_error", 0);
    return true;
}

bool testfn__region_log_error_case_1()
{
    RegionError error = get_region_error_for_testing(REGION_ERROR_TYPE_INVALID_ARGUMENT);

    const char *expected = "[Region][ERROR](<TEST_FILE_NAME>:0:<TEST_FUNC_NAME>()): Invalid arguments. <TEST_ERROR_MESSAGE_APPENDIX>.\n";

    __region_log_error(error, TEST_OUT_STREAM);

    fclose(TEST_OUT_STREAM);
    TEST_OUT_STREAM = fopen("temp.txt", "r");

    char *actual = get_str_from_file(TEST_OUT_STREAM);

    if (!actual) {
        fprintf(stderr, "[Region][Test][Error]: Couldn't read the contents of a temporary stream. Terminating...\n");
        fclose(TEST_OUT_STREAM);
        exit(1);
    }

    if (strcmp(expected, actual) != 0) {
        LOG_TEST_FAILED("__region_log_error", 1, expected, actual);
        free(actual);
        return false;
    }

    free(actual);
    fclose(TEST_OUT_STREAM);
    TEST_OUT_STREAM = fopen("temp.txt", "w");
    LOG_TEST_PASSED("__region_log_error", 1);
    return true;
}

bool testfn__region_log_error_case_2()
{
    RegionError error = get_region_error_for_testing(REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY);

    const char *expected = "[Region][ERROR](<TEST_FILE_NAME>:0:<TEST_FUNC_NAME>()): Not enough memory to allocate. <TEST_ERROR_MESSAGE_APPENDIX>.\n";

    __region_log_error(error, TEST_OUT_STREAM);

    fclose(TEST_OUT_STREAM);
    TEST_OUT_STREAM = fopen("temp.txt", "r");

    char *actual = get_str_from_file(TEST_OUT_STREAM);

    if (!actual) {
        fprintf(stderr, "[Region][Test][Error]: Couldn't read the contents of a temporary stream. Terminating...\n");
        fclose(TEST_OUT_STREAM);
        exit(1);
    }

    if (strcmp(expected, actual) != 0) {
        LOG_TEST_FAILED("__region_log_error", 2, expected, actual);
        free(actual);
        return false;
    }

    free(actual);
    fclose(TEST_OUT_STREAM);
    TEST_OUT_STREAM = fopen("temp.txt", "w");
    LOG_TEST_PASSED("__region_log_error", 2);
    return true;
}

bool testfn__region_log_error()
{   
    bool case0 = testfn__region_log_error_case_0(); if (!case0) goto failed;
    bool case1 = testfn__region_log_error_case_1(); if (!case1) goto failed;
    bool case2 = testfn__region_log_error_case_2(); if (!case2) goto failed;

    fprintf(stderr, "[Region][Test][Passed]: for the function `__region_log_error`\n");
    return true;

failed:
    fprintf(stderr, "[Region][Test][FAILED]: for the function `__region_log_error`\n");
    return false;   
}

bool testfn__region_alloc_case_0()
{
    //Region *__region_alloc(size_t capacity, RegionError *error, const char *filename, int line, const char *func);

    RegionError error = {0};
    
    Region *region = region_alloc(__SIZE_MAX__, &error);
    
    if (error.code == REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY) {
        LOG_TEST_PASSED("__region_alloc", 0);
        return true;
    }
    
    LOG_TEST_FAILED("__region_alloc", 0, 
        error_code_to_str(REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY), 
        error_code_to_str(error.code));
        
    if (region) free(region);
    
    return false;
}

// void *__region_alloc_item(Region *region, size_t size, RegionError *error, const char *filename, int line, const char *func)
bool testfn__region_alloc_item_case_0()
{  
    RegionError error = get_region_error_for_testing(0);
    
    __region_alloc_item(NULL, 1, &error, "<TEST_FILE_NAME>", 0, "<TEST_FUNC_NAME>");
    __region_log_error(error, TEST_OUT_STREAM);
    
    const char *expected = "[Region][ERROR](<TEST_FILE_NAME>:0:<TEST_FUNC_NAME>()): Invalid arguments. The `region` holds a null reference.\n";
    
    fclose(TEST_OUT_STREAM);
    TEST_OUT_STREAM = fopen("temp.txt", "r");

    char *actual = get_str_from_file(TEST_OUT_STREAM);

    
    bool result = strcmp(expected, actual) == 0;
    
    result
    ? LOG_TEST_PASSED("__region_alloc_item", 0)
    : LOG_TEST_FAILED("__region_alloc_item", 0, expected, actual);
    
    fclose(TEST_OUT_STREAM);
    TEST_OUT_STREAM = fopen("temp.txt", "w");

    free(actual);

    return result;
}

bool testfn__region_alloc()
{
    bool case0 = testfn__region_alloc_case_0(); if (!case0) goto failed;

    printf("[Region][Test][Passed]: for the function `__region_alloc`\n");
    return true;

failed:
    fprintf(stderr, "[Region][Test][FAILED]: for the function `__region_alloc`\n");
    return false;   
}

bool testfn__region_alloc_item()
{
    bool case0 = testfn__region_alloc_item_case_0(); if (!case0) goto failed;

    printf("[Region][Test][Passed]: for the function `__region_alloc_item`\n");
    return true;

failed:
    fprintf(stderr, "[Region][Test][FAILED]: for the function `__region_alloc_item`\n");
    return false;   
}

int main()
{
    int result = 0;
    TEST_OUT_STREAM = fopen("temp.txt", "w");

    if (!TEST_OUT_STREAM) {
        fprintf(stderr, "[Region][TEST][Error]: Couldn't create a temporary file for testing: %s\n", 
            strerror(ferror(TEST_OUT_STREAM)));
        return 1;
    }

    if (!testfn__region_log_error()) {
        result = 1; goto exiting;
    }

    if (!testfn__region_alloc()) {
        result = 1; goto exiting;
    }

    if (!testfn__region_alloc_item()) {
        result = 1; goto exiting;
    }
    
    printf("[Region][Test]: All tests have passed!\n");

exiting:
    if (TEST_OUT_STREAM) {
        fclose(TEST_OUT_STREAM);
        remove("temp.txt");
    }
    return result;
}