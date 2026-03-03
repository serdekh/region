make:
	gcc -std=gnu99 -DREGION_IMPLEMENTATION -x c -c src/region.h
#	gcc src/test.c region.o