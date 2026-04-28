#pragma once

#if defined(_WIN32)
	#define RT_FILE_PATHS_TARGETS   ".\\.build\\obj\\funcs.dll"
	#define RT_FILE_PATHS_REGION_SO "..\\.build\\sobj\\region-test.dll"
#else
	#define RT_FILE_PATHS_TARGETS   "./.build/obj/funcs.so"
	#define RT_FILE_PATHS_REGION_SO "../.build/sobj/region-test.so"
#endif
