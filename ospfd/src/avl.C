
/* Routine implementing AVL trees.
 * These are handled by the AVLitem and AVLtree
 * classes.
 */

#include "machdep.h"
#include "stack.h"
#include "avl.h"
#include <stdio.h>

/* Constructor for an AVLitem, Initialize the values
 * of the keys, and set the pointers to NULL and the balance
 * factor to 0.
 */

AVLitem::AVLitem(uns32 a, uns32 b) : _index1(a), _index2(b)

{
    right = 0;
    left = 0;
    balance = 0;
    in_db = 0;
    refct = 0;
}

/* Destrustor does nothing, but defined here so that inherited
 * classes can have their destructors called when AVL item
 * is deleted.
 */

AVLitem::~AVLitem()

{
}

/* Find an item in the AVL tree, given its two keys (indexes).
 */

AVLitem *AVLtree::find(uns32 key1, uns32 key2)

{
    AVLitem *ptr;

    for (ptr = _root; ptr; ) {
	if (key1 > ptr->_index1)
	    ptr = ptr->right;
	else if (key1 < ptr->_index1)
	    ptr = ptr->left;
	else if (key2 > ptr->_index2)
	    ptr = ptr->right;
	else if (key2 < ptr->_index2)
	    ptr = ptr->left;
	else
	    break;
    }

    return(ptr);
}


/* Find the item in the AVL tree that immediately precedes the
 * given keys (indexes).
 */

AVLitem *AVLtree::previous(uns32 key1, uns32 key2)

{
    AVLitem *ptr;
    AVLitem *last_right;

    if (key1 == 0 && key2 == 0)
	return(0);
    else if (key2 != 0)
	key2--;
    else {
	key2 = 0xffffffffL;
	key1--;
    }

    last_right = 0;
    
    for (ptr = _root; ptr; ) {
	if ((key1 > ptr->_index1) ||
	    (key1 == ptr->_index1 && key2 > ptr->_index2)) {
	    last_right = ptr;
	    ptr = ptr->right;
	}
	else if ((key1 < ptr->_index1) ||
		 (key2 < ptr->_index2))
	    ptr = ptr->left;
	else
	    return(ptr);
    }

    return(last_right);
}


/* Clear the whole AVL tree, by walking the ordered link
 * list.
 */

void AVLtree::clear()

{
    AVLitem *ptr;
    AVLitem *next;

    for (ptr = sllhead; ptr; ptr = next) {
	next = ptr->sll;
	ptr->in_db = 0;
	ptr->chkref();
    }

    _root = 0;
    sllhead = 0;
    count = 0;
    instance++;
}

/* Add item to balanced tree. Search for place in tree, remembering
 * the balance point (the last place where the tree balance was
 * non-zero). Insert the item, and then readjust the balance factors
 * starting at the balance point. If the balance at the balance point is
 * now +2 or -2, must shift to get the tree back in balance. There
 * are four cases: simple left shift, double left shift, and the right
 * shift counterparts.
 */

void AVLtree::add(AVLitem *item)

