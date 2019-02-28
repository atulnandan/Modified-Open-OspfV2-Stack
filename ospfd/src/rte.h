
class TNode;
class LSA;
class SpfIfc;
class SpfData;
class INrte;
//REMOVE
class FWDrte;

/* Data structure storing multiple equal-cost paths.
 */

struct NH {
    InAddr if_addr; // IP address of outgoing interface
    int phyint; // Physical interface
    InAddr gw;	// New hop gateway
};

class MPath : public PatEntry {
  public:
    int	npaths;
    NH	NHs[MAXPATH];
    int pruned_phyint;
    MPath *pruned_mpath;
    static PatTree nhdb;
    static MPath *create(int, NH *);
    static MPath *create(SpfIfc *, InAddr);
    static MPath *create(int, InAddr);
    static MPath *merge(MPath *, MPath *);
    static MPath *addgw(MPath *, InAddr);
    MPath *prune_phyint(int phyint);
    bool all_in_area(class SpfArea *);
    bool some_transit(class SpfArea *);
};	

/* Defines for type of routing table entry
 * Organized from most preferred to least preferred.
 */

enum {
    RT_DIRECT=1,  // Directly attached
    RT_SPF,	// Intra-area
    RT_SPFIA,	// Inter-area
    RT_EXTT1,	// External type 1
    RT_EXTT2,	// External type 2
    RT_REJECT,	// Reject route, for own area ranges
    RT_STATIC,	// External routes we're importing
    RT_NONE,	// Deleted, inactive
};

/* The IP routing table.
 */

class INtbl {
  protected:
    AVLtree root; 	// Root of AVL tree
  public:
    INrte *add(uns32 net, uns32 mask);
    inline INrte *find(uns32 net, uns32 mask);
    INrte *best_match(uns32 addr);
    friend class INiterator;
    friend class OSPF;
};

inline INrte *INtbl::find(uns32 net, uns32 mask)

{
    return((INrte *) root.find(net, mask));
}

/* Data that is used only for internal OSPF routes (intra-
 * and inter-area routes).
 */

class SpfData {
  public:
    byte lstype; 	// LSA leading to entry
    lsid_t lsid;
    rtid_t rtid;
    aid_t r_area; 	// Associated area
    MPath *old_mpath;	// Old next hops
    uns32 old_cost;	// Old cost
};

/* Definition of the generic routing table entry. Organized as a
 * balanced or AVL tree, this is the base class for both
 * IP and router routing table entries.
 */

class RTE : public AVLitem {
  protected:
    byte r_type; 	// Type of routing table entry
    SpfData *r_ospf;	// Intra-AS OSPF-specific data

  public:
    byte changed:1,	// Entry changed?
	dijk_run:1;	// Dijkstra run, sequence number
    MPath *r_mpath; 	// Next hops
    MPath *last_mpath; 	// Last set of Next hops given to kernel
    uns32 cost;		// Cost of routing table entry
    uns32 t2cost;	// Type 2 cost of entry

    RTE(uns32 key_a, uns32 key_b);
    void new_intra(TNode *V, bool stub, uns16 stub_cost, int index);
    void host_new_intra(SpfIfc *ip, uns32 new_cost);
    virtual void set_origin(LSA *V);
    virtual void declare_unreachable();
    LSA	*get_origin();
    void save_state();
    bool state_changed();
    void set_area(aid_t);
    SpfIfc *ifc();
    inline void update(MPath *newnh);
    inline byte type();
    inline int valid();
    inline int intra_area();
    inline int inter_area();
    inline int intra_AS();
    inline aid_t area();

    friend class SpfArea;
    friend class summLSA;
    friend class FWDrte;
};

// Inline functions
inline void RTE::update(MPath *newnh)
{
    r_mpath = newnh;
}
inline byte RTE::type()
{
    return(r_type);
}
inline int RTE::valid()
{
    return(r_type != RT_NONE);
}
inline int RTE::intra_area()
{
    return(r_type == RT_SPF);
}
inline int RTE::inter_area()
{
    return(r_type == RT_SPFIA);
}
inline int RTE::intra_AS()
{
    return(r_type == RT_SPF || r_type == RT_SPFIA);
}
inline aid_t RTE::area()
{
    return((r_ospf != 0) ? r_ospf->r_area : 0);
}

/* Definition of the IP routing table entry. Organized as a balanced
 * or AVL tree, with the key being the combination of the
 * network number (most significant) and the network mask (least
 * significant.
 *
 * Non-contiguous subnet masks are not supported.
 *
 * Each entry has a pointer to the previous least significant match
 * (prefix).
 */

