#pragma once 

#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#if defined(_WIN32)
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

#include "../include/rt-shared.h"

#ifdef _WIN32
    
    typedef HMODULE dl_handle;

    #define dlopen(path, flags) LoadLibraryA(path)
    #define dlsym(handle, name) GetProcAddress(handle, name)
    #define dlclose(handle) FreeLibrary(handle)

    static const char *dlerror(void) {
		static char buffer[256];

	    DWORD err = GetLastError();
    	FormatMessageA(
	        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	        NULL,
	        err,
	        0,
	        buffer,
	        sizeof(buffer),
	        NULL
	    );

    	return buffer;
	}
#else
	typedef void* dl_handle;
#endif

void *rt_try_load_shared_object(const char *file_path);

bool rt_try_load_file_and_test(RegionAPI *api, const char *file_path);

bool rt_try_load_files_and_test(const char *directory);
