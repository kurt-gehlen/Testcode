/*
 * seg_array.h
 *
 *  Created on: Mar 6, 2012
 *      Author: kurt
 */

#ifndef SEG_ARRAY_H_
#define SEG_ARRAY_H_

#define _SEG_ARRAY_MAX_SEGMENTS 16

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SegArray
{
	unsigned int	mask;
	unsigned int	shift;
	int				numSegments;
	int				objectSize;
	unsigned int	numObjects;
	void *			segments[_SEG_ARRAY_MAX_SEGMENTS];
} SegArray;


static inline void * SA_atIndex( SegArray * sa, unsigned int index )
{
	if( index > sa->numObjects )
		return 0;

	return (unsigned char *)sa->segments[index >> sa->shift] + (sa->objectSize * (index & sa->mask));
}


static inline int isPowerOf2( unsigned int v )
{
	return v && !(v & (v - 1));
}

static inline int logBase2( unsigned int v )
{
	int r = 0;
	while ( v >>= 1 )
		r++;

	return r;
}


static inline int
SA_init( SegArray * sa, int numObjects, int numSegments, int objectSize )
{
	if ( !isPowerOf2(numSegments) || !isPowerOf2(numObjects) )
		return -1;

	int perSegment = (numObjects / numSegments);
	int segmentSize = perSegment * objectSize;
	int i;
	for	( i = 0; i < numSegments; ++i )
	{
		if ( !(sa->segments[i] = malloc( segmentSize )) )
			break;
	}

	if ( i == numSegments )
	{
		sa->objectSize = objectSize;
		sa->numSegments = numSegments;
		sa->numObjects = perSegment * numSegments;
		sa->mask = perSegment - 1;
		sa->shift = logBase2( sa->mask ) + 1;

		return 0;
	}

	for ( --i; i >= 0; --i )
		free( sa->segments[i] );

	return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* SEG_ARRAY_H_ */
