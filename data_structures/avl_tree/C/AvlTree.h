#ifndef __AVL_TREE__H__
#define __AVL_TREE__H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Node
{
	int				height;
	struct Node *	parent;
	struct Node *	left;
	struct Node *	right;
} Node;


typedef int (*CompFunc)(void *, void *);
typedef void (*PrintFunc)(void *);
typedef int (*OpFunc)( void *, void * );

typedef struct AvlTree
{
	struct Node *	root;
	int				count;
	int				itemSize;
	CompFunc		compare;
} AvlTree;


int		AVL_initialize( AvlTree *, int itemSize, CompFunc comp );
void *	AVL_insert( AvlTree *, void * );
void *	AVL_remove( AvlTree *, void * );
void *	AVL_find( AvlTree*, void * );
void	AVL_print( AvlTree *,  PrintFunc );
void	AVL_delete( AvlTree *, OpFunc del );
int		AVL_validate( AvlTree *, CompFunc, PrintFunc );
int		AVL_iterate( AvlTree *, OpFunc, void * );
void *	AVL_first( AvlTree * );
void *	AVL_next( AvlTree *, void * );

#ifdef __cplusplus
}
#endif

#endif //__AVL_TREE__H__
