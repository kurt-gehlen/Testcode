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
SCH_init( SC_Hashtable * hashtable, int tablesize, int objsize, HASHFUNC hash, COMPFUNC comp, DELFUNC del )
{
	if ( (hashtable->table = (SC_Node **)malloc( tablesize * sizeof(int *) )) )
	{
		hashtable->tablesize	= tablesize;
		hashtable->objsize		= objsize;
		hashtable->count		= 0;
		hashtable->hash			= hash;
		hashtable->comp			= comp;
		hashtable->del			= del;

		int i;
		for ( i = 0; i < tablesize; ++i )
			hashtable->table[i] = 0;

		return 0;
	}

	return -1;
}


void
SCH_delete( SC_Hashtable * hashtable )
{
	int i;
	for ( i = 0; i < hashtable->tablesize; ++i )
	{
		int * node = hashtable->table[i];
		while ( *node != -1 )
		{
			int * next = CALC_PTR(*node);

			if( hashtable->del )
				hashtable->del( node + 1 );

			//free( node );

			node = next;
		}
	}

	if ( hashtable->table )
		hashtable->table;
}


void *
SCH_find( SC_Hashtable * hashtable, void * item )
{
	unsigned long hash_value = hashtable->hash( item );

	hash_value %= hashtable->tablesize;

	int node;
	int * pnode;
	int v;
	for	( node = hashtable->table[hash_value],pnode = CALC_PTR(node); node != -1 && (v = hashtable->comp(item,node + 1)) < 0; node = *pnode, pnode = CALC_PTR(node) )
		;

	if ( node != -1 && v == 0 )
		return pnode + 1;

	return 0;
}


void *
SCH_insert( SC_Hashtable * hashtable, void * item, int * found )
{
	unsigned long hash_value = hashtable->hash( item );

	hash_value %= hashtable->tablesize;

	int v;
	int * prev = &hashtable->table[hash_value];
	int * pnode;
	int node;

	for	( node = hashtable->table[hash_value],pnode = CALC_PTR(node); node != -1 && (v = hashtable->comp(item,node + 1)) < 0; node = *pnode, pnode = CALC_PTR(node) )
		prev = pnode;

	if ( node != -1 && v == 0 ) // item already in table;
	{
		if ( found )
			*found = 1;

		return node + 1;
	}

	int * newNode = OBTAIN_SLOT();
	if ( newNode )
	{
		memcpy( newNode + 1, item, hashtable->objsize );
		*newNode = node;
		*prev = CALC_INDEX(newNode);
		hashtable->count++;
	}

	return newNode ? newNode + 1 : 0;
}


int
SCH_remove( SC_Hashtable * hashtable, void * item, void * copy )
{
	unsigned long hash_value = hashtable->hash( item );

	hash_value %= hashtable->tablesize;

	int node;
	int * prev = &hashtable->table[hash_value];
	int * pnode;
	int v;
	for	( node = hashtable->table[hash_value],pnode = CALC_PTR(node); node != -1 && (v = hashtable->comp(item,node + 1)) < 0; node = *pnode, pnode = CALC_PTR(node) )
		prev = pnode;

	if ( !node || v != 0 ) // item not found
		return 0;

	*prev = CALC_PTR(*pnode);

	if ( copy )
		memcpy( copy, node + 1, hashtable->objsize );

	if ( hashtable->del )
		hashtable->del( pnode + 1 );

	//free( node );
	REMOVE_OBJECT(node);

	hashtable->count--;

	return 1;
}


void
SCH_iterate( SC_Hashtable * hashtable, OPFUNC op )
{
	int i, size = hashtable->tablesize;
	for	( i = 0; i < size; ++i )
	{
		SC_Node * node;
		for ( node = hashtable->table[i]; node; node = node->next )
			op( node + 1, i );
	}
}


void *
SCH_first( SC_Hashtable * hashtable, int * last )
{
	SC_Node * ret = 0;
	SC_Node ** table = hashtable->table;

	int i, size = hashtable->tablesize;
	for ( i = 0; i < size; ++i )
		if ( (ret = table[i]) )
			break;

	*last = i;

	return ret ? ret + 1 : 0;
}


void *
SCH_next( SC_Hashtable * hashtable, void * cur, int * last )
{
	SC_Node * ret = 0;
	SC_Node ** table = hashtable->table;
	SC_Node * node = (SC_Node *)cur - 1;

	if ( node->next )
		return node->next + 1;

	int i, size = hashtable->tablesize;
	for ( i = *last + 1; i < size; ++i )
		if ( (ret = table[i]) )
			break;

	*last = i;

	return ret ? ret + 1 : 0;
}
