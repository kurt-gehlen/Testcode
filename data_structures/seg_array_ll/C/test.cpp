
#include <stdio.h>
#include <stdlib.h>

#include "seg_array_list.h"

#include <iostream>
#include <string>

using namespace std;
using namespace __gnu_cxx;

struct TestStruct
{
	SAL_Node	node;
	long		val;
};

void walkList( SegArrayList * sal )
{
	for ( SAL_Node * p = (SAL_Node *)SA_atIndex ( sal->array, sal->headPtr->next ); p != sal->headPtr; p = (SAL_Node *)SA_atIndex( sal->array, p->next ) )
		cout << (void*)p << " next = " << p->next << " prev = " << p->prev << " val = " << ((TestStruct *)p)->val << endl;
}

int
main( int argc, char ** argv )
{
	SegArray sa;
	
	if ( SA_init( &sa, 0x10, 0x2, sizeof( TestStruct ) ) == 0 )
	{
		cout << "Array initialized " << (void *)sa.mask << " " << (void *)sa.shift << " " << sa.numObjects << endl;
		SegArrayList sal1, sal2;
		if ( SAL_init( &sal1, &sa, 0, 0 ) == 0 && SAL_init( &sal2, &sa, 1, 0 ) == 0 )
		{
			cout << "Lists initialized" << endl;

			for ( int i = 0; i < sa.numObjects - 2; ++i )
			{
				int index = i + 2;
				TestStruct * ts = (TestStruct *)SA_atIndex( &sa, index );
				if ( index % 3 )
				{
					ts->val = i;
					SAL_insertHead( &sal1, index );
				}
				else
				{
					ts->val = 0x1000000 - i;
					SAL_insertTail( &sal1, index );
				}
			}

			cout << "count = " << sal1.count << endl;
			walkList( &sal1 );

			TestStruct * ts;
			int index;
			while( (ts = (TestStruct *)SAL_removeHead( &sal1, &index )) )
			{
				cout << ts->node.prev << " " << ts->node.next << " " << ts->val << endl;
				SAL_insertHead( &sal2, index );
			}

			walkList( &sal2 );
		}
	}
	
}
