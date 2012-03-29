
#include "sc_hash.h"
#include "seg_array_list.h"

#include <iostream>
#include <string>
#include <ext/hash_set>
#include <map>
#include <getopt.h>

#include <sys/times.h>

using namespace std;
using namespace __gnu_cxx;

#define offset_of(x,y) (unsigned long)(&((x*)0)->y)

#define    CRC64_ECMA_182 0xC96C5795D7870F42ULL // ECMA-182 standard crc polynomial
uint64_t crc64Table[256];

struct TestStruct
{
	long		key;
	long		data;
};

struct HashNode
{
	int next;
	TestStruct data;
	SAL_Node	node;
} __attribute__((__packed__));
int compareTest( TestStruct * a, TestStruct * b )
{
	return b->key - a->key;
}

static uint64_t
modonly( unsigned char * buffer )
{
	return *(unsigned long *)buffer;
}


static uint64_t
crc64Hash( unsigned char * buffer )
{
	buffer += offset_of(TestStruct,key);

    uint64_t crc64 = -1ULL;

    // translate key to a uniform distribution via a standardized CRC function with uniform output
    for ( int i = 0; i < sizeof(long); i++ )
    	crc64 = (crc64 >> 8) ^ crc64Table[(crc64 ^ buffer[i]) & 0xFF];

    return crc64;
}

int doNothing( void * a, int index )
{
	return 0;
}

int printLong( void * a, int index )
{
	if ( !a )
		printf("What???\n");
	TestStruct * ts = (TestStruct *)a;
	printf("%10i %ld %ld\n",index,ts->key,ts->data);
	return 0;
}

static SegArrayList l_sal;
static int l_NodeWordOffset = 0;
int
getObject()
{
	int index;
	void * ts = SAL_removeHead( &l_sal, &index );
	if ( ts )
	{
		return index - l_NodeWordOffset;
	}
	return -1;
}


