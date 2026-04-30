#pragma once

#if defined (_WIN32)
	#define RT_FILE_PATHS_SEP "\\"
	#define RT_FILE_PATHS_EXT_SO "dll"
#else
	#define RT_FILE_PATHS_SEP "/"
	#define RT_FILE_PATHS_EXT_SO "so"
#endif

#define RT_FILE_PATHS_REGION_SO ".."RT_FILE_PATHS_SEP"lib"RT_FILE_PATHS_SEP"region-test."RT_FILE_PATHS_EXT_SO
