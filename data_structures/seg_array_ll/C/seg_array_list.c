/*
 * seg_array_list.c
 *
 *  Created on: Feb 14, 2012
 *      Author: kurt
 */

#include <stdlib.h>
#include <string.h>

#include "seg_array_list.h"
#include "seg_array.h"

static inline SAL_Node * sal_fromIndex( SegArrayList * sal, int index )
{
	return SA_atIndex( sal->array, index ) + sal->offset;
}

int
SAL_init( SegArrayList * sal, SegArray * sa, int head, int offset )
{
	memset( sal, sizeof(sal), 0 );

	sal->array = sa;

	sal->head = head;
	sal->offset = offset;
	sal->headPtr = sal_fromIndex( sal, head );
	sal->headPtr->next = sal->headPtr->prev = head;
	sal->count = 0;

	return 0;
}


int
SAL_insert( SegArrayList * sal, int toInsert, int toInsertAfter )
{
	SAL_Node * newNode = sal_fromIndex( sal, toInsert );
	SAL_Node * oldNode = sal_fromIndex( sal, toInsertAfter );
	if ( !newNode && !oldNode )
		return -1;

	SAL_Node * oldNext = sal_fromIndex( sal, oldNode->next );
	if ( !oldNext )
		return -1;

	newNode->next = oldNode->next;
	newNode->prev = toInsertAfter;

	oldNext->prev = toInsert;
	oldNode->next = toInsert;

	sal->count++;

	return 0;
}


int
SAL_insertHead( SegArrayList * sal, int index )
{
	return SAL_insert( sal, index, sal->head );
}


int
SAL_insertTail( SegArrayList * sal, int index )
{
	return SAL_insert( sal, index, sal->headPtr->prev );
}


void *
SAL_remove( SegArrayList * sal, int index )
{
	if ( sal->count == 0 )
		return 0;

	SAL_Node * node = sal_fromIndex( sal, index );
	if ( !node )
		return 0;

	SAL_Node * prev = sal_fromIndex( sal, node->prev );
	SAL_Node * next = sal_fromIndex( sal, node->next );
	if ( !(prev && next ) )
		return 0;

	prev->next = node->next;
	next->prev = node->prev;

	sal->count--;

	node->next = node->prev = -1;

	return (void*)node - sal->offset;
}


void *
SAL_removeNode( SegArrayList * sal, SAL_Node * node, int * index )
{
	SAL_Node * prev = sal_fromIndex( sal, node->prev );
	return SAL_remove( sal, prev->next );
}


void *
SAL_removeHead( SegArrayList * sal, int * index )
{
	if ( index )
		*index = sal->headPtr->next;
	return SAL_remove( sal, sal->headPtr->next );
}


void *
SAL_removeTail( SegArrayList * sal, int * index )
{
	if ( index )
		*index = sal->headPtr->prev;
	return SAL_remove( sal, sal->headPtr->prev );
}


