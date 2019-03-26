
/* Routines implementing a priority queue. There are two standard
 * operations. priq_merge() merges two priority queues. Addition
 * of a single element is a special case of the merge (second priority
 * queue consisting of a single element). Removing the smallest queue
 * element (always the head) is also implementing by merging the left
 * and right children. The second operation is removing a queue element
 * (not necessarily the head) and is implemented by priq_delete().
 *
 * This priority queue leans to the left. Following right pointers
 * always gets you to NULL is O(log(n) operations. Each queue element
 * has the following items: left pointer, right pointer, distance to NULL,
 * and the item's cost. The properties of the queue are:
 *
 * 	COST(a) <= COST(LEFT(a)) and COST(a) <= COST(RIGHT(a))
 * 	DIST(LEFT(a)) >= DIST(RIGHT(a))
 * 	DIST(a) = 1 + DIST(RIGHT(a))
 *
 * It's this last clause that makes the tree lean to the left.
 * Priority queues are covered in Section 5.2.3 of Knuth Vol. III.
 */

#include "machdep.h"
#include "priq.h"

/*
 * Function to swap nodes in binary heap to maintain ascending binary heap.
 */
void PriQ::node_swap(PriQElt *parent_node, PriQElt *child_node)
{
    PriQElt   *left = NULL, *right = NULL;

    left = child_node->left;
    right = child_node->right;
    child_node->parent = parent_node->parent;
    if(child_node->parent == NULL)
        root = child_node;
    else {
        if(child_node->parent->left == parent_node)
            child_node->parent->left = child_node;
        else if(child_node->parent->right == parent_node)
            child_node->parent->right = child_node;
    }
    if(parent_node->left == child_node) {
        child_node->left = parent_node;
        child_node->right = parent_node->right;
        if(child_node->right)
            child_node->right->parent = child_node;
    } else if(parent_node->right == child_node) {
        child_node->right = parent_node;
        child_node->left = parent_node->left;
        if(child_node->left)
            child_node->left->parent = child_node;
    }
    parent_node->left = left;
    if(left)
        left->parent = parent_node;
    parent_node->right = right;
    if(right)
        right->parent = parent_node;
    parent_node->parent = child_node;
    return;
}


/* Add an element to a priority queue.
 */

void PriQ::priq_add(PriQElt *new_node)

{
    int     path = 0;
    int     k = 0, n = 0;
    PriQElt    *child = NULL, *last_node = NULL;

	if(new_node == NULL)
		return;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;

    n = 1 + nelts;
    /*find the last node based on position*/
    for(;n >= 2;n /= 2,k++) {
        path = (path << 1) | (n & 1);
    }

    nelts++;
    child = root;
    if(!root) {
        /*First element, just return;*/
        root = new_node;
        return;
    }
    /*Traverse according to the path and find the last node*/
    while(--k > 0) {
        if(path & 1)
            child = child->right;
        else
            child = child->left;

        path = path >> 1;
    }

    last_node = child;
    /*Insert the new node at this position*/
    if(path & 1)
        last_node->right = new_node;
    else
        last_node->left = new_node;

    new_node->parent = last_node;

    /*Heapify by going up the ladder.*/
    for(;new_node->parent && new_node->costs_less(new_node->parent);) {
            node_swap(new_node->parent, new_node);
    }
}

/* Delete an item from the middle of the priority queue. Merge
 * the two branches leading from the deleted node, and then go back
 * up the tree, rebalancing when necessary.
 *
 * To enable going back up the tree, parent pointers have been added
 * to the priority queue items.
 */

void PriQ::priq_delete(PriQElt *new_node)

{
    int     path = 0;
    int     k = 0, n = 0;
    PriQElt   *child = NULL, *last_node = NULL, *parent = NULL, *smallest = NULL;

    if(nelts == 0)
        return;

    n = nelts;
    /*find the last node based on position*/
    for(;n >= 2;n /= 2,k++) {
        path = (path << 1) | (n & 1);
    }

    child = root;
    /*Traverse according to the path and find the last node*/
    while(k-- > 0) {
        if(path & 1)
            child = child->right;
        else
            child = child->left;

        path = path >> 1;
    }

    last_node = child;
    nelts--;

    /*Unlink the parent of last node.*/
    parent = last_node->parent;
    if(parent) {
        if(parent->left == last_node)
            parent->left = NULL;
        else
            parent->right = NULL;
    }

    if(last_node == new_node) {
        if(last_node == root)
            root = NULL;
        return;
    }

    /*Replace node to be deleted with last node.*/
    last_node->parent = new_node->parent;

    if(new_node->parent) {
        if(new_node->parent->left == new_node)
            new_node->parent->left = last_node;
        else if (new_node->parent->right == new_node)
            new_node->parent->right = last_node;
    } else
        root = last_node;

    /*In the following code, only exception is if new_node is parent of last_node, that case has to be taken care.*/
    last_node->left = new_node->left;
    last_node->right = new_node->right;
    if(last_node->left)
        last_node->left->parent = last_node;
    if(last_node->right)
        last_node->right->parent = last_node;

    /*Heapify*/
    child = last_node;


    /*Walk down the subtree*/
    for(;;) {
        smallest = child;
        if(child->left != NULL && child->left->costs_less(smallest))
            smallest = child->left;
        if(child->right != NULL && child->right->costs_less(smallest))
            smallest = child->right;

        if(smallest == child)
                break;
        node_swap(child, smallest);
    }

    /*Walk up the subtree*/
    while(child->parent != NULL && child->costs_less(child->parent))
        node_swap(child->parent, child);
}

/* Take the top element off the priority queue.
 * Maintain the priority queue structure by merging
 * the left and right halves of the tree.
 */

PriQElt *PriQ::priq_rmhead()

{
    PriQElt *top;

    if (!(top = root))
	return(0);

	priq_delete(top);
	return(top);
}

