#include "../include/funcs.h"

int main()
{
    TestContext context = {0};

    if (!testfn__region_log_error(&context)) goto exiting;
    if (!testfn__region_alloc(&context)) goto exiting;
    if (!testfn__region_alloc_item(&context)) goto exiting;
    if (!testfn__region_free(&context)) goto exiting;

    printf("[Region][Test]: All tests have passed!\n");
    return 0;

exiting:
    if (context.out_stream) fclose(context.out_stream);
    return (int)context.passed;
}