/*
 * seg_array_list.h
 *
 *  Created on: Feb 14, 2012
 *      Author: kurt
 */

#ifndef SEG_ARRAY_LIST_H_
#define SEG_ARRAY_LIST_H_

#include "seg_array.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct SAL_Node
{
	int	prev;
	int	next;
} SAL_Node;


#ifndef MAX_SEGMENTS
#define MAX_SEGMENTS 8
#endif

typedef struct SegArrayList
{
	SegArray *	array;
	int			count;
	int			offset;
	int			head;
	SAL_Node *	headPtr;
} SegArrayList;

extern int SAL_init( SegArrayList *, SegArray *, int head, int offset );
extern int SAL_insertHead( SegArrayList *, int index );
extern int SAL_insertTail( SegArrayList *, int index );
extern void * SAL_removeHead( SegArrayList *, int * );
extern void * SAL_removeTail( SegArrayList *, int * );
extern void * SAL_remove( SegArrayList *, int index );
extern void * SAL_removeNode( SegArrayList *, SAL_Node *, int * );

inline void * SAL_head( SegArrayList * sal, int * index )
{
	if ( sal->headPtr->next != sal->head )
	{
		if ( index )
			*index = sal->headPtr->next;
		return SA_atIndex( sal->array, sal->headPtr->next );
	}

	return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* SEG_ARRAY_LIST_H_ */
