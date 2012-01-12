
#include "AvlTree.h"

#include <iostream>
#include <string>
#include <ext/hash_set>

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
	char c;
	
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
		v = atol(argv[1]);

		int r_factor = argc > 2 ? atol(argv[2]) : 0;

		printf("running auto test (%ld iterations)...\n",v);
		
		hash_set<long> in_use;

		long i;
		long range = v * 4;

		for ( i = 0; i < v; ++i )
		{
			long x = random() % range;
			if ( r_factor && (i%r_factor) == 0 )
			{
				AVL_remove(&a,&x);
				in_use.erase( x );
			}
			else
			{
				AVL_insert( &a, &x );
				in_use.insert(x);
			}

			if ( AVL_validate( &a, printLong ) )
			{
				printf("Invalid tree, stopping test\n");
				break;
			}
		}

		if ( i == v )
		{
			printf("Inserts good.  Testing finds... %d %d\n", a.count, in_use.size());

			for ( i = 0; i < range; ++i )
			{
				Node * found = AVL_find( &a, &i );

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
					long val = *(long *)(found + 1);
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

//		AVL_print( &a, printLong );
	}
	
end:

///	dumpNode( a.root, 5 );

	AVL_delete( &a );
}
