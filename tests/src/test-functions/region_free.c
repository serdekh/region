#include  "../../include/test-functions/region_free.h"

TestContext testfn__region_free_case_0()
{
    TestContext context = tctx_get(0, 0, TESTOUT_FILE_PATH);

    Region *node_0 = region_alloc(1, &context.error);
    Region *node_1 = region_alloc(1, &context.error);
    Region *node_2 = region_alloc(1, &context.error);
    Region *node_3 = region_alloc(1, &context.error);

    if (context.error.code != 0) {
        fprintf(stderr, "[Region][Test][Error]: Could not allocate nodes for the tests: %s\n", 
            error_code_to_str(context.error.code));
        exit(1);
    }  

    node_0->next = node_1;
    node_1->next = node_2;
    node_2->next = node_3;

    region_free(&node_0);

    if (node_0) {
        LOG_TEST_FAILED("__region_free", context, "All nodes to be freed.", "The head is still being allocated.");
        context.passed = false;
    }

    return context;
}

bool testfn__region_free(TestContext *context) 
{
    *context = testfn__region_free_case_0(); if (!context->passed) goto failed;

    fprintf(stderr, "[Region][Test][Passed]: for the function `__region_free`\n");
    return context->passed;

failed:
    fprintf(stderr, "[Region][Test][FAILED]: for the function `__region_free`\n");
    return context->passed;
}