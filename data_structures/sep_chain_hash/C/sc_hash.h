/*
 * sc_hash.h
 *
 *  Created on: Feb 8, 2012
 *      Author: kurt
 */

#ifndef SC_HASH_H_
#define SC_HASH_H_

typedef struct SC_Node
{
	SC_Node *	next;
} SC_Node;


typedef unsigned long (*HASHFUNC)( void * );
typedef int (*COMPFUNC)( void *, void * );
typedef void (*DELFUNC)( void * );
typedef int (*OPFUNC)( void *, int );

typedef struct SC_Hashtable
{
	int			tablesize;
	int			objsize;
	int			count;
	SC_Node **	table;
	HASHFUNC	hash;
	COMPFUNC	comp;
	DELFUNC		del;
} SC_Hashtable;

extern int		SCH_init	( SC_Hashtable * hashtable, int tablesize, int objsize, HASHFUNC hash, COMPFUNC comp, DELFUNC del );
extern void		SCH_delete	( SC_Hashtable * hashtable );
extern void *	SCH_find	( SC_Hashtable * hashtable, void * item );
extern void *	SCH_insert	( SC_Hashtable * hashtable, void * item, int * found );
extern int		SCH_remove	( SC_Hashtable * hashtable, void * item, void * copy );
extern void		SCH_iterate	( SC_Hashtable * hashtable, OPFUNC op );
extern void *	SCH_first	( SC_Hashtable * hashtable, int * last );
extern void *	SCH_next	( SC_Hashtable * hashtable, void * cur, int * last );

#endif /* SC_HASH_H_ */
