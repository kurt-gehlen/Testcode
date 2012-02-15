
#include "sc_hash.h"

#include <iostream>
#include <string>
#include <ext/hash_set>

#include <getopt.h>

#include <sys/times.h>

using namespace std;
using namespace __gnu_cxx;

#define    CRC64_ECMA_182 0xC96C5795D7870F42ULL // ECMA-182 standard crc polynomial
uint64_t crc64Table[256];


int compareLong( long * a, void * b )
{
	return *(long *)b - *(long *)a;
}


static uint64_t
crc64Hash( unsigned char * buffer )
{
    uint64_t crc64 = -1ULL;

    // translate key to a uniform distribution via a standardized CRC function with uniform output
    for ( int i = 0; i < sizeof(long); i++ )
    	crc64 = (crc64 >> 8) ^ crc64Table[(crc64 ^ buffer[i]) & 0xFF];

    return crc64;
}


int printLong( void * a, int index )
{
	if ( !a )
		printf("What???\n");
	long i = *(long *)a;
	printf("%10i %ld\n",index,i);
	return 0;
}


int
main( int argc, char ** argv )
{
	SC_Hashtable a;
	long v = 0;
	int icnt = 0,dcnt = 0,sdcnt = 0;
	
	unsigned int seed = time(0);
	long range = 0;
	long r_factor = 0;
	bool stopOnError = false;
	bool quiet = true;
	bool iterate = false;
	bool performanceTest = false;
	int i_repeat = 1,f_repeat = 1;
	int tablesize = 100;

	char c;
    while ( (c = getopt(argc,argv,"m:n:s:d:r:evipt:")) != -1 )
    {
        switch(c)
        {
        	case 'n':
        	{
        		f_repeat = atoi(optarg);

        		if ( f_repeat < 1 )
        			printf("bad repeat value\n");

        	} break;

        	case 'm':
        	{
        		i_repeat = atoi(optarg);

        		if ( i_repeat < 1 )
        			printf("bad repeat value\n");

        	} break;

            case 's':
            {
                seed = atoi(optarg);

            } break;

            case 'd':
            {
                r_factor = atoi(optarg);

            } break;

            case 'r':
            {
                range = atoi(optarg);

            } break;

            case 'p':
            {
            	performanceTest = true;

            } break;

            case 'e':
            {
            	stopOnError = true;

            } break;

            case 'v':
            {
            	quiet = false;

            } break;

            case 'i':
            {
            	iterate = true;

            } break;

            case 't':
            {
            	tablesize = atoi(optarg);

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


    for (int i = 0; i < 256; i++)
    {
    	uint64_t *entry = crc64Table + i;
        *entry = i;
        int j;
        for (j = 8; j > 0; j--)
            *entry = (*entry >> 1) ^ (-(*entry & 1) & CRC64_ECMA_182);
    }

	SCH_init( &a, tablesize, sizeof(long), (HASHFUNC)crc64Hash, (COMPFUNC)compareLong, 0 );
	int save;

	if ( !v )
	{
		while ( (cin >> c) && (cin >> v) )
		{
			save = 0;

			switch( c )
			{
			case 'a':
				if ( SCH_insert( &a, &v, &save ) ) cout << "inserted " << save << endl; else cout << "insert failed" << endl; break;
			
			case 'd':
			case 'r':
				if ( SCH_remove( &a, &v, 0 ) ) cout << "removed" << endl; else cout << "not removed" << endl; break;

			case 'f':
				if ( SCH_find( &a, &v ) == 0 ) cout << "Not found" << endl; else cout << "found: " << endl;
				break;

			case 'q':
				goto end;
			}
		}
	}
	else if ( performanceTest )
	{
		struct tms tms1, tms2;
		clock_t t1, t2;
		unsigned long delta;

		long tps = sysconf(_SC_CLK_TCK);
		printf( "Running performance test w/ %d nodes (%d tps)...\n", range, tps );

		t1 = times(&tms1);
		for ( int r = 0; r < i_repeat; ++r )
			for ( long i = 0; i < v; ++i )
				SCH_insert( &a, &i, &save );
		t2 = times(&tms2);

		delta = t2 - t1;
		printf( "Inserts took %d ticks (%d per 10^9)\n", delta, delta * 1000000000/(i_repeat * v * tps) );

		if ( r_factor )
		{
			t1 = times(&tms1);
			for ( long i = 0; i < v; ++i )
				if ( (i % r_factor) == 0 )
					SCH_remove( &a, &i, 0 );
			t2 = times(&tms2);

			delta = t2 - t1;
			printf( "Removes took %d ticks (%d per 10^9)\n", delta, delta * 1000000000/(v * tps) );
		}

		t1 = times(&tms1);
		for ( int r = 0; r < f_repeat; ++r )
			for ( long i = 0; i < v; ++i )
				SCH_find( &a, &i );
		t2 = times(&tms2);

		delta = t2 - t1;
		printf( "Finds took %d ticks (%d per 10^9)\n", delta, delta * 1000000000/(f_repeat * v * tps) );
	}
	else
	{
		srandom(seed);

		printf("running auto test (%ld iterations) w/ seed %u, remove 1/%ld...\n",v,seed,r_factor);
		
		hash_set<long> in_use;

		long i;

		struct tms t1,t2;
		clock_t start = times(&t1);
		for ( i = 0; i < v; ++i )
		{
			long x = random() % range;
			int op = 0,op2 = 0;
			if ( r_factor && (i%r_factor) == 0 )
			{
				long x2 = x;
				op = 1;

				if ( in_use.find(x) == in_use.end() )
				{
					op2 = 0;
					SCH_insert( &a, &x, &save );
				}
				else
					sdcnt++;

				if ( !SCH_remove( &a, &x, 0 ) )
				{
					printf("How could we not find that %ld %s?\n", x, in_use.find( x ) == in_use.end() ? "huh?" : "" );
					//break;
				}

				in_use.erase( x2 );
				dcnt++;
			}
			else
			{
				SCH_insert( &a, &x, &save );
				in_use.insert(x);
				icnt++;
			}

			if ( 0 )
			{
				bool good = true;

				if ( in_use.size() != a.count )
				{
					printf("Size mismatch: %d %d\n",in_use.size(),a.count);
					good = false;
				}

				for ( hash_set<long>::iterator itr = in_use.begin(); itr != in_use.end(); ++itr )
				{
					long x = *itr;
					if ( SCH_find( &a, &x) == 0 )
					{
						printf("Should have %ld, didn't\n",x);
						good = false;
					}
				}

				if ( !good )
				{
					printf("Invalid table after iteration %d, performing %s%s of %ld, stopping test\n", i, (op2 ? "(auto)" : ""), (op == 0 ? "add":"remove"), x );
					break;
				}
			}
		}
		clock_t end = times(&t2);

		printf("Inserts done %d ticks.\n",end-start);

		int status = 0;
		if ( i == v )
		{
			printf("Testing finds... %d %d\n", a.count, in_use.size());

			struct tms t1, t2;
			clock_t start = times(&t1);
			for ( i = 0; i < range; ++i )
			{
				void * found = SCH_find( &a, &i );

				if ( found != 0 && in_use.find(i) == in_use.end() )
				{
					printf("Found %ld, shouldn't have\n",i);
					status = 1;//break;
				}

				if ( found == 0 && in_use.find(i) != in_use.end() )
				{
					printf("Didn't find %ld, should have\n",i);
					status = 1;//break;
				}

				if ( found )
				{
					long val = *(long *)(found);
					if ( val != i )
					{
						printf("Item found but wrong: %ld %ld\n",val,i);
						status = 1;//break;
					}
				}
			}
			clock_t end = times(&t2);

			if ( status != 0 )
			{
				printf("test failed\n");
			}
			else
				printf("test successful: %d ticks\n",end - start);
		}
	}
	
end:

	printf("Table has %d entries\n", a.count );
	printf( "%d inserts, %d deletes (%d already in table) completed\n",icnt,dcnt,sdcnt);

	if ( iterate )
	{
		int i = 0;
		for ( void * itr = SCH_first( &a, &save ); itr; itr = SCH_next( &a, itr, &save ) )
		{
			printf( "%d: ", i++); printLong( itr, 0 ); printf("\n");
		}
	}

	if ( !quiet )
	{
		SCH_iterate( &a, printLong );
	}

	SCH_delete( &a );
}
