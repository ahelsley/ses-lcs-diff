all: cdiff

cdiff: diff.c
	gcc -g -O3 -std=c99 diff.c -o cdiff

memcheck: cdiff
	valgrind --leak-check=full -- ./cdiff

cachegrind: cdiff
	valgrind --tool=cachegrind -- ./cdiff
	cg_annotate --PID cdiff diff.c diff.h | grep -Fv '???:'