{
#if 0
    AVLitem **parent_ptr;
    AVLitem **balance_ptr;
    AVLitem *ptr;
    uns32 index1;
    uns32 index2;
    AVLitem *sllprev;

    item->in_db = 1;
    index1 = item->_index1;
    index2 = item->_index2;
    balance_ptr = &_root;
    instance++;

    for (parent_ptr = &_root; (ptr = *parent_ptr); ) {
	// Remember balance point's parent
	if (ptr->balance != 0)
	    balance_ptr = parent_ptr;
	// Search for insertion point
	if (index1 > ptr->_index1)
	    parent_ptr = &ptr->right;
	else if (index1 < ptr->_index1)
	    parent_ptr = &ptr->left;
	else if (index2 > ptr->_index2)
	    parent_ptr = &ptr->right;
	else if (index2 < ptr->_index2)
	    parent_ptr = &ptr->left;
	else {
	    // Replace current entry
	    *parent_ptr = item;
	    item->right = ptr->right;
	    item->left = ptr->left;
	    item->balance = ptr->balance;
	    // Update ordered singly linked list
	    item->sll = ptr->sll;
	    if (!(sllprev = previous(index1, index2)))
		sllhead = item;
	    else
		sllprev->sll = item;
	    ptr->in_db = 0;
	    ptr->chkref();
	    return;
	}
    }

    // Insert into tree
    *parent_ptr = item;
    count++;

    // Readjust balance, from balance point on down
    for (ptr = *balance_ptr; ptr != item; ) {
	if (index1 > ptr->_index1) {
	    ptr->balance += 1;
	    ptr = ptr->right;
	}
	else if (index1 < ptr->_index1) {
	    ptr->balance -= 1;
	    ptr = ptr->left;
	}
	else if (index2 > ptr->_index2) {
	    ptr->balance += 1;
	    ptr = ptr->right;
	}
	else if (index2 < ptr->_index2) {
	    ptr->balance -= 1;
	    ptr = ptr->left;
	}
    }

    // If necessary, shift to return balance
    ptr = *balance_ptr;
    if (ptr->balance == 2) {
	if (ptr->right->balance == -1)
	    dbl_left_shift(balance_ptr);
	else
	    left_shift(balance_ptr);
    }
    else if (ptr->balance == -2) {
	if (ptr->left->balance == 1)
	    dbl_right_shift(balance_ptr);
	else
	    right_shift(balance_ptr);
    }

    // Update ordered singly linked list
    if (!(sllprev = previous(index1, index2))) {
	item->sll = sllhead;
	sllhead = item;
    }
    else {
	item->sll = sllprev->sll;
	sllprev->sll = item;
    }
#endif

	AVLitem* p=NULL,*ya=NULL,*fya=NULL,*fp=NULL,*s=NULL,*gs=NULL,*ffp=NULL;
    uns32 index1;
    uns32 index2;
    AVLitem *sllprev;

    item->in_db = 1;
    index1 = item->_index1;
    index2 = item->_index2;
    instance++;

	p = ya = _root;

	//Find the insert point
	while(p != NULL)
	{
#if 0	
		if(node->key == p->key) 
			return;
#endif
	
		if(p->balance != 0)
		{
			ya = p; 
			fya = fp;
		}
		ffp = fp;
		fp = p;
		if(index1 < p->_index1)
			p = p->left;
		else if(index1 > p->_index1)
			p = p->right;
		else if(index2 < p->_index2)
			p = p->left;
		else if(index2 > p->_index2)
			p = p->right;
		else {
			//printf("add(): replacing entry for index1:%x index2:%x\n", item->_index1,item->_index2);
			// Replace current entry
			if(ffp == NULL)
				_root = item;
			else {
				if(ffp->left == p)
					ffp->left = item;
				else if(ffp->right == p)
					ffp->right = item;
			}
			item->right = p->right;
			item->left = p->left;
			item->balance = p->balance;
			// Update ordered singly linked list
			item->sll = p->sll;
			if (!(sllprev = previous(index1, index2)))
			sllhead = item;
			else
			sllprev->sll = item;
			p->in_db = 0;
			p->chkref();
			return;
		}
	}

	if(!_root) {
		_root = item;
	    // Update ordered singly linked list
	    if (!(sllprev = previous(index1, index2))) {
		//printf("\nadd()[_root case:sllprev NULL]: previous(%x, %x)\n", index1, index2);
		item->sll = sllhead;
		sllhead = item;
	    }
	    else {
		//printf("\nadd()[_root case:sllprev not NULL]: previous(%x, %x)\n", index1, index2);
		item->sll = sllprev->sll;
		sllprev->sll = item;
    	}
		count++;
		return;
	}

	if(fp) {
		
		if(index1 < fp->_index1)
			fp->left = item;
		else if(index1 > fp->_index1)
			fp->right = item;
		else if(index2 < fp->_index2)
			fp->left = item;
		else if(index2 > fp->_index2)
			fp->right = item;		
	}
	count++;
	
	//Find the son of ya in the direction of imbalance
	if(index1 < ya->_index1) {
		s = ya->left;
	} else if(index1 > ya->_index1) {
		s = ya->right;
	} else if(index2 < ya->_index2) {
		s = ya->left;
	} else if(index2 > ya->_index2) {
		s = ya->right;
	}

	//Adjust the balances from ya(youngest ancestor) down the inserted node.
	p = ya;
	while(p != NULL)
	{
		if(p == item)
			break;
		if(index1 < p->_index1) {
			p->balance--;
			p = p->left;
		} else if(index1 > p->_index1) {
			p->balance++;
			p = p->right;
		} else if(index2 < p->_index2) {
			p->balance--;
			p = p->left;
		} else if(index2 > p->_index2) {
			p->balance++;
			p = p->right;
		}
	}


	//Check if tree is unbalanced and rotate accordingly.
	p = ya;
	if(p->balance == -2) {
			if(p->left->balance == 1)
				ya = shiftLeftThenRight(ya, s, s->right);
			else
				ya = shiftRight(ya, s);
	} else if(p->balance == 2) {
		if(p->right->balance == -1)
				ya = shiftRightThenLeft(ya, s, s->left);
			else
				ya = shiftLeft(ya, s);
	} 

	if(fya == NULL)
		_root = ya;
	else
		if(index1 < fya->_index1)
			fya->left = ya;
		else if(index1> fya->_index1)
			fya->right = ya;
		else if(index2 < fya->_index2)
			fya->left = ya;
		else if(index2 > fya->_index2)
			fya->right = ya;		
		
    // Update ordered singly linked list
    if (!(sllprev = previous(index1, index2))) {
	//printf("\nadd()[sllprev NULL]: previous(%x, %x)\n", index1, index2);
	item->sll = sllhead;
	sllhead = item;
    }
    else {
	//printf("\nadd()[sllprev not NULL]: previous(%x, %x)\n", index1, index2);
	item->sll = sllprev->sll;
	sllprev->sll = item;
    }
	return;
}


