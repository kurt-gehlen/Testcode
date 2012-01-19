
#include "SortedArray.h"

#include <iostream>
#include <string>
#include <ext/hash_set>

#include <getopt.h>

using namespace std;
using namespace __gnu_cxx;

int compareLong( void * a, void * b )
{
	long i = *(long *)a;
	long j = *(long *)b;
	
	if ( i < j )
		return -1;
	else if ( i > j )
		return 1;
	
	return 0;
}

void printLong( void * a )
{
	if ( !a )
		printf("What???\n");
	long i = *(long *)a;
	cout << i << endl;
}



int
main( int argc, char ** argv )
{
	SortedArray a;
	long v;
	int icnt = 0,dcnt = 0,sdcnt = 0;
	
	unsigned int seed = time(0);
	long range = 0;

	char c;
    while ( (c = getopt(argc,argv,"s:r:e")) != -1 )
    {
        switch(c)
        {
            case 's':
            {
                seed = atoi(optarg);

            } break;

            case 'r':
            {
                range = atoi(optarg);

            } break;

            default:

                printf("bad option: %c\n",c);

                return -1;;
        }
    }

    if ( optind < argc )
    {
		v = atoi(argv[optind]);
		if ( range == 0 )
			range = v;
    }

	SA_initialize( &a, sizeof(long), compareLong, 100 );
	
	if ( argc == 1 )
	{
		while ( (cin >> c) && (cin >> v) )
		{
			switch( c )
			{
			case 'a':
				SA_insert( &a, &v ); break;
			
			case 'f':
				if ( SA_find( &a, &v ) == 0 ) cout << "Not found" << endl; else cout << "found: " << endl;
				break;

			case 'q':
				goto end;
			}
			
			SA_print( &a, printLong );
		}
	}
	else
	{
		srandom(seed);

		printf("running auto test (%ld iterations) w/ seed %u...\n",v,seed);
		
		hash_set<long> in_use;

		long i;

		for ( i = 0; i < v; ++i )
		{
			long x = random() % range;
			SA_insert( &a, &x );
			in_use.insert(x);
			icnt++;
		}

		printf("Inserts done.\n");

		if ( i == v )
		{
			printf("Testing finds... %d %d\n", a.usedEntries, in_use.size());

			for ( i = 0; i < range; ++i )
			{
				void * found = SA_find( &a, &i );

				if ( found != 0 && in_use.find(i) == in_use.end() )
				{
					printf("Found %ld, shouldn't have\n",i);
					break;
				}

				if ( found == 0 && in_use.find(i) != in_use.end() )
				{
					printf("Didn't find %ld, should have\n",i);
					break;
				}

				if ( found )
				{
					long val = *(long *)found;
					if ( val != i )
					{
						printf("Item found but wrong: %ld %ld\n",val,i);
						break;
					}
				}
			}

			if ( i != range )
			{
				printf("test failed\n");
			}
			else
				printf("test successful\n");
		}
	}
	
end:

	printf( "%d inserts, %d deletes (%d in tree) completed\n",icnt,dcnt,sdcnt);

	SA_print( &a, printLong );

	SA_delete( &a );
}
