
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SortedArray.h"




static int
sa_find( unsigned char * array, int contentSize, int arraySize, CompFunc compare, void * toFind, unsigned char ** result )
{
	*result = array;

	int low = 0, high = arraySize - 1;
	while ( low <= high )
	{
		int index = (low + high)/2;

		unsigned char * p = array + (index * contentSize);
		*result = p;

		int comp = compare( toFind, p );
		if ( comp < 0 )
		{
			high = index - 1;
		}
		else if ( comp > 0 )
		{
			low = index + 1;
			*result = p + contentSize;
		}
		else
		{
			return 1; // found
		}
	}

	return 0;
}


int
SA_initialize( SortedArray * sa, int contentSize, CompFunc compare, int startingSize )
{
	sa->arrayEntries = startingSize;
	sa->usedEntries = 0;
	sa->contentSize = contentSize;
	sa->compare = compare;

	sa->array = (unsigned char *)malloc( sa->arrayEntries * contentSize);

	return sa->array == 0 ? -1 : 0;
}


void *
SA_insert( SortedArray * sa, void * obj )
{
	unsigned char * spot = 0;
	if ( sa_find( sa->array, sa->contentSize, sa->usedEntries, sa->compare, obj, &spot ) )
	{
		memcpy( spot, obj, sa->contentSize );
	}
	else
	{
		if ( sa->usedEntries == sa->arrayEntries )
		{
			printf("Allocating new memory\n");

			void * tmp = malloc( sa->arrayEntries * sa->contentSize * 2 );
			if ( tmp )
			{
				int spotOffset = spot - sa->array;

				memcpy( tmp, sa->array, sa->arrayEntries * sa->contentSize );
				free( sa->array );
				sa->array = (unsigned char *)tmp;
				sa->arrayEntries *= 2;

				spot = sa->array + spotOffset;
			}
			else
				return 0;
		}

		unsigned char * p = sa->array + (sa->contentSize * sa->usedEntries );
		while ( p > spot )
		{
			memcpy( p, p - sa->contentSize, sa->contentSize );
			p -= sa->contentSize;
		}

		memcpy( spot, obj, sa->contentSize );

		sa->usedEntries++;
	}

	return spot;
}


void *
SA_remove( SortedArray * sa, void * obj )
{

}


void *
SA_find( SortedArray * sa, void * obj )
{
	unsigned char * result = 0;

	if ( sa_find( sa->array, sa->contentSize, sa->usedEntries, sa->compare, obj, &result ) )
		return result;

	return 0;
}


void
SA_delete( SortedArray * sa )
{
	if ( sa->array )
		free( sa->array );

	sa->array = 0;
}


void
SA_print( SortedArray * sa, PrintFunc print )
{
	printf("Sorted array of length %d, %d slots used\n", sa->arrayEntries, sa->usedEntries );

	if ( sa->array )
	{
		unsigned char * p = (unsigned char *)sa->array;
		int i = 0;
		for ( ; i < sa->usedEntries; ++i, p += sa->contentSize )
			print( p );
	}
}
