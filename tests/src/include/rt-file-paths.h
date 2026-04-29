#pragma once

#if defined(_WIN32)
	#define RT_FILE_PATHS_REGION_SO "..\\lib\\region-test.dll"
#else
	#define RT_FILE_PATHS_REGION_SO "../lib/region-test.so"
#endif
