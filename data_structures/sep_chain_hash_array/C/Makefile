
all: sc_hash.o
	g++ -O3 -I../../seg_array_ll/C -g -o test test.cpp sc_hash.o ../../seg_array_ll/C/seg_array_list.o

sc_hash.o: sc_hash.c sc_hash.h
	gcc -O3 -I../../seg_array_ll/C -g -c sc_hash.c
