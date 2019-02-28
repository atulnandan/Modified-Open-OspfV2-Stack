
/* Declaration and implementation of the stack class.
 * This is used by the AVL trees deletion function.
 */

class Stack {
    enum {StkSz = 32};
    void *stack[StkSz];
    void **sp;
    void **marker;

public:
    Stack() : sp(stack) {};
    inline void push(void *ptr);
    inline void *pop();
    inline void mark();
    inline void replace_mark(void *ptr);
    inline int is_empty();
    inline void *truncate_to_mark();
    inline void reset();
};

// Inline functions
inline void Stack::push(void *ptr)
{
    *sp++ = ptr;
}
inline void *Stack::pop()
{
    if (sp > stack)
	return(*(--sp));
    else return(0);
}
inline void Stack::mark()
{
    marker = sp;
}
inline void Stack::replace_mark(void *ptr)
{
    *marker = ptr;
}
inline int Stack::is_empty()
{
    return(sp == stack);
}
inline void *Stack::truncate_to_mark()
{
    sp = marker;
    return(*sp);
}
inline void Stack::reset()
{
    sp = stack;
}
