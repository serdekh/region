#define REGION_NO_STDLIB
#include <stdlib.h>

#define REGION_TEST_AVAILABLE_MEMORY_DEFAULT 2048

void *test_malloc(size_t capacity);
void __test_set_available_memory(size_t value);
void __test_set_default_memory();
size_t __test_get_available_memory();

#define REGION_MALLOC test_malloc
#define REGION_FREE free

#define REGION_IMPLEMENTATION
#include "../../../src/region.h"

static size_t __test_available_memory = REGION_TEST_AVAILABLE_MEMORY_DEFAULT;

void __test_set_available_memory(size_t value) 
{
    __test_available_memory = value;
}

void __test_set_default_memory() 
{
    __test_available_memory = REGION_TEST_AVAILABLE_MEMORY_DEFAULT;
}

size_t __test_get_available_memory()
{
    return __test_available_memory;
}

void *test_malloc(size_t capacity)
{
    if (capacity > __test_available_memory) return NULL;

    if (__test_available_memory - capacity > __test_available_memory) {
        __test_available_memory = 0;
        return NULL;
    }

    __test_available_memory -= capacity;

    return malloc(capacity);
}