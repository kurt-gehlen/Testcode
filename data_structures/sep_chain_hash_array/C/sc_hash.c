/*
 * sc_hash.c
 *
 *  Created on: Feb 8, 2012
 *      Author: kurt
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sc_hash.h"


int
SCH_init( SC_Hashtable * ht, int tablesize, SegArray * array, HASHFUNC hash, COMPFUNC comp, ALLOCFUNC alloc, FREEFUNC dealloc, DELFUNC del )
{
	if ( (ht->table = (int *)malloc( tablesize * sizeof(int *) )) )
	{
		ht->tablesize	= tablesize;
		ht->array		= array;
		ht->objsize		= array->objectSize - sizeof(int);
		ht->count		= 0;
		ht->maxdepth	=  0;
		ht->hash		= hash;
		ht->comp		= comp;
		ht->del			= del;
		ht->alloc		= alloc;
		ht->dealloc		= dealloc;

		int i;
		for ( i = 0; i < tablesize; ++i )
			ht->table[i] = -1;

		return 0;
	}

	return -1;
}


void
SCH_delete( SC_Hashtable * ht )
{
	int i;
	for ( i = 0; i < ht->tablesize; ++i )
	{
		int index = ht->table[i];
		while ( index != -1 )
		{
			int * object = SA_atIndex( ht->array, index );

			if( ht->del )
				ht->del( object + 1 );

			index = *object;
		}
	}

	if ( ht->table )
		free(ht->table);
}


void *
SCH_find( SC_Hashtable * ht, void * item )
{
	unsigned long hash_value = ht->hash( item );

	hash_value %= ht->tablesize;

	int * node;
	int v;
	for	( node = SA_atIndex(ht->array,ht->table[hash_value]); node && (v = ht->comp(item,node + 1)) < 0; node = SA_atIndex(ht->array,*node) )
		;

	if ( node && v == 0 )
		return node + 1;

	return 0;
}


void *
SCH_insert( SC_Hashtable * ht, void * item, int * found )
{
	unsigned long hash_value = ht->hash( item );

	hash_value %= ht->tablesize;

	int depth = 0;
	int v;
	int * prev = &ht->table[hash_value];
	int * node;

	for	( node = SA_atIndex(ht->array,ht->table[hash_value]); node && (v = ht->comp(item,node + 1)) < 0; node = SA_atIndex(ht->array,*node), depth++ )
		prev = node;

	if ( depth > ht->maxdepth )
		ht->maxdepth = depth;

	if ( node && v == 0 ) // item already in table;
	{
		if ( found )
			*found = 1;

		return node + 1;
	}

	int newNodeIndex = ht->alloc();
	int * newObject = SA_atIndex( ht->array, newNodeIndex );
	if ( newObject )
	{
		memcpy( newObject + 1, item, ht->objsize );
		*newObject = *prev;
		*prev = newNodeIndex;
		ht->count++;
	}

	return newObject ? newObject + 1 : 0;
}


int
SCH_remove( SC_Hashtable * ht, void * item, void * copy )
{
	unsigned long hash_value = ht->hash( item );

	hash_value %= ht->tablesize;

	int v;
	int * prev = &ht->table[hash_value];
	int * node;

	for	( node = SA_atIndex(ht->array,ht->table[hash_value]); node && (v = ht->comp(item,node + 1)) < 0; node = SA_atIndex(ht->array,*node) )
		prev = node;


	if ( !node || v != 0 ) // item not found
		return 0;

	if ( copy )
		memcpy( copy, node + 1, ht->objsize );

	if ( ht->del )
		ht->del( node + 1 );

	int next = *node;

	ht->dealloc( *prev );

	*prev = next;

	ht->count--;

	return 1;
}


void
SCH_iterate( SC_Hashtable * ht, OPFUNC op )
{
	int i, size = ht->tablesize;
	for	( i = 0; i < size; ++i )
	{
		int * node;
		for	( node = SA_atIndex(ht->array,ht->table[i]); node; node = SA_atIndex(ht->array,*node) )
			op( node + 1, i );
	}
}


void *
SCH_first( SC_Hashtable * ht, int * last )
{
	int * ret = 0;
	int * table = ht->table;

	int i, size = ht->tablesize;
	for ( i = 0; i < size; ++i )
		if ( (ret = SA_atIndex(ht->array,table[i])) )
			break;

	*last = i;

	return ret ? ret + 1 : 0;
}


void *
SCH_next( SC_Hashtable * ht, void * cur, int * last )
{
	int * ret = 0;
	int * table = ht->table;
	int * node = (int *)cur - 1;

	int * next = SA_atIndex(ht->array,*node);
	if ( next )
		return next + 1;

	int i, size = ht->tablesize;
	for ( i = *last + 1; i < size; ++i )
		if ( (ret = SA_atIndex(ht->array,table[i])) )
			break;

	*last = i;

	return ret ? ret + 1 : 0;
}
