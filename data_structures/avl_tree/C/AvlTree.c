
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "AvlTree.h"

#define max(x,y) (x > y ? x : y)

static inline int
avl_height( Node * n )
{
	return n ? n->height : -1;
}


static inline int
avl_balance( Node * n )
{
	return avl_height(n->left) - avl_height(n->right);
}


static int
avl_validate( Node * node, CompFunc comp, PrintFunc print, int verbose, int * c )
{
	int status = 0;

	(*c)++;

	if ( node->left && node->left->parent != node )
	{
		if ( verbose ) printf("mismatched parent (left) l = %lx lp = %lx n = %lx\n", node->left, node->left->parent, node );
		status = -1;
	}

	if ( node->right && node->right->parent != node )
	{
		if ( verbose ) printf("mismatched parent (right) r = %lx rp = %lx n = %lx\n", node->right, node->right->parent, node );
		status = -1;
	}

	if ( avl_height(node) != max(avl_height(node->left),avl_height(node->right)) + 1 )
	{
		if ( verbose ) printf("Height mismatch %d %d %d\n", avl_height(node), avl_height(node->left), avl_height(node->right) );
		status = -1;
	}

	if ( abs(avl_balance(node)) > 1 )
	{
		if ( verbose )
		{
			printf("Balance mismatch %d (%x) %d %lx(%d) %lx(%d)\n", avl_balance(node), node, node->height, node->left, avl_height(node->left), node->right, avl_height(node->right) );
			if ( print )
			{
				print( node + 1 );
				if ( node->left ) {	printf("\nleft "); print(node->left + 1); }
				if ( node->right ) { printf("\nright "); print(node->right + 1); }
				printf("\n");
			}
		}
		status = -1;
	}

	if ( node->left == 0 && node->right == 0 && node->height != 0 )
	{
		if ( verbose ) printf("Invalid height in leaf node: %d\n", node->height );
		status = -1;
	}

	if ( node->left )
	{
		if ( comp )
		{
			if ( comp(node->left + 1,node + 1) >= 0 )
			{
				if ( verbose ) { printf("sort failure (left)\n"); }
				status = -1;
			}
		}

		if ( avl_validate(node->left,comp,print,verbose,c) != 0 )
			status = -1;
	}

	if ( node->right )
	{
		if ( comp )
		{
			if ( comp(node->right + 1,node + 1) <= 0 )
			{
				if ( verbose ) printf("sort failure (right)\n");
				status = -1;
			}
		}

		if ( avl_validate(node->right, comp, print, verbose,c) != 0 )
			status = -1;
	}

	return status;
}


static inline int
avl_ll_rebalance( Node ** node )
{
	Node * currentTop = *node;
	Node * leftChild = currentTop->left;

	currentTop->left  = leftChild->right;
	if ( leftChild->right )
		leftChild->right->parent = currentTop;
	leftChild->right = currentTop;
	leftChild->parent = currentTop->parent;
	currentTop->parent = leftChild;

	currentTop->height = max(avl_height(currentTop->left), avl_height(currentTop->right)) + 1;
	leftChild->height = max(avl_height(leftChild->left), avl_height(leftChild->right)) + 1;

	*node = leftChild;
}


static inline int
avl_rr_rebalance( Node ** node )
{
	Node * currentTop = *node;
	Node * rightChild = currentTop->right;

	currentTop->right  = rightChild->left;
	if ( rightChild->left )
		rightChild->left->parent = currentTop;
	rightChild->left = currentTop;
	rightChild->parent = currentTop->parent;
	currentTop->parent = rightChild;

	currentTop->height = max(avl_height(currentTop->left), avl_height(currentTop->right)) + 1;
	rightChild->height = max(avl_height(rightChild->left), avl_height(rightChild->right)) + 1;

	*node = rightChild;
}


static inline int
avl_lr_rebalance( Node ** node )
{
	avl_rr_rebalance( &(*node)->left );
	avl_ll_rebalance( node );
}


static inline int
avl_rl_rebalance( Node ** node )
{
	avl_ll_rebalance( &(*node)->right );
	avl_rr_rebalance( node );
}


static inline int
avl_rebalance( Node ** node )
{
	int balance = avl_balance(*node);

	if ( balance < -1 )
	{
		if ( avl_balance((*node)->right) <= 0 )
			avl_rr_rebalance( node );
		else
			avl_rl_rebalance( node );
	}
	else if ( balance > 1 )
	{
		if ( avl_balance((*node)->left) >= 0 )
			avl_ll_rebalance( node );
		else
			avl_lr_rebalance( node );
	}

	Node * n = *node;
	n->height = 1 + max( avl_height(n->left), avl_height(n->right) );

	return abs(balance);
}


static Node *
avl_allocNode( AvlTree * tree, void * e )
{
	Node * newNode = (Node *)malloc( sizeof(Node) + tree->itemSize );

	if ( newNode )
	{
		newNode->parent = 0;
		newNode->left = 0;
		newNode->right = 0;
		newNode->height = 0;

		if ( e )
		{
			void * element = (void *)(newNode + 1);
			memcpy( element, e, tree->itemSize );
		}
	}

	return newNode;
}


