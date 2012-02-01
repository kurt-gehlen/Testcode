
#include "AvlTree.h"

#include <iostream>
#include <string>
#include <ext/hash_set>

#include <getopt.h>

using namespace std;
using namespace __gnu_cxx;

int jim = 44;


int collectLessThan( void * a, void * b )
{
	hash_set<long> * hs = (hash_set<long> *)b;

	long l = *(long *)a;

	if ( l > jim )
	{
		printf ("stopping at %ld\n",l);
		return -1;
	}

	hs->insert(l);

	return 0;
}

int compareLong( void * a, void * b )
{
/*
	long i = *(long *)a;
	long j = *(long *)b;
	
	if ( i < j )
		return -1;
	else if ( i > j )
		return 1;
	
	return 0;
*/
	return *(unsigned long *)a - *(unsigned long *)b;
}

void printLong( void * a )
{
	if ( !a )
		printf("What???\n");
	long i = *(long *)a;
	//cout << i << endl;
	printf("%ld",i);
}


void dumpNode( Node * n, int level )
{
	for ( int i = 0; i < level; ++i )
		printf( "\t");
	printf( "%ld\n", *(long *)(n+ 1) );

	if ( n->left ) dumpNode( n->left, level - 1);
	if ( n->right ) dumpNode( n->right, level + 1);
}

int
main( int argc, char ** argv )
{
	AvlTree a;
	long v;
	int icnt = 0,dcnt = 0,sdcnt = 0;
	
	unsigned int seed = time(0);
	long range = 0;
	long r_factor = 0;
	bool stopOnError = false;
	bool quiet = true;
	bool iterate = false;

	char c;
    while ( (c = getopt(argc,argv,"s:d:r:evi")) != -1 )
    {
        switch(c)
        {
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
            default:
                printf("bad option: %c\n",c);
                return -1;;
        }
    }

    if ( optind >= argc )
    {
    	printf("usage:\n");
    	return -1;
    }

    v = atoi(argv[optind]);
    if ( range == 0 )
    	range = v;

	AVL_initialize( &a, sizeof(long), compareLong );
	
	if ( argc == 1 )
	{
		while ( (cin >> c) && (cin >> v) )
		{
			switch( c )
			{
			case 'a':
				AVL_insert( &a, &v ); break;
			
			case 'd':
			case 'r':
				AVL_remove( &a, &v ); break;

			case 'f':
				if ( AVL_find( &a, &v ) == 0 ) cout << "Not found" << endl; else cout << "found: " << endl;
				break;

			case 'q':
				goto end;
			}
			
			AVL_print( &a, printLong );
		}
	}
	else
	{
		srandom(seed);

		printf("running auto test (%ld iterations) w/ seed %u, remove 1/%ld...\n",v,seed,r_factor);
		
		hash_set<long> in_use;

		long i;

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
					AVL_insert(&a,&x);
				}
				else
					sdcnt++;

				if ( !AVL_remove(&a,&x) )
				{
					printf("How could we not find that %ld %s?\n", x, in_use.find( x ) == in_use.end() ? "huh?" : "" );
					//break;
				}

				in_use.erase( x2 );
				dcnt++;
			}
			else
			{
				AVL_insert( &a, &x );
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
					if ( AVL_find( &a, &x) == 0 )
					{
						printf("Should have %ld, didn't\n",x);
						good = false;
					}
				}

				if ( !good )
				{
					printf("Invalid tree after iteration %d, performing %s%s of %ld, stopping test\n", i, (op2 ? "(auto)" : ""), (op == 0 ? "add":"remove"), x );
					break;
				}
			}

			if ( stopOnError )
			{
				if ( AVL_validate( &a, compareLong, printLong ) )
				{
					printf("Invalid tree after iteration %d, performing %s%s of %ld, stopping test\n", i, (op2 ? "(auto)" : ""), (op == 0 ? "add":"remove"), x );
					goto end;
				}
			}
		}

		printf("Inserts done.\n");

		if ( AVL_validate( &a, compareLong, printLong ) )
		{
			printf("Tree not a valid avl tree\n");
		}
		else
		{
			int status = 0;
			if ( i == v )
			{
				printf("Testing finds... %d %d\n", a.count, in_use.size());

				for ( i = 0; i < range; ++i )
				{
					void * found = AVL_find( &a, &i );

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

				if ( status != 0 )
				{
					printf("test failed\n");
				}
				else
					printf("test successful\n");
			}
		}
	}
	
end:

	printf( "%d inserts, %d deletes (%d already in tree) completed\n",icnt,dcnt,sdcnt);

	AVL_print( &a, quiet ? 0 : printLong );

	if ( iterate )
	{
		int i = 0;
		for ( void * itr = AVL_first( &a ); itr; itr = AVL_next( &a, itr ) )
		{
			printf( "%d: ", i++); printLong( itr ); printf("\n");
		}
	}

	AVL_delete( &a, 0 );
}
