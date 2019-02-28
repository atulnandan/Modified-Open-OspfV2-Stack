
/* Definitions for the generic Patricia tree data
 * structure.
 */

/* Patricia bit check operation. Returns true if bit is set,
 * false otherwise. We're doing exact match, including length,
 * so we append the length as last four bytes of the string,
 * and then zero fill after that.
 */

inline bool bit_check(byte *str, int len, int bit)

{
    int32 bitlen;

    bitlen = len << 3;
    if (bit < bitlen)
	return((str[bit>>3] & (0x80 >> (bit & 0x07))) != 0);
    else if (bit < bitlen + 32)
	return((bitlen & (1 << (bit - bitlen))) != 0);
    else
	return(false);
}

/* Element within a Patricia tree.
 */

const uns32 MaxBit = 0xffffffffL;

class PatEntry {
    PatEntry *zeroptr;
    PatEntry *oneptr;
    uns32 chkbit;
  public:
    byte *key;
    int	keylen;

    inline bool	bit_check(int bit);
    friend class PatTree;
};

inline bool PatEntry::bit_check(int bit)
{
    return(::bit_check(key, keylen, bit));
}

/* The Patricia tree root.
 */

class PatTree {
    PatEntry *root;
    int	size;
  public:
    PatTree();
    void init();
    void add(PatEntry *);
    PatEntry *find(byte *key, int keylen);
    PatEntry *find(char *key);
    void remove(PatEntry *);
    void clear();
    void clear_subtree(PatEntry *);
	PatEntry *insert(PatEntry *child_node, PatEntry *node, unsigned int i,
                                            PatEntry *parent_node);
};
