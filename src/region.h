// ----- HEADERS -----
#ifndef REGION_NO_STDIO
#include <stdio.h>
#define REGION_STDIN stdin
#define REGION_STDOUT stdout
#define REGION_STDERR stderr
#define REGION_PRINTF(...) printf(__VA_ARGS__)
#define REGION_FPRINTF(...) fprintf(__VA_ARGS__)
#define REGION_SPRINTF(...) sprintf(__VA_ARGS__)
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

#ifdef __cplusplus
  #define REGION_EXTERN_C_BEGIN extern "C" {
  #define REGION_EXTERN_C_END   }
#else
  #define REGION_EXTERN_C_BEGIN
  #define REGION_EXTERN_C_END
#endif

// ----- DATA STRUCTS -----

#define REGION_PRIVATE_CORE_FIELDS \
    size_t capacity;               \
    size_t size;                   \
    char *data;                    \

typedef struct __Region {
    REGION_PRIVATE_CORE_FIELDS
    struct __Region *next;
} Region;

// ----- DATA STRUCTS FOR ERRORS -----

typedef char ErrorCode;

#define REGION_ERROR_TYPE_NO_ERROR          (ErrorCode)0
#define REGION_ERROR_TYPE_INVALID_ARGUMENT  (ErrorCode)1
#define REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY (ErrorCode)2

typedef struct {
    int line;
    ErrorCode code;
    char file_name[256];
    char func_name[32];
    char message[256];
} RegionError;

REGION_EXTERN_C_BEGIN

// ----- FUNCTION DECLARATIONS (PRIVATE) -----
void __region_set_error(RegionError *error, ErrorCode error_code, const char *filename, int line, const char *func);
Region *__region_alloc(size_t capacity, RegionError *error, const char *filename, int line, const char *func);
void *__region_alloc_item(Region *region, size_t size, RegionError *error, const char *filename, int line, const char *func);
void __region_reset(Region *region);

// ----- PUBLIC API -----
void region_log_error(RegionError error);
void region_free(Region **region);

#define region_reset(region) __region_reset(region)
#define region_alloc(capacity, error) __region_alloc((capacity), (error), __FILE__, __LINE__, __func__)
#define region_alloc_item(region, size, error) __region_alloc_item((region), (size), (error), __FILE__, __LINE__, __func__)

// ----- * -----

#ifdef REGION_IMPLEMENTATION

void __region_set_error(RegionError *error, ErrorCode error_code, const char *filename, int line, const char *func)
{
    error->line = line;
    error->code = error_code;
    REGION_STRNCPY(error->file_name, filename, REGION_STRLEN(filename));
    REGION_STRNCPY(error->func_name, func, REGION_STRLEN(func));
}

Region *__region_alloc(size_t capacity, RegionError *error, const char *filename, int line, const char *func)
{
    Region *region = (Region *)REGION_MALLOC(sizeof(Region));

    if (!region) {
        if (error) {
            __region_set_error(error, REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY, filename, line, func);
        }
        return NULL;
    }

    region->data = (char *)REGION_MALLOC(sizeof(char) * capacity);
    
    if (!region->data) {
        REGION_FREE(region);
        if (error) {
            __region_set_error(error, REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY, filename, line, func);
        }
        return NULL;
    }

    region->capacity = capacity;
    region->size = 0;
    region->next = NULL;

    for (size_t i = 0; i < region->capacity; i++) region->data[i] = '\0';

    return region;
}

void region_log_error(RegionError error)
{
    if (error.code == REGION_ERROR_TYPE_NO_ERROR) return;

    REGION_FPRINTF(REGION_STDERR, "[Region][ERROR](%s:%d:%s()): ",
        error.file_name,
        error.line,
        error.func_name);

    switch (error.code) {
        case REGION_ERROR_TYPE_INVALID_ARGUMENT:
            REGION_FPRINTF(REGION_STDERR, "Invalid arguments.");
            break;
        case REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY:
            REGION_FPRINTF(REGION_STDERR, "Not enough memory to allocate.");
            break;
        default:
            break;
    }

    if (REGION_STRLEN(error.message) == 0) {
        REGION_FPRINTF(REGION_STDERR, "\n");
    }
    else {
        REGION_FPRINTF(REGION_STDERR, " %s.\n", error.message);
    }
}

void region_free(Region **region)
{
    if (!region || !(*region)) return;

    while (*region) {
        Region *temp = (*region)->next;

        if ((*region)->data) REGION_FREE((*region)->data);

        REGION_FREE(*region);

        *region = temp;
    }
}

void *__region_alloc_item(Region *region, size_t size, RegionError *error, const char *filename, int line, const char *func)
{
    if (!region) {
        if (error) {
            __region_set_error(error, REGION_ERROR_TYPE_INVALID_ARGUMENT, filename, line, func);
            REGION_SPRINTF(error->message, "The `region` holds a null reference");
        }
        return NULL;
    }
    if (size == 0) {
        if (error) {
            __region_set_error(error, REGION_ERROR_TYPE_INVALID_ARGUMENT, filename, line, func);
            REGION_SPRINTF(error->message, "The value of `size` cannot be equal to zero");
        }
        return NULL;
    }

    Region *current = region;

    while (current->size + size > current->capacity) {
        if (!current->next) {
            current->next = region_alloc(current->capacity * 2 + size, NULL);
            if (!current->next) {
                if (error) {
                    __region_set_error(error, REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY, filename, line, func);
                    REGION_SPRINTF(error->message, ".\nWarning: The current `region` is not freed. Use `region_free` to avoid a memory leak");
                }
                return NULL;
            }
            current = current->next;
            break;
        }

        current = current->next;
    }

    void *result = current->data + current->size;

    current->size += size;

    return result;
}

void __region_reset(Region *region)
{
    for (Region *i = region; i; i = i->next) {
        i->size = 0;
    }
}

REGION_EXTERN_C_END

#endif // REGION_IMPLEMENTATION