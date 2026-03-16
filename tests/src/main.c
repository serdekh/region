#include "../include/funcs.h"

int main()
{
    TestContext context = {0};

    if (!testfn__region_log_error(&context))  goto fatal;
    if (!testfn__region_alloc(&context))      goto fatal;
    if (!testfn__region_alloc_item(&context)) goto fatal;
    if (!testfn__region_free(&context))       goto fatal;

    printf("[Region][Test]: All function tests have passed!\n");
    return 0;

fatal:
    if (context.out_stream) fclose(context.out_stream);
    return (int)context.passed;
}