#ifndef _NON_RECURSIVE_
static Node *
avl_insert( AvlTree * tree, Node ** node, void * e )
{
	if ( *node == 0 )
	{
		Node * n = avl_allocNode( tree, e );
		if ( !n )
		{
			printf("No space available for avl node\n");

			return 0;
		}

		*node = n;

		tree->count++;

		return n;
	}

	Node * n = *node;
	Node * r = 0;
	int comp = tree->compare( e, n + 1 );

	if ( comp == 0 )
	{
		// Its already here
//		memcpy( n + 1, e, tree->itemSize );
		r = n;
	}
	else
	{
		if ( comp < 0 )
		{
			r = avl_insert( tree, &(*node)->left, e );
		}
		else if ( comp > 0 )
		{
			r = avl_insert( tree, &(*node)->right, e );
		}

		if ( r )
		{
			if ( !r->parent )
				r->parent = *node;

			avl_rebalance( node );
		}
	}

	return r;
}
#else
static Node *
avl_insert( AvlTree * tree, Node ** node, void * e )
{
	Node * parent = 0;

	while( *node )
	{
		Node * n = *node;

		int comp = tree->compare( e, n + 1 );

		if ( comp == 0 )
		{
			// Its already here
			memcpy( n + 1, e, tree->itemSize );

			return n;
		}
		else
		{
			parent = n;

			if ( comp < 0 )
			{
				node = &n->left;
			}
			else if ( comp > 0 )
			{
				node = &n->right;
			}
		}
	}

	// Found space, create and insert
	Node * foundNode = avl_allocNode( tree, e );
	if ( foundNode )
	{
		*node = foundNode;
		foundNode->parent = parent;
		tree->count++;

		// Rebalance the tree

		while ( parent )
		{
			int  orig_height = parent->height;

			parent->height = max( avl_height(parent->left), avl_height(parent->right) ) + 1;

			Node ** gpc;

			Node * gp = parent->parent;

			if ( gp )
			{
				if ( parent == gp->right )
					gpc = &gp->right;
				else if ( parent == gp->left )
					gpc = &gp->left;
				else
				{
					printf("mismatched parents shouldn't happen\n");

					return 0;
				}
			}
			else
				gpc = &tree->root;

			avl_rebalance( gpc );

			parent = *gpc;

			if ( parent )
			{
				if ( orig_height == parent->height )
					break;

				parent = parent->parent;
			}
		}
	}

	return foundNode;
}
#endif

#ifndef _NON_RECURSIVE_
static Node *
avl_findMin( AvlTree * tree, Node * node )
{
	Node * foundNode = node;
	if ( node )
	{
		if ( node->left )
			foundNode = avl_findMin( tree, node->left );
	}

	return foundNode;
}
#else
static Node *
avl_findMin( AvlTree * tree, Node * node )
{
	while ( node )
	{
		if ( node->left )
			node = node->left;
		else
			break;
	}

	return node;
}
#endif

#ifndef _NON_RECURSIVE_
static Node *
avl_findMax( AvlTree * tree, Node * node )
{
	Node * foundNode = node;
	if ( node )
	{
		if ( node->right )
			foundNode = avl_findMax( tree, node->right );
	}

	return foundNode;
}
#else
static Node *
avl_findMax( AvlTree * tree, Node * node )
{
	while ( node )
	{
		if ( node->right )
			node = node->right;
		else
			break;
	}

	return node;
}
#endif

#ifndef _NON_RECURSIVE_
static Node *
avl_find( AvlTree * tree, Node * node, void * e )
{
	Node * foundNode = 0;
	if ( node )
	{
		int comp = tree->compare( e, node + 1 );
		if ( comp < 0 )
			foundNode = avl_find( tree, node->left, e );
		else if ( comp > 0 )
			foundNode = avl_find( tree, node->right, e );
		else
			foundNode = node;
	}

	return foundNode;
}
#else
#warning "Using non-recursive find"
static Node *
avl_find( AvlTree * tree, Node * node, void * e )
{
	Node * foundNode = 0;
	while ( node )
	{
		int comp = tree->compare( e, node + 1 );
		if ( comp < 0 )
			node = node->left;
		else if ( comp > 0 )
			node = node->right;
		else
		{
			foundNode = node;
			break;
		}
	}

	return foundNode;
}
#endif