/* Remove item from balanced tree. First, reduce to the case where the
 * item being removed has a NULL right or left pointer. If this is not the
 * case, simply find the item that is jus prvious (in sorted order); this is
 * guaranteed to have a NULL right pointer. Then swap the entries before
 * deleting. After deleting, go up the stack adjusting the balance
 * when necessary.
 *
 * We can stop whenevr the balance factor ceases to change.
 */

void AVLtree::remove(AVLitem *item)

{
#if 0
    AVLitem **parent_ptr;
    AVLitem *ptr;
    uns32 index1;
    uns32 index2;
    Stack stack;
    AVLitem **item_parent;
    AVLitem **prev;
    AVLitem *sllprev;

    index1 = item->_index1;
    index2 = item->_index2;

    for (parent_ptr = &_root; (ptr = *parent_ptr); ) {
	// Have we found element to be deleted?
	if (ptr == item)
	    break;
	// Add to stack for later balancing
	stack.push((void *) parent_ptr);
	// Search for item
	if (index1 > ptr->_index1)
	    parent_ptr = &ptr->right;
	else if (index1 < ptr->_index1)
	    parent_ptr = &ptr->left;
	else if (index2 > ptr->_index2)
	    parent_ptr = &ptr->right;
	else if (index2 < ptr->_index2)
	    parent_ptr = &ptr->left;
    }

    // Deletion failed
    if (ptr != item)
	return;

    instance++;
    count--;
    // Update ordered singly linked list
    if (!(sllprev = previous(index1, index2)))
	sllhead = item->sll;
    else
	sllprev->sll = item->sll;

    // Remove item from btree
    if (!item->right)
	*parent_ptr = item->left;
    else if (!item->left)
	*parent_ptr = item->right;
    // If necessary, swap with previous to get NULL pointer
    else {
	item_parent = parent_ptr;
	stack.push((void *) parent_ptr);
	parent_ptr = &item->left;
	stack.mark();
	for (ptr = *parent_ptr; ptr->right; ptr = *parent_ptr) {
	    stack.push((void *) parent_ptr);
	    parent_ptr = &ptr->right;
	}
	// Remove item from btree
	*parent_ptr = ptr->left;
	// Swap item and previous
	*item_parent = ptr;
	ptr->right = item->right;
	ptr->left = item->left;
	ptr->balance = item->balance;
	// Replace stack element that was item
	if (parent_ptr == &item->left)
	    parent_ptr = &ptr->left;
	else
	    stack.replace_mark((void *) &ptr->left);
    }

    // Go back up stack, adjusting balance where necessary;
    for (; (prev = (AVLitem **) stack.pop()) != 0; parent_ptr = prev) {
	AVLitem *parent;
	parent = *prev;
	if (parent_ptr == &parent->left)
	    parent->balance++;
	else if (parent_ptr == &parent->right)
	    parent->balance--;
	// tree has shrunken if balance now zero
	if (parent->balance == 0)
	    continue;
	// If out-of-balance, shift
	// Continue only if tree has then shrunken
	else if (parent->balance == 2) {
	    if (parent->right->balance == -1)
		dbl_left_shift(prev);
	    else if (parent->right->balance == 1)
		left_shift(prev);
	    else {
		left_shift(prev);
		break;
	    }
	}
	else if (parent->balance == -2) {
	    if (parent->left->balance == 1)
		dbl_right_shift(prev);
	    else if (parent->left->balance == -1)
		right_shift(prev);
	    else {
		right_shift(prev);
		break;
	    }
	}
	else
	    break;
    }
#endif

	AVLitem* p=NULL, *q=NULL, *fp=NULL, *fq=NULL, *child=NULL, *parent=NULL;
	short imbal = 0;
	bool rotationDone = false;
    bool unbal2NodeWithRChild0Bal = false;
    bool unbal2NodeWithLChild0Bal = false;
    uns32 index1 = item->_index1;
    uns32 index2 = item->_index2;
	Stack stack;
	AVLitem *sllprev;
	
	
	p = _root;
	fp = NULL;
	parent = NULL;
	
	//Find the correct node holder for the key.
	while(p != NULL) {
		
		if(p == item)
			break;
		fp = p;
		stack.push((void *)p);
		if(index1 < p->_index1) {
			p = p->left;
		} else if(index1 > p->_index1) {
			p = p->right;
		} else 	if(index2 < p->_index2) {
			p = p->left;
		} else if(index2 > p->_index2) {
			p = p->right;
		}
	}

	if(!p || p != item)
		return;

    instance++;
    count--;
    // Update ordered singly linked list
    if (!(sllprev = previous(index1, index2)))
	sllhead = item->sll;
    else
	sllprev->sll = item->sll;
	
	if(!p->left && !p->right) {
		child = NULL;
		if(fp) {
			if(fp->left == p) {
				fp->left = NULL;
				imbal = 1;
			} else if(fp->right == p) {
				fp->right = NULL;
				imbal = -1;
			}
		}else {
            /* Special case of only one element: root. */
            _root = NULL;
       }		
	} else if(!p->right) {
		child = p->left;
		if(fp) {
			if(fp->left == p)
				fp->left = child;
			else if(fp->right == p)
				fp->right = child;
		}else {
            /* Special case of only two elements: root and left child. */
            _root = p->left; 
		}
	} else if(!p->left) {
		child = p->right;
		if(fp) {
			if(fp->left == p)
				fp->left = child;
			else if(fp->right == p)
				fp->right = child;
		}else {
            /* Special case of only two elements: root and left child. */
            _root = p->right; 
		}		
		
	} else {
		/* Find the previous no in inorder which will guarantee it's right ptr is NULL. Later on, node to be deleted can be exchanged with this previous node. */
		if(p->left->right != NULL) {
			stack.mark();
			stack.push((void *)p);
		} else {			
			stack.mark();
			stack.push((void *)p);
		}
		
		fq = p;
		q = p->left;

		for(;q->right;) {
			fq = q;
			stack.push((void *)fq);
			q = q->right;
		}
	
	
		/* Now exchange node to be deleted "p" with it's immediate previous node "q" */
#if 0
		printf("deleteNode->key:%d \n", p->key);
		printf("newNode->key:%d \n", q->key);
		if(fp)
			printf("fp->key:%d \n", fp->key);
		if(fq)
			printf("fq->key:%d \n", fq->key);
#endif
		
		if(fq == p) {
			/* Node to be swapped is left child of node to be deleted */
			child = q->left;
			imbal = 1;
		} else {
			child = q->left;
			imbal = -1;
		}
				
		/* Delete newNode q from it's current location and adjust pointers. */
		if(fq->left == q)
			fq->left = q->left;
		else if(fq->right == q)
			fq->right = q->left;
		
		//Swap entries.
		q->left = p->left;
		q->right = p->right;
		q->balance = p->balance;
		
		/* Adjust child pointer of parent of newly inserted node except root pointer. */
		if(fp) {
			if(fp->left == p)
				fp->left = q;
			else if(fp->right == p)
				fp->right = q;
		}
		
		stack.replace_mark((void *)q);

		/* If node to be replaced is _root itself, update _root pointer accordingly */
		if(_root == item)
			_root = q;
	}
	
	// Go back up stack, adjusting balance where necessary;
	for(;(parent = (AVLitem *)stack.pop()) != NULL;child = parent) {

		if(rotationDone) {
			//rotation case, we can keep any indication flag for rotation instead of long check.
			if(child->_index1 < parent->_index1)
				parent->left = child;
			else if(child->_index1 > parent->_index1)
				parent->right = child;
			else if(child->_index2 < parent->_index2)
				parent->left = child;
			else if(child->_index2 > parent->_index2)
				parent->right = child;
			
			
			rotationDone = false;
            if(unbal2NodeWithRChild0Bal || unbal2NodeWithLChild0Bal) {
                //printf("unbal2NodeWithRChild0Bal || unbal2NodeWithLChild0Bal case. \n");
                break;
            }
		}
		
		if(child == NULL) {
			parent->balance += imbal;	
		} else if(parent->left == child)
			parent->balance++;
		else if(parent->right == child)
			parent->balance--;
		
		if(parent->balance == 0)
			continue;
		
		//Check if tree is unbalanced and rotate accordingly.
		if(parent->balance == -2) {
			rotationDone = true;
			if(parent->left->balance == 1)
				parent = shiftLeftThenRight(parent, parent->left, parent->left->right);
			else if(parent->left->balance == -1)
				parent = shiftRight(parent, parent->left);
            else {
				parent = shiftRight(parent, parent->left);
                unbal2NodeWithLChild0Bal = true;
            }
		} else if(parent->balance == 2) {
			rotationDone = true;
			if(parent->right->balance == -1)
				parent = shiftRightThenLeft(parent, parent->right, parent->right->left);
			else if(parent->right->balance == 1)
				parent = shiftLeft(parent, parent->right);
            else {
				parent = shiftLeft(parent, parent->right);
                unbal2NodeWithRChild0Bal = true;
            }
		} else
			break;
	}
	
	if(stack.is_empty()) {
		_root = parent;
	}
	
	if(parent == NULL)
		_root = child;

	item->in_db = 0;
	return;
}

