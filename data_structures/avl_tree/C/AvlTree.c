
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
avl_ll_rebalance( Node ** node )
{
	Node * currentTop = *node;
	Node * leftChild = currentTop->left;

	currentTop->left  = leftChild->right;
	leftChild->right = currentTop;
	leftChild->parent = currentTop->parent;
	currentTop->parent = leftChild;

	currentTop->height = max(avl_height(currentTop->left), avl_height(currentTop->right)) + 1;
	leftChild->height = max(avl_height(leftChild->left), avl_height(leftChild->right)) + 1;

	*node = leftChild;
}


static int
avl_rr_rebalance( Node ** node )
{
	Node * currentTop = *node;
	Node * rightChild = currentTop->right;

	currentTop->right  = rightChild->left;
	rightChild->left = currentTop;
	rightChild->parent = currentTop->parent;
	currentTop->parent = rightChild;

	currentTop->height = max(avl_height(currentTop->left), avl_height(currentTop->right)) + 1;
	rightChild->height = max(avl_height(rightChild->left), avl_height(rightChild->right)) + 1;

	*node = rightChild;
}


static int
avl_lr_rebalance( Node ** node )
{
	avl_rr_rebalance( &(*node)->left );
	avl_ll_rebalance( node );
}


static int
avl_rl_rebalance( Node ** node )
{
	avl_ll_rebalance( &(*node)->right );
	avl_rr_rebalance( node );
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


int
avl_rebalance( Node ** node )
{
	int balance = avl_balance(*node);

	if ( balance < -1 )
	{
		if ( avl_balance((*node)->right) < 0  )
			avl_rr_rebalance( node );
		else
			avl_rl_rebalance( node );
	}
	else if ( balance > 1 )
	{
		if ( avl_balance((*node)->left) > 0  )
			avl_ll_rebalance( node );
		else
			avl_lr_rebalance( node );
	}

	Node * n = *node;
	n->height = 1 + max( avl_height(n->left), avl_height(n->right) );
}


Node *
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
		memcpy( n + 1, e, tree->itemSize );

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


Node *
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


Node *
avl_findMax( AvlTree * tree, Node * node )
{
	Node * foundNode = node;
	if ( node )
	{
		if ( node->right )
			foundNode = avl_findMax( tree, node->left );
	}

	return foundNode;
}


Node *
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


Node *
avl_remove( AvlTree * tree, Node ** node, void * e )
{
	Node * foundNode = avl_find( tree, *node, e );
	if ( foundNode )
	{
		Node * parent = 0;
		if ( foundNode->right || foundNode->left )
		{
			Node * newSubRoot;

			printf("Remove node w/ branches\n");
			if ( foundNode->right )
				newSubRoot = avl_findMin( tree, foundNode->right );
			else
				newSubRoot = avl_findMax( tree, foundNode->left );

			memcpy( foundNode + 1, newSubRoot + 1, tree->itemSize );

			foundNode = newSubRoot;
		}

		parent = foundNode->parent;

		if ( parent )
		{
			if ( foundNode == parent->left )
				parent->left = 0;
			else if ( foundNode == parent->right )
				parent->right = 0;
			else
				printf("Shouldn't have happend\n");


		}

		if ( foundNode == *node )
			*node = 0;

		free( foundNode );

		tree->count--;
	}
}


void
avl_print( Node * node, PrintFunc print )
{
	if ( node )
	{
		if ( node->left )
			avl_print( node->left, print );
		printf( "Node height: %d ", node->height );
		print( node + 1 );
		if ( node->right )
			avl_print( node->right, print );
	}
}


void
avl_delete( Node * node )
{
	if ( node )
	{
		if ( node->left )
			avl_delete( node->left );
		if ( node->right )
			avl_delete( node->right );

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
AVL_delete( AvlTree * tree )
{
	avl_delete( tree->root );

	tree->root = 0;
}


Node *
AVL_insert( AvlTree * tree, void * e )
{
	return avl_insert( tree, &tree->root, e );
}


Node *
AVL_remove( AvlTree * tree, void * e )
{
	return avl_remove( tree, &tree->root, e );
}


Node *
AVL_find( AvlTree * tree, void * e )
{
	return avl_find( tree, tree->root, e );
}


void
AVL_print( AvlTree * tree, PrintFunc print )
{
	printf("Tree w/ %d nodes\n",tree->count);
	avl_print( tree->root, print );
}


int
avl_validate( Node * node, PrintFunc print )
{
	int status = 0;
#if 0
	if ( node->left && node->left->parent != node )
	{
		printf("mismatched parent (left) %lx %lx\n", node->left->parent, node );
		status = -1;
	}

	if ( node->right && node->right->parent != node )
	{
		printf("mismatched parent (right) %lx %lx\n", node->right->parent, node );
		status = -1;
	}
#endif
	if ( avl_height(node) != max(avl_height(node->left),avl_height(node->right)) + 1 )
	{
		printf("Height mismatch %d %d %d\n", avl_height(node), avl_height(node->left), avl_height(node->right) );
		status = -1;
	}

	if ( abs(avl_balance(node)) > 1 )
	{
		printf("Balance mismatch %d\n", avl_balance(node) );
		status -1;
	}

	if ( node->left )
		if ( avl_validate(node->left,print) != 0 )
			status = -1;

	if ( node->right )
		if ( avl_validate(node->right,print) != 0 )
			status = -1;

	return status;
}

int
AVL_validate( AvlTree * tree, PrintFunc print )
{
	if ( tree->root )
		return avl_validate( tree->root, print );

	return 0;
}
