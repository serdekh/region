#pragma once

#if defined (_WIN32)
	#define RT_FILE_PATHS_SEP "\\"
	#define RT_FILE_PATHS_EXT_SO "dll"
	#define RT_FILE_PATHS_REGION_SO "..\\lib\\region-test.dll"
#else
	#define RT_FILE_PATHS_SEP "/"
	#define RT_FILE_PATHS_EXT_SO "so"
	#define RT_FILE_PATHS_REGION_SO ".build/lib/region-test.so"
#endif
