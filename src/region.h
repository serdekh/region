#include <errno.h>

//TODO: Redefine the struct to hide the fields inside a macro
typedef struct {
    size_t capacity;
    size_t size;
    char *data;
} Region;

#ifndef REGION_NO_STDLIB
#include <stdlib.h>
#define REGION_MALLOC malloc
#define REGION_FREE free
#endif // REGION_NO_STDLIB

#ifndef REGION_NO_ASSERT
#include <assert.h>
#define REGION_ASSERT assert
#endif // REGION_NO_ASSERT

#ifndef REGION_NO_STDBOOL
#include <stdbool.h>
#define REGION_BOOL bool
#define REGION_BOOL_TRUE true
#define REGION_BOOL_FALSE false
#endif // REGION_NO_BOOL

//TODO: Rewrite errno with the custom logger 
Region *region_alloc(size_t capacity);
void region_free(Region **region);
void *region_alloc_item(Region *region, size_t size);
void region_reset(Region *region);

#ifdef REGION_IMPLEMENTATION

Region *region_alloc(size_t capacity)
{
    Region *region = (Region *)REGION_MALLOC(sizeof(Region));

    if (!region) {
        errno = ENOMEM;
        return NULL;
    }

    region->data = (char *)REGION_MALLOC(sizeof(char) * capacity);
    
    if (!region->data) {
        REGION_FREE(region);
        errno = ENOMEM;
        return NULL;
    }

    region->capacity = capacity;
    region->size = 0;

    return region;
}

void region_free(Region **region)
{
    if (!region || !(*region)) return;

    if ((*region)->data) REGION_FREE((*region)->data);

    REGION_FREE(*region);
}

void *region_alloc_item(Region *region, size_t size)
{
    if (!region || size == 0) {
        errno = EINVAL;
        return NULL;
    }

    void *result = region->data + region->size;

    region->size += size;

    return result;
}

void region_reset(Region *region)
{
    region->size = 0;
}

#endif // REGION_IMPLEMENTATION