static Node *
avl_remove( AvlTree * tree, Node ** node, void * e )
{
	Node * foundNode = avl_find( tree, *node, e );
	if ( foundNode )
	{
		Node * parent = 0;

		if ( foundNode->right || foundNode->left )
		{
			Node * newSubRoot;

			if ( foundNode->right )
				newSubRoot = avl_findMin( tree, foundNode->right );
			else
				newSubRoot = avl_findMax( tree, foundNode->left );

			memcpy( foundNode + 1, newSubRoot + 1, tree->itemSize );

			foundNode = newSubRoot;
		}

		parent = foundNode->parent;

		if ( foundNode->left && foundNode->right )
			printf("This node shouldn't be removed\n");

		if ( parent )
		{
			if ( foundNode == parent->left )
			{
				if ( foundNode->right )
					parent->left = foundNode->right;
				else
					parent->left = foundNode->left;

				if ( parent->left )
					parent->left->parent = parent;
			}
			else if ( foundNode == parent->right )
			{
				if( foundNode->right )
					parent->right = foundNode->right;
				else
					parent->right = foundNode->left;

				if ( parent->right )
					parent->right->parent = parent;
			}
			else
				printf("Shouldn't have happend\n");

			while ( parent )
			{
				int  orig_height = parent->height;

				parent->height = max( avl_height(parent->left), avl_height(parent->right) ) + 1;

				Node ** gpc;

				Node * gp = parent->parent;

				if ( gp )
				{
					if ( parent == gp->right )
						gpc = &gp->right;
					else if ( parent == gp->left )
						gpc = &gp->left;
					else
					{
						printf("mismatched parents shouldn't happen\n");

						return 0;
					}
				}
				else
					gpc = &tree->root;

				avl_rebalance( gpc );

				parent = *gpc;

				if ( parent )
				{
					if ( orig_height == parent->height )
						break;

					parent = parent->parent;
				}
			}
		}

		if ( foundNode == *node )
			*node = 0;

		free( foundNode );

		tree->count--;

		return (Node *)1;
	}

	return 0;
}


static int
avl_iterate( AvlTree * tree, Node * node, OpFunc op, void * param )
{
	if( node->left )
	{
		if ( avl_iterate( tree, node->left, op, param ) == -1 )
			return -1;
	}

	if ( op( node + 1, param ) == -1 )
		return -1;

	if ( node->right )
	{
		if ( avl_iterate( tree, node->right, op, param ) == -1 )
			return -1;
	}

	return 0;
}


static Node *
avl_next( AvlTree * tree, Node * node )
{
	if ( node->right )
		return avl_findMin( tree, node->right );

	if ( node->parent )
	{
		for ( ; node->parent; node = node->parent )
		{
			if ( node == node->parent->left )
				break;
		}

		return node->parent;
	}

	return 0;
}


static void
avl_print( Node * node, PrintFunc print )
{
	if ( node )
	{
		if ( node->left )
			avl_print( node->left, print );
		printf( "Node height: %d ", node->height );
		print( node + 1 );
		printf( "\n" );
		if ( node->right )
			avl_print( node->right, print );
	}
}


static void
avl_delete( Node * node, OpFunc del )
{
	if ( node )
	{
		if ( node->left )
			avl_delete( node->left, del );
		if ( node->right, del )
			avl_delete( node->right, del );

		if ( del )
			del( node + 1, 0 );

		free( node );
	}
}


int
AVL_initialize( AvlTree * tree, int itemSize, CompFunc comp )
{
	tree->root = 0;
	tree->count = 0;
	tree->itemSize = itemSize;
	tree->compare = comp;
}


void
AVL_delete( AvlTree * tree, OpFunc del )
{
	avl_delete( tree->root, del );

	tree->root = 0;
}


void *
AVL_insert( AvlTree * tree, void * e )
{
	Node * n = avl_insert( tree, &tree->root, e );

	return n ? n + 1 : 0;
}


void *
AVL_remove( AvlTree * tree, void * e )
{
	return avl_remove( tree, &tree->root, e );
}


void *
AVL_find( AvlTree * tree, void * e )
{
	Node * n = avl_find( tree, tree->root, e );

	return n ? n + 1 : 0;
}


void
AVL_print( AvlTree * tree, PrintFunc print )
{
	printf("Tree w/ %d nodes of height %d\n",tree->count,avl_height(tree->root) );
	if ( print )
		avl_print( tree->root, print );
}


int
AVL_validate( AvlTree * tree, CompFunc comp, PrintFunc print )
{
	if ( tree->root )
	{
		int c = 0;
		int status = avl_validate( tree->root, comp, print, 1, &c );

		if ( tree->count != c )
		{
			printf("Mismatched node count %d %d\n", tree->count, c );
			status = -1;
		}

		return status;
	}

	return 0;
}


int
AVL_iterate( AvlTree * tree, OpFunc op, void * param )
{
	if ( tree->root )
		return avl_iterate( tree, tree->root, op, param );

	return 0;
}


void *
AVL_next( AvlTree * tree, void * current )
{
	Node * node = 0;

	if ( current )
	{
		node = (Node *)current - 1;

		node = avl_next( tree, node );
	}

	return node ? node + 1 : 0;
}


void *
AVL_first( AvlTree * tree )
{
	Node * node = 0;

	if ( tree->root )
	{
		node = avl_findMin( tree, tree->root );
	}

	return node ? node + 1 : 0;
}