AVLitem* leftRotate(AVLitem *ya) 
{
	AVLitem* p,*q;
	
	p = ya->right;
	q = p->left;
	p->left = ya;
	ya->right = q;

    return p;

}

AVLitem* rightRotate(AVLitem *ya)
{
	AVLitem* p,*q;
	
	p = ya->left;
	q = p->right;
	p->right = ya;
	ya->left = q;

    return p;

}
AVLitem* shiftLeft(AVLitem *ya, AVLitem *s)
{
	//Left rotate around ya(youngest ancestor)
	leftRotate(ya);

	//Update balances of ya and son of ya in the direction of imbalance.
	if(s->balance == 1) {
		s->balance = 0;
		ya->balance = 0;
	} else {
        s->balance = -1;
        ya->balance = 1;
    }
	return s;
}

AVLitem* shiftRight(AVLitem *ya, AVLitem *s)
{
	//Right rotate around ya(youngest ancestor)
	rightRotate(ya);
	
	//Update balances of ya and son of ya in the direction of imbalance.
	if(s->balance == -1) {
		s->balance = 0;
		ya->balance = 0;
	} else {
        s->balance = 1;
        ya->balance = -1;
    }
	return s;

}
AVLitem * shiftLeftThenRight(AVLitem *ya, AVLitem *s, AVLitem *gs)
{
	//Left rotate around ya(youngest ancestor)
	ya->left = leftRotate(s);
	rightRotate(ya);

	//Update balances of ya, son of ya, grandson of ya in the direction of imbalance.
	if(gs->balance == -1) {
		ya->balance = 1;
		s->balance = 0;
		gs->balance = 0;
	} else if(gs->balance == 1) {
		ya->balance = 0;
		s->balance = -1;
		gs->balance = 0;		
	} else if(gs->balance == 0) {
		ya->balance = 0;
		s->balance = 0;
		gs->balance = 0;				
	}
	return gs;

}
AVLitem* shiftRightThenLeft(AVLitem *ya, AVLitem *s, AVLitem *gs)
{
	//Right rotate around ya(youngest ancestor)
	ya->right=rightRotate(s);
	leftRotate(ya);
	
	//Update balances of ya and son of ya in the direction of imbalance.
	if(gs->balance == 1) {
		ya->balance = -1;
		s->balance = 0;
		gs->balance = 0;
	} else if(gs->balance == -1) {
		ya->balance = 0;
		s->balance = 1;
		gs->balance = 0;						
	} else if(gs->balance == 0) {
		ya->balance = 0;
		s->balance = 0;
		gs->balance = 0;
	}
	return gs;

}

