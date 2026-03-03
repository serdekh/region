// ----- HEADERS -----
#ifndef REGION_NO_STDIO
#include <stdio.h>
#define REGION_STDIN stdin
#define REGION_STDOUT stdout
#define REGION_STDERR stderr
#define REGION_PRINTF(...) printf(__VA_ARGS__)
#define REGION_FPRINTF(...) fprintf(__VA_ARGS__)
#endif // REGION_NO_STDIO

#ifndef REGION_NO_STRING
#include <string.h>
#define REGION_STRLEN strlen
#define REGION_STRNCPY strncpy
#define REGION_STRCPY strcpy
#endif // REGION_NO_STRING

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
#endif // REGION_NO_STDBOOL

// ----- DATA STRUCTS -----

//TODO: Redefine the struct to hide the fields inside a macro
typedef struct {
    size_t capacity;
    size_t size;
    char *data;
} Region;

// ----- DATA STRUCTS FOR ERRORS -----

// TODO: Redefine the values to be more specific depending on a function from
// which the error has occured.
typedef enum {
    REGION_ERROR_KIND_NOT_FOUND,
    REGION_ERROR_KIND_INVALID_ARGUMENTS,
    REGION_ERROR_KIND_NOT_ENOUGH_MEMORY
} RegionErrorKind;

// TODO: Remove the macros or redefine their sizes 
// to be more efficient in terms of memory usage 
#define REGION_ERROR_FILE_NAME_CAPACITY 64
#define REGION_ERROR_FUNC_CAPACITY 64
#define REGION_ERROR_DATA_CAPACITY 1024

typedef struct {
    
    char filename[REGION_ERROR_FILE_NAME_CAPACITY];
    char func[REGION_ERROR_FUNC_CAPACITY];
    char data[REGION_ERROR_DATA_CAPACITY];
    int line;
    RegionErrorKind kind;
} RegionError;

#define REGION_ERRORS_ITEMS_CAPACITY 256
typedef struct {
    RegionError items[REGION_ERRORS_ITEMS_CAPACITY];
    int size;
} RegionErrors;

static RegionErrors __region_errors = { .size = 0 };

// ----- FUNCTION DECLARATIONS (PRIVATE) -----
void __region_push_error(const char *filename, int line, const char *func, RegionErrorKind kind, char *data);
Region *__region_alloc(size_t capacity, const char *filename, int line, const char *func);
void *__region_alloc_item(Region *region, size_t size, const char *filename, int line, const char *func);


// ----- PUBLIC API -----
void region_log_error();
void region_free(Region **region);

#define region_reset(region) (region)->size = 0
#define region_alloc(capacity) __region_alloc((capacity), __FILE__, __LINE__, __func__)
#define region_alloc_item(region, size) __region_alloc_item((region), (size), __FILE__, __LINE__, __func__)

// ----- * -----

#ifdef REGION_IMPLEMENTATION

void __region_push_error(const char *filename, int line, const char *func, RegionErrorKind kind, char *data)
{
    // TODO: Handle the region errors overflow by shifting all of the errors
    // to the left and pushing the new ones at the end
    if (__region_errors.size >= REGION_ERRORS_ITEMS_CAPACITY) return;

    RegionError new_error = __region_errors.items[__region_errors.size];

    new_error.line = line;
    new_error.kind = kind;

    if (filename) REGION_STRNCPY(new_error.filename, filename, REGION_STRLEN(filename));
    if (func) REGION_STRNCPY(new_error.func, func, REGION_STRLEN(func));
    if (data) REGION_STRNCPY(new_error.data, data, REGION_STRLEN(data));

    __region_errors.items[__region_errors.size] = new_error;
    __region_errors.size += 1;

}

Region *__region_alloc(size_t capacity, const char *filename, int line, const char *func)
{
    Region *region = (Region *)REGION_MALLOC(sizeof(Region));

    if (!region) {
        __region_push_error(filename, line, func, REGION_ERROR_KIND_NOT_ENOUGH_MEMORY, NULL);
        return NULL;
    }

    region->data = (char *)REGION_MALLOC(sizeof(char) * capacity);
    
    if (!region->data) {
        REGION_FREE(region);
        __region_push_error(filename, line, func, REGION_ERROR_KIND_NOT_ENOUGH_MEMORY, NULL);
        return NULL;
    }

    region->capacity = capacity;
    region->size = 0;

    return region;
}

void region_log_error()
{
    if (__region_errors.size == 0) return;

    RegionError last_error = __region_errors.items[__region_errors.size - 1];

    if (last_error.kind == REGION_ERROR_KIND_NOT_FOUND) return;

    REGION_FPRINTF(REGION_STDERR, "[Region][ERROR](%s:%d:%s()): ",
        last_error.filename,
        last_error.line,
        last_error.func);

    switch (last_error.kind)
    {
        case REGION_ERROR_KIND_INVALID_ARGUMENTS:
            REGION_FPRINTF(REGION_STDERR, "Invalid arguments\n");
            break;
        case REGION_ERROR_KIND_NOT_ENOUGH_MEMORY:
            REGION_FPRINTF(REGION_STDERR, "Not enough memory to allocate\n");
            break;
        default:
            break;
    }
}

void region_free(Region **region)
{
    if (!region || !(*region)) return;

    if ((*region)->data) REGION_FREE((*region)->data);

    REGION_FREE(*region);
}

void *__region_alloc_item(Region *region, size_t size, const char *filename, int line, const char *func)
{
    if (!region || size == 0) {
        __region_push_error(filename, line, func, REGION_ERROR_KIND_INVALID_ARGUMENTS, NULL);
        return NULL;
    }

    void *result = region->data + region->size;

    region->size += size;

    return result;
}

#endif // REGION_IMPLEMENTATION