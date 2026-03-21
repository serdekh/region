#include "../include/helpers.h"

TestContext tctx_get(ErrorCode code, int current_case, const char *file_path)
{
    TestContext context = {0};

    if(!context.out_stream) context.out_stream = fopen(file_path, "w");

    if (!context.out_stream) goto fatal;

    context.test_file_path = file_path;
    context.passed = true;
    context.current_case = current_case;
    
    tctx_init_field_error(&context, code);

    return context;

fatal:
    fprintf(stderr, "[Region][Test][Error]: Could not initialize a TestContext struct: %s\n", strerror(errno));
    exit(1);
}

char *tctx_get_str_from_file(TestContext context)
{
    if (!context.out_stream) return NULL; 

    char *buffer = NULL;

    fclose(context.out_stream);
    context.out_stream = fopen(context.test_file_path, "r");
    if (!context.out_stream) goto fatal;
    
    fseek(context.out_stream, 0, SEEK_END); 
    size_t size = ftell(context.out_stream); 
    rewind(context.out_stream); 

    buffer = (char *)malloc(size + 1); 
    if (!buffer) goto fatal;

    fread(buffer, 1, size, context.out_stream); 
    buffer[size] = '\0';

    fclose(context.out_stream);
    context.out_stream = fopen(context.test_file_path, "w");
    if (!context.out_stream) goto fatal;

    return buffer;

fatal:
    fprintf(stderr, "[Region][Test][Error]: Could not read a string from the TestContext stream: %s\n", strerror(errno));
    
    if (buffer) free(buffer);
    if (context.out_stream) fclose(context.out_stream);
    
    exit(1);
}


void tctx_init_field_error(TestContext *context, ErrorCode code)
{
    assert(context != NULL);
    
    context->error.code = code;

    context->error.location.line = TEST_CONTEXT_ERROR_DEFAULT_LINE;
    context->error.location.file_name = TEST_CONTEXT_ERROR_DEFAULT_FILE_NAME;
    context->error.location.func_name = TEST_CONTEXT_ERROR_DEFAULT_FUNC_NAME;
    
    strcpy(context->error.message,   TEST_CONTEXT_ERROR_DEFAULT_MESSAGE);
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

void log_test_failed(char *func_name, TestContext context, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stderr,
        "[Region][Test][FAILED]: for the function `%s` (case %d):\n",
        func_name, context.current_case);

    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);
}


void log_test_passed(char *func_name, TestContext context)
{
    printf("[Region][Test][Passed]: for the function `%s` (case %d).\n",
        func_name, context.current_case);
}