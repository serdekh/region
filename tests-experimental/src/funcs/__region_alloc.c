#include "../include/shared.h"
#include "../../../src/region.h"

#define REGION_ALLOC_FUNCTION_NAME "__region_alloc"
#define REGION_SHARED_OBJECT_FILE_PATH "../.build/obj/region.so"

typedef Region*(*FuncPtr_region_alloc)(size_t, RegionError*, RegionLocation);

// TODO: Extract the symbol look-up part into a separate function so it
// could be reused by other testing function modules
FuncPtr_region_alloc get_region_alloc()
{
    const char *region_handle_file_path = REGION_SHARED_OBJECT_FILE_PATH;
    
    void *region_handle = dlopen(region_handle_file_path, RTLD_LAZY);
    
    if (!region_handle) {
        fprintf(stderr, "[Test][Error]: Could not load the region shared library (%s): %s\n",
            region_handle_file_path, dlerror());
        exit(1);
    }
    
    FuncPtr_region_alloc fptr_region_alloc = dlsym(region_handle, REGION_ALLOC_FUNCTION_NAME);
    
    if (!fptr_region_alloc) {
        fprintf(stderr, "[Test][Error]: Could not find the '%s' function symbol (%s): %s\n",
            REGION_ALLOC_FUNCTION_NAME, region_handle_file_path, dlerror());
        dlclose(region_handle);
        exit(1);
    }

    return fptr_region_alloc;
}

bool region_alloc_case_1() 
{
    FuncPtr_region_alloc fptr_region_alloc = get_region_alloc();

    RegionError error = {0};

    fptr_region_alloc(0, &error, REGION_GET_CURRENT_FILE_LOCATION);

    return error.code == REGION_ERROR_CODE_EINVAL_REGION_ALLOC_SMALL_CAPACITY;
}

REGISTER_TEST(region_alloc_case_1, 1);
EXPOSE(tests)