/* Establish point at which AVL search will begin. First item returned
 * by next will have keys immediately following those specified to this
 * routine.
 */

void AVLsearch::seek(uns32 key1, uns32 key2)

{
    c_index1 = key1;
    c_index2 = key2;

    // Initially set up for next to fail
    current = 0;

    // If at end, next should fail
    if (!tree->_root || (++key2 == 0 && ++key1 == 0))
	return;
    
    // Now set current equal to previous item to key1, key2
    if ((current = tree->previous(key1, key2))) {
	c_index1 = current->_index1;
	c_index2 = current->_index2;
    }
}

/* Iterator for an AVL tree, using the ordered singly linked
 * list. A return of NULL indicates that
 * the entire tree has been searched.
 */

AVLitem *AVLsearch::next()

{
    if (!tree->_root)
	return(0);
    if (instance != tree->instance) {
        if (current)
	    seek(c_index1, c_index2);
	// We're now synced up with tree
	instance = tree->instance;
    }

    if (!current)
	current = tree->sllhead;
    else if (!current->sll)
	return(0);
    else
	current = current->sll;

    c_index1 = current->_index1;
    c_index2 = current->_index2;
    return(current);
}

/*
 * Display AVL Tree
 */

void AVLsearch::displayTree()
{
    int i;
    if (!tree->_root)
	return;
	display(tree->_root, 1);
	printf("\n");
	AVLitem *ptr1;
	AVLitem *next;
	for(ptr1= tree->_root;ptr1->left;)
		ptr1=ptr1->left;
    for (; ptr1; ptr1 = next) {
	next = ptr1->sll;
	printf("%x\n", ptr1);
    }
	printf("\n");
	printf("tree->sllhead: %x", tree->sllhead);
}

/*
 * Display AVL Tree
 */
void AVLsearch::display(AVLitem* ptr, int level)
{
    int i;

    if (ptr!=NULL)
    {
        display(ptr->right, level + 1);
		printf("\n");
        if (ptr == tree->_root) {
			printf("Root -> ");
		}
        for (i = 0; i < level && ptr != tree->_root; i++) {
			printf("        ");
		}
		printf("%x,%x,%d,%d,%d,%x", ptr->_index1, ptr->_index2,ptr->balance, ptr->in_db,ptr->refct,ptr->sll);
        display(ptr->left, level + 1);
    }
}

