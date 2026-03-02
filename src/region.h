#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>

//TODO: Redefine the struct to hide the fields inside a macro
typedef struct {
    size_t capacity;
    size_t size;
    char *data;
} Region;

//TODO: Extract the libc function (malloc|free) into a separate macro
//TODO: Rewrite errno with the custom logger 
Region *region_alloc(size_t capacity);
void region_free(Region **region);
void *region_alloc_item(Region *region, size_t size);
void region_reset(Region *region);

#ifdef REGION_IMPLEMENTATION

Region *region_alloc(size_t capacity)
{
    Region *region = (Region *)malloc(sizeof(Region));

    if (!region) {
        errno = ENOMEM;
        return NULL;
    }

    region->data = (char *)malloc(sizeof(char) * capacity);
    
    if (!region->data) {
        free(region);
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

    if ((*region)->data) free((*region)->data);

    free(*region);
}

void *region_alloc_item(Region *region, size_t size)
{
    if (!region) {
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