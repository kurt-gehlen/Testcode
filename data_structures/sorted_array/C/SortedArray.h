#ifndef __SORTED_ARRAY__H__
#define __SORTED_ARRAY__H__

#ifdef __cplusplus
extern "C"
{
#endif


typedef int (*CompFunc)(void *, void *);
typedef void (*PrintFunc)(void *);


typedef struct SortedArray
{
	int				arrayEntries;
	int				usedEntries;
	int				contentSize;
	unsigned char *	array;
	CompFunc		compare;
} SortedArray;


int		SA_initialize( SortedArray *, int contentSize, CompFunc comp, int startingSize );
void *	SA_insert( SortedArray *, void * obj );
void *	SA_find( SortedArray *, void * obj );
void	SA_delete( SortedArray * );
void	SA_print( SortedArray *, PrintFunc );



#ifdef __cplusplus
}
#endif

#endif // __SORTED_ARRAY__H__
