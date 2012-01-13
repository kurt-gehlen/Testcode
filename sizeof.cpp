#include <stdio.h>

struct jim
{
	int a;
	int b;
};

struct jimbo
{
	long a;
	long b;
};


int
main(int argc,char ** argv)
{
	printf("int = %d, long = %d, long long = %d\n", sizeof(int), sizeof(long), sizeof(long long));
	printf("int * = %d, void * = %d\n", sizeof(int *),sizeof(void*));
	printf("jim = %d, jimbo = %d\n", sizeof(jim),sizeof(jimbo));
	printf("done\n");
}
