#include <stdio.h>

int
main(int argc,char ** argv)
{
	printf("int = %d, long = %d, long long = %d\n", sizeof(int), sizeof(long), sizeof(long long));
	printf("int * = %d, void * = %d\n", sizeof(int *),sizeof(void*));
}
