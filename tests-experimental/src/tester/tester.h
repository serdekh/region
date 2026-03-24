#include "../../../src/region.h"
#include "../include/shared.h"

const TestContext *(*get_start)(void);
const TestContext *(*get_end  )(void);

bool load_and_test(const char *file_path);