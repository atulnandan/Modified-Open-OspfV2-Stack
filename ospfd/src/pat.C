
/* Routines implemneting the generic Patricia tree.
 */

#include "machdep.h"
#include "pat.h"

/* Initialize the Patricia tree. Install a dummy entry
 * with a NULL key, testing the largest bit and with pointers
 * to itself.
 */

PatTree::PatTree()

{
    init();
}

/* Initialization performed in a function other
 * than the constructor, so that the object can be 
 * reconstructed later.
 */

void PatTree::init()

{
    root = new PatEntry;
    root->zeroptr = root;
    root->oneptr = root;
    root->chkbit = MaxBit;
    root->key = 0;
    root->keylen = 0;
    size = 0;
}

/* Find an element within the Patricia tree, based on its key
 * string.
 */

PatEntry *PatTree::find(byte *key, int keylen)

{
    PatEntry  *p = NULL, *t = root;
    unsigned int    i;

    for(i=t->chkbit;;i=t->chkbit){
        t = bit_check(key, keylen, i) ? t->oneptr : t->zeroptr;
        if(i >= t->chkbit)
            break;
    }

    if((keylen == t->keylen) && memcmp(key, t->key, t->keylen) == 0)
        return t;
    return NULL;
}

/* Find a particular character string. We assume that it is
 * NULL terminated, and so just find the length and then
 * call the above routine.
 */

PatEntry *PatTree::find(char *key)

{
    int keylen;

    keylen = strlen(key);
    return(find((byte *) key, keylen));
}

PatEntry *PatTree::insert(PatEntry *child_node, PatEntry *node, unsigned int i,
                                            PatEntry *parent_node)
{
    if((child_node->chkbit >= i) || (child_node->chkbit <= parent_node->chkbit)) {
        node->chkbit = i;
        node->zeroptr = bit_check(node->key, node->keylen, i) ? child_node : node;
        node->oneptr = bit_check(node->key, node->keylen, i) ? node : child_node;
        return node;
    }

    if(bit_check(node->key, node->keylen, child_node->chkbit))
        child_node->oneptr = insert(child_node->oneptr, node, i, child_node);
    else
        child_node->zeroptr = insert(child_node->zeroptr, node, i, child_node);
    return child_node;
}

/* Add an element to a Patricia tree. We assume that the caller
 * has verified that the key is not already in the tree.
 * Find the next bit to test,
 * and then insert the node in the proper place.
 */

void PatTree::add(PatEntry *entry)

{
    unsigned int    i, keylen;
    unsigned char   *key = NULL;
    PatEntry   		*t = NULL;


    if(!root || !entry)
        return;

    key = entry->key;
    keylen = entry->keylen;

    t = root;
    /* Find closest matching leaf node */
    for(i=t->chkbit ;; i=t->chkbit){
        t = bit_check(key, keylen, i) ? t->oneptr : t->zeroptr;
        if(i >= t->chkbit)
            break;
    }
    /* Find the first bit that differs */
    for(i=0; (i < (keylen << 3)) && (bit_check(key, keylen, i) == bit_check(t->key, t->keylen, i)); i++);

    if((size == 0) || (root->chkbit > i)) {
        /* First element or least chkbit element to be inserted as root at top. */
        if(bit_check(key, keylen, i)) {
            entry->zeroptr  = root;
            entry->oneptr = entry;
        } else {
            entry->oneptr  = root;
            entry->zeroptr = entry;
        }
        entry->chkbit = i;
        root = entry;
        size++;
        return;
    }

    /* Recursive step */
    if(bit_check(key, keylen, root->chkbit)) {
            root->oneptr = insert(root->oneptr, entry, i, root);
    } else {
            root->zeroptr = insert(root->zeroptr, entry, i, root);
    }
    size++;
    return;
}

/* Delete an element from the Patricia tree. After locating the
 * element, handle the special cases:
 * 1) does the element point to itself?
 * 2) is the element the root?
 */

void PatTree::remove(PatEntry *entry)

{
    PatEntry   		*p, *t;
    PatEntry   		*gg = NULL; /*Great grandparent */
	PatEntry		*g = NULL;
    unsigned int    i, keylen;
    unsigned char   *key = NULL;
    bool            upleft, is_head_node_deleted = false;

    key = entry->key;
    keylen = entry->keylen;

    if(entry == NULL)
            return;

    /* For removal, we need to store parent and grandparent.
 	 * Find closest matching leaf node.
 	 */
	p = g = t = root;
    for(i=t->chkbit; ; i=t->chkbit){
        if((t == entry) && (gg == NULL)) {
            gg = p;
        }
        g = p;
        p = t;
        t = bit_check(key, keylen, i) ? t->oneptr : t->zeroptr;
        if(i >= t->chkbit)
            break;
    }

    // Entry not found?
    if (t != entry)
	return;

    upleft = (p->zeroptr == entry);
    if(t == root)
        is_head_node_deleted = true;

    /* Unlink grandparent from downward tree correctly. */
    if(bit_check(key, keylen, g->chkbit)) {
        g->oneptr = (upleft ? p->oneptr : p->zeroptr);
        /* If root itsef is getting deleted, changed root pointer correctly. */
        if(is_head_node_deleted)
            root = g->oneptr;
    } else {
        g->zeroptr = (upleft ? p->oneptr : p->zeroptr);
        /* If root itsef is getting deleted, changed root pointer correctly. */
        if(is_head_node_deleted)
            root = g->zeroptr;
    }

    /* Entry points to self?
 	 * If not, switch entry.
 	 */
    if (p != entry) {
        /* If root itsef is getting deleted, changed root pointer correctly. */
        if(is_head_node_deleted)
            root = p;
        else {
	        if(bit_check(key, keylen, gg->chkbit))
	            gg->oneptr = p;
	        else
	            gg->zeroptr = p;
		}

        p->chkbit = entry->chkbit;
        p->oneptr = entry->oneptr;
        p->zeroptr = entry->zeroptr;
    }
    size--;
}

/* Clear the entire Patricia tree. This is a recursive
 * operation, which deletes all the nodes.
 */

void PatTree::clear()

{
    clear_subtree(root);
    init();
}

/* Clear the subtree rooted at the given entry.
 * Works recursively.
 */

void PatTree::clear_subtree(PatEntry *entry)

{
    if (!entry)
        return;
    if (entry->zeroptr && entry->zeroptr->chkbit > entry->chkbit)
        clear_subtree(entry->zeroptr);
    if (entry->oneptr && entry->oneptr->chkbit > entry->chkbit)
        clear_subtree(entry->oneptr);
    delete entry;
}
