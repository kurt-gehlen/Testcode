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

typedef struct AvlTree
{
	struct Node *	root;
	int				count;
	int				itemSize;
	CompFunc		compare;
} AvlTree;


int		AVL_initialize( AvlTree *, int itemSize, CompFunc comp );
Node *	AVL_insert( AvlTree *, void * );
Node *	AVL_remove( AvlTree *, void * );
Node *	AVL_find( AvlTree*, void * );
void	AVL_print( AvlTree *,  PrintFunc );
void	AVL_delete( AvlTree * );
int		AVL_validate( AvlTree *, PrintFunc );


#ifdef __cplusplus
}
#endif

#endif //__AVL_TREE__H__