void
returnObject( int index )
{
	SAL_insertTail( &l_sal, index + l_NodeWordOffset );
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
	int numobjects = 0x10000;
	bool synopsis = false;
	HASHFUNC hash = (HASHFUNC)crc64Hash;
	long tps = sysconf(_SC_CLK_TCK);

	char c;
    while ( (c = getopt(argc,argv,"a:m:n:s:d:r:evipt:ch")) != -1 )
    {
        switch(c)
        {
        case 'h':
        {
        	hash = (HASHFUNC)modonly;
        }break;

			case 'c':
			{
				synopsis = true;
			} break;

			case 'a':
			{
				numobjects = atoi(optarg);

			} break;

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
                return -1;
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

    int wordsPerObject = sizeof(HashNode)/sizeof(int);
    int numwords = wordsPerObject * numobjects;

    if ( !isPowerOf2(numwords) )
    {
    	numwords--;
    	numwords |= numwords >> 1;
    	numwords |= numwords >> 2;
    	numwords |= numwords >> 4;
    	numwords |= numwords >> 8;
    	numwords |= numwords >> 16;
    	numwords++;
    }

    SegArray sa;
    if ( SA_init( &sa, numwords, 0x10, 4 ) == -1 )
    	printf("Crap\n");
#if 0
SA_init( &sa, numobjects, 0x10, sizeof( HashNode ) );
SAL_init( &l_sal, &sa, 0, offset_of(HashNode,node) );
for ( int i = 1; i < sa.numObjects - 1; ++i )
	SAL_insertHead( &l_sal, i );
#else
    numobjects = numwords / wordsPerObject;
    printf("Using %d objects, %d(%x) words, %d words per object\n",numobjects,numwords,numwords,wordsPerObject);
    int listNodeWordOffset = offset_of(HashNode,node)/sizeof(int);
    SAL_init( &l_sal, &sa, listNodeWordOffset, 0 );
    l_NodeWordOffset = listNodeWordOffset;
	for ( int i = listNodeWordOffset + wordsPerObject; i < sa.numObjects - wordsPerObject; i += wordsPerObject )
		SAL_insertHead( &l_sal, i );
#endif
    SCH_init( &a, tablesize, sizeof(TestStruct), &sa, hash, (COMPFUNC)compareTest, getObject, returnObject, 0 );
	int save;

	if ( !v )
	{
		while ( (cin >> c) && (cin >> v) )
		{
			save = 0;

			TestStruct ts;
			ts.key = v;

			switch( c )
			{
			case 'a':
				if ( SCH_insert( &a, &ts, &save ) ) cout << "inserted " << save << endl; else cout << "insert failed" << endl; break;
			
			case 'd':
			case 'r':
				if ( SCH_remove( &a, &ts, 0 ) ) cout << "removed" << endl; else cout << "not removed" << endl; break;

			case 'f':
				if ( SCH_find( &a, &ts ) == 0 ) cout << "Not found" << endl; else cout << "found: " << endl;
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

		printf( "Running performance test w/ %d nodes (%d tps)...\n", range, tps );

		int failed = 0;
		timespec ts1,ts2;
		t1 = times(&tms1);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&ts1);
		for ( long i = 0; i < v; ++i )
		{
			TestStruct ts;
			ts.key = i;
			if ( !SCH_insert( &a, &ts, &save ) )
				failed++;
		}
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&ts2);
		t2 = times(&tms2);

		delta = t2 - t1;
		printf( "Inserts took %d ticks (%d per 10^9) (%d failed)\n", delta, delta * 1000000000/(i_repeat * v * tps), failed );
		unsigned long start = ts1.tv_sec * 1000000000 + ts1.tv_nsec;
		unsigned long finish = ts2.tv_sec * 1000000000 + ts2.tv_nsec;
		unsigned long deltaNsec = finish - start;
		printf( "using other  clock, took %ld nsecs, %ld per \n", deltaNsec, deltaNsec/v );

		if ( r_factor )
		{
			t1 = times(&tms1);
			for ( long i = 0; i < v; ++i )
				if ( (i % r_factor) == 0 )
				{
					TestStruct ts;
					ts.key = i;
					SCH_remove( &a, &ts, 0 );
				}
			t2 = times(&tms2);

			delta = t2 - t1;
			printf( "Removes took %d ticks (%d per 10^9)\n", delta, delta * 1000000000/((v/r_factor) * tps) );
		}

		t1 = times(&tms1);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&ts1);
		for ( int r = 0; r < f_repeat; ++r )
		{
			for ( long i = 0; i < v; ++i )
			{
				TestStruct ts;
				ts.key = i;
				SCH_find( &a, &ts );
			}
		}
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&ts2);
		t2 = times(&tms2);

		delta = t2 - t1;
		printf( "Finds took %d ticks (%d per 10^9)\n", delta, delta * 1000000000/(f_repeat * v * tps) );
		start = ts1.tv_sec * 1000000000 + ts1.tv_nsec;
		finish = ts2.tv_sec * 1000000000 + ts2.tv_nsec;
		deltaNsec = finish - start;
		printf( "using other  clock, took %ld nsecs, %ld per \n", deltaNsec, deltaNsec/(v*f_repeat) );
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

			TestStruct ts;
			ts.key = x;
			ts.data = i;

			if ( r_factor && (i%r_factor) == 0 )
			{
				bool tryRemove = true;
				long x2 = x;
				op = 1;

				if ( in_use.find(x) == in_use.end() )
				{
					op2 = 0;
					if ( !SCH_insert( &a, &ts, &save ) )
						tryRemove = false;
				}
				else
					sdcnt++;

				if ( tryRemove )
				{
					if ( !SCH_remove( &a, &ts, 0 ) )
					{
						printf("How could we not find that %ld %s?\n", x, in_use.find( x ) == in_use.end() ? "huh?" : "" );
						//break;
					}

					in_use.erase( x2 );
					dcnt++;
				}
			}
			else
			{
				save = 0;
				if ( SCH_insert( &a, &ts, &save ) && !save )
				{
					in_use.insert(x);
					icnt++;
				}
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
				TestStruct ts;
				ts.key = i;
				void * found = SCH_find( &a, &ts );

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
					long val = ((TestStruct *)found)->key;
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
			{
				unsigned long delta = end - start;
				printf("test successful: %d ticks (%d nsec)\n",delta,delta * 1000000000/(range * tps));
			}
		}
	}
	
end:

	printf("Table has %d entries (max chain %d)\n", a.count, a.maxdepth );
	printf( "%d inserts, %d deletes (%d already in table) completed\n",icnt,dcnt,sdcnt);

	if ( synopsis )
	{
		map<int,int> summary;
		int sum = 0;
		int i = 0;
		int last = 0;
		for ( void * itr = SCH_first( &a, &save ); itr; itr = SCH_next( &a, itr, &save ), i++ )
		{
			if ( last != save )
			{
				for ( last++; last < save; last++ )
				{
					summary[0]++;
				}

				summary[i]++;
				sum += i;
				i = 0;
			}

			last = save;
		}

		sum += i;
		summary[i]++;
		summary[0] += a.tablesize - save - 1;

		printf("Summary: %d %d\n",a.count,sum);
		sum = 0;
		for ( map<int,int>::iterator itr = summary.begin(); itr != summary.end(); ++itr )
		{
			sum += itr->second;
			printf("%3d: %d\n",itr->first,itr->second);
		}
		if ( sum != a.tablesize )
			printf("Size mismatch: %d %d\n",sum,a.tablesize);
	}

	if ( iterate )
	{
		SCH_iterate( &a, doNothing );
/*
		int i = 0;
		for ( void * itr = SCH_first( &a, &save ); itr; itr = SCH_next( &a, itr, &save ) )
		{
			printf( "%d: ", i++); printLong( itr, 0 ); printf("\n");
		}
*/
	}

	if ( !quiet )
	{
		SCH_iterate( &a, printLong );
	}

	SCH_delete( &a );
}