class INrte : public RTE {
  protected:
    INrte *_prefix;	// Previous less specific match
    uns32 tag;		// Advertised tag

  public:
    class summLSA *summs;       // summary-LSAs
    byte range:1,		// Configured area address range?
	 ase_orig:1;		// Have we originated an AS-external-LSA?

    inline INrte(uns32 xnet, uns32 xmask);
    inline uns32 net();
    inline uns32 mask();
    inline bool matches(InAddr addr);
    inline uns32 broadcast_address();
    inline INrte *prefix();
    inline int is_range();
    inline int is_child(INrte *o);
    int within_range();

    summLSA *my_summary_lsa(); // Find the one we originated
    void run_inter_area(); // Calculate inter-area routes
    void incremental_summary(SpfArea *);
    void sys_install();  // Install routes into kernel
    virtual void declare_unreachable();

    friend class SpfArea;
    friend class summLSA;
    friend class INiterator;
    friend class INtbl;
    friend class OSPF;
};

// Inline functions
inline INrte::INrte(uns32 xnet, uns32 xmask) : RTE(xnet, xmask)
{
    _prefix = 0;
    tag = 0;
    summs = 0;
    range = false;
    ase_orig = false;
}
inline uns32 INrte::net()
{
    return(index1());
}
inline uns32 INrte::mask()
{
    return(index2());
}
inline bool INrte::matches(InAddr addr)
{
    return((addr & mask()) == net());
}
inline uns32 INrte::broadcast_address()
{
    return(net() | ~mask());
}
inline INrte *INrte::prefix()
{
    return(_prefix);
}
inline int INrte::is_range()
{
    return(range != 0);
}
inline int INrte::is_child(INrte *o)
{
    return((net() & o->mask()) == o->net() && mask() >= o->mask());
}

/* Iterator for the routing table. Simply follows the singly
 * linked list.
 */

class INiterator : public AVLsearch {
  public:
    inline INiterator(INtbl *);
    inline INrte *nextrte();
};

// Inline functions
inline INiterator::INiterator(INtbl *t) : AVLsearch(&t->root)
{
}
inline INrte *INiterator::nextrte()
{
    return((INrte *) next());
}

/* Routing table entry for routers, per-area.
 */

class RTRrte : public RTE {
  public:
    byte rtype;		// Router type
    RTRrte *asbr_link;	// Linked in ASBR entry
    class VLIfc *VL;	// configured VL w/ this endpoint
    class SpfArea *ap;	// area to which router belongs

    RTRrte(uns32 rtrid, class SpfArea *ap);
    virtual ~RTRrte();

    inline uns32 rtrid();
    inline bool	b_bit();// area border router?
    inline bool	e_bit();// AS boundary router?
    inline bool	v_bit();// virtual link endpoint?
    inline bool	w_bit();// wild-card multicast receiver?
    virtual void set_origin(LSA *V);
};

// Inline functions
inline uns32 RTRrte::rtrid()
{
    return(index1());
}
inline bool RTRrte::b_bit()
{
    return((rtype & RTYPE_B) != 0);
}
inline bool RTRrte::e_bit()
{
    return((rtype & RTYPE_E) != 0);
}
inline bool RTRrte::v_bit()
{
    return((rtype & RTYPE_V) != 0);
}
inline bool RTRrte::w_bit()
{
    return((rtype & RTYPE_W) != 0);
}

class FWDtbl {
  protected:
    AVLtree root; 	// Root of AVL tree
  public:
    FWDrte *add(uns32 addr);
    void resolve();
    void resolve(INrte *changed_rte);
    friend class OSPF;
};

class FWDrte : public RTE {
    SpfIfc *ifp;	// On this interface
    INrte *match;	// Best matching routing table entry
  public:
    inline FWDrte(uns32 addr);
    inline InAddr address();
    void resolve();
    friend class OSPF;
    friend class INrte;
    friend class ExRtData;
    friend class FWDtbl;
};

// Inline functions
inline FWDrte::FWDrte(uns32 addr) : RTE(addr, 0)
{
    ifp = 0;
    match = 0;
}
inline InAddr FWDrte::address()
{
    return(index1());
}

/* Data structure used to store differences between the
 * kernel routing table and OSPF's. Time is when the
 * kernel has reported a deletion that we didn't know
 * about.
 */

class KrtSync : public AVLitem {
  public:
    SPFtime tstamp;
    KrtSync(InAddr net, InMask mask);
};
