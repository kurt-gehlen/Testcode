/*
 * sc_hash.h
 *
 *  Created on: Feb 8, 2012
 *      Author: kurt
 */

#ifndef SC_HASH_H_
#define SC_HASH_H_

#include <seg_array.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long (*HASHFUNC)( void * );
typedef int (*COMPFUNC)( void *, void * );
typedef void (*DELFUNC)( void * );
typedef int (*OPFUNC)( void *, int );
typedef int (*ALLOCFUNC)();
typedef void (*FREEFUNC)(int);

typedef struct SC_Hashtable
{
	int			tablesize;
	int			objsize;
	int			count;
	int	*		table;
	SegArray *	array;
	int			maxdepth;
	HASHFUNC	hash;
	COMPFUNC	comp;
	DELFUNC		del;
	ALLOCFUNC	alloc;
	FREEFUNC	dealloc;
} SC_Hashtable;

extern int		SCH_init	( SC_Hashtable * hashtable, int tablesize, int objectsize, SegArray *, HASHFUNC hash, COMPFUNC comp, ALLOCFUNC, FREEFUNC, DELFUNC del );
extern void		SCH_delete	( SC_Hashtable * hashtable );
extern void *	SCH_find	( SC_Hashtable * hashtable, void * item );
extern void *	SCH_insert	( SC_Hashtable * hashtable, void * item, int * found );
extern int		SCH_remove	( SC_Hashtable * hashtable, void * item, void * copy );
extern void		SCH_iterate	( SC_Hashtable * hashtable, OPFUNC op );
extern void *	SCH_first	( SC_Hashtable * hashtable, int * last );
extern void *	SCH_next	( SC_Hashtable * hashtable, void * cur, int * last );

#ifdef __cplusplus
}
#endif

#endif /* SC_HASH_H_ */
