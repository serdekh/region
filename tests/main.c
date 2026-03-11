#include <stdio.h>
#include <errno.h>

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

bool testfn__region_log_error()
{
    RegionError error = {0};

    // No errors case:
    __region_log_error(error, TEST_OUT_STREAM);
    
    fseek(TEST_OUT_STREAM, 0, SEEK_END);
    long error_message_length = ftell(TEST_OUT_STREAM);
    if (error_message_length != 0) {
        fprintf(stderr, "[Region][Test]: at __region_log_error()\n\tExpected outstream length to be 0 but got %zu\n", 
            error_message_length);
        return false;
    }

    error.line = 0;
    error.code = REGION_ERROR_TYPE_INVALID_ARGUMENT;
    strcpy(error.file_name, "<TEST_FILE_NAME>");
    strcpy(error.func_name, "<TEST_FUNC_NAME>");
    strcpy(error.message, "<TEST_ERROR_MESSAGE_APPENDIX>");

    // case 0: Invalid argument
    const char *expected = "[Region][ERROR](<TEST_FILE_NAME>:0:<TEST_FUNC_NAME>()): Invalid arguments. <TEST_ERROR_MESSAGE_APPENDIX>.\n";

    __region_log_error(error, TEST_OUT_STREAM);

    fclose(TEST_OUT_STREAM);
    TEST_OUT_STREAM = fopen("temp.txt", "r");

    char *actual = get_str_from_file(TEST_OUT_STREAM);
    if (!actual) {
        fprintf(stderr, "[Region][TEST][Error]: Couldn't read a string from a file (`%s`) for comparison: %s\n", 
            "temp.txt",
            strerror(errno));
        return false;
    }

    if (strcmp(expected, actual) != 0) {
        fprintf(stderr, "[Region][Test]: at __region_log_error() for case 0\n\tExpected:\n\t\t`%s`\n\tBut got:\n\t\t`%s`\n", 
            expected, actual);
        free(actual);
        return false;
    }
    
    free(actual);

    return true;
}

int main()
{
    TEST_OUT_STREAM = fopen("temp.txt", "w");
    if (!TEST_OUT_STREAM) {
        fprintf(stderr, "[Region][TEST][Error]: Couldn't create a temporary file for testing: %s\n", 
            strerror(ferror(TEST_OUT_STREAM)));
        return 1;
    }

    if (!testfn__region_log_error()) {
        fclose(TEST_OUT_STREAM);
        return 1;
    }

    printf("Passed!\n");

    fclose(TEST_OUT_STREAM);
    remove("temp.txt");
    return 0;
}