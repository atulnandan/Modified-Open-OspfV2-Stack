
// Interval timer. When fires, database aging performed.

class DBageTimer : public ITimer {
  public:
    virtual void action();
};

// Global timer queue
extern PriQ timerq;		// Currently pending timers

/* The OSPF base class. This class contains all the data necessary
 * to run a sungle instance of the OSPF protocol.
 */

class OSPF : public ConfigItem {
    // Global configuration data
    const rtid_t myid;	// Our router ID
    int	new_flood_rate; // # self-orig per second
    uns16 max_rxmt_window;// # back-to-back retransmissions
    byte max_dds;	// # simultaneous DB exchanges
    byte host_mode;	// Don't forward data packets?
    int32 refresh_rate;	// Rate to refresh DoNotAge LSAs
    uns32 PPAdjLimit;	// Max # p-p adjacencies to neighbor
    bool random_refresh;// Should we spread out LSA refreshes?
    // Dynamic data
    InAddr myaddr;	// Global address: source on unnumbered
    bool wakeup; 	// Timers running?
    SpfIfc *ifcs; 	// List of interfaces
    uns32 wo_donotage;	// #LSAs claiming no DoNotAge support
    bool dna_change;	// Change in network DoNotAge support
    LsaList dna_flushq; // DoNotAge LSAs being flushed from lack of support
    SpfNbr *g_adj_head;	// Adjacencies to form, head
    SpfNbr *g_adj_tail;	// Adjacencies to form, tail
    byte *build_area;	// build area
    uns16 build_size;	// size of build area
    byte *orig_buff;	// Origination staging area
    uns16 orig_size;	// size of staging area
    bool orig_buff_in_use;// Staging area being used?
    uns32 n_orig_allocs;// # allocs for staging area
    byte *mon_buff;	// Monitor replay staging area
    int mon_size;	// size of staging area
    int	shutdown_phase;	// Shutting down if > 0
    int	countdown;	// Number of seconds before exit
    int hitless_prep_phase;// Hitless restart version
    int phase_duration; // Time in current phase
    int grace_period;	// Length of grace period (current or next)
    byte restart_reason;// Encoding in lshdr.h
    TLVbuf tlvbuf;	// Buffer in which grace-LSAs are built
    bool delete_neighbors; // Neighbors being deleted?
    AVLtree phyints;	// Physical interfaces
    AVLtree krtdeletes;	// Deleted, unsynced kernel routing entries
    bool need_remnants; // Yet to get remnants?
    // Flooding queues
    int	n_local_flooded;// AS-external-LSAs originated this tick
    LsaList replied_list; // LSAs that we have recently sent
			 // in reponse to old LSAs received
    // For LSA aging
    DBageTimer dbtim;	// Database aging timer
    LsaList MaxAge_list; // MaxAge LSAs, being flushed
    uns32 total_lsas;	// Total number of LSAs in all databases
    LsaList dbcheck_list; // LSAs whose checksum is being verified
    LsaList pending_refresh; // LSAs awaiting refresh
    // Database Overflow variables
    bool OverflowState;	// true => database has overflowed
    // Group membership
    AVLtree ospfd_membership; // Our application's

    SpfArea *areas; 	// List of areas
    SpfArea *summary_area;
    SpfArea *first_area;
    int	n_area;		// Number of actively attached areas
    int	n_dbx_nbrs;	// # nbrs undergoing database exchange
    int	n_lcl_inits;	// # locally initiated
    int	n_rmt_inits;	// # remotely initiated
    uns16 ospf_mtu;	// Max IP datagram for all interfaces
    Pkt	o_update;	// Current flood
    Pkt	o_demand_upd;	// Current flood out demand interfaces
    // State flags
    int	full_sched:1,	// true => full calculation scheduled
	ase_sched:1;	// true => all ases should be reexamined
    // Statistics
    uns32 n_dijkstras;
    // Logging variables
    int logno;		// Logging event number
	/* ATUL */
    char logbuf[800];   // Logging buffer
    char *logptr;       // Current place in logging buffer
    char *logend;	// End of logging buffer
    int base_priority;
    bool disabled_msgno[MAXLOG+1];
    bool enabled_msgno[MAXLOG+1];
    AVLtree remnants;	// Entries installed before restart	
    bool start_htl_exit;
    bool exiting_htl_restart;
    bool check_htl_termination;
    char *htl_exit_reason;
    // Helper variables
    bool topology_change;
    SPFtime start_time;
    int n_helping;	// # neighbors being helped

    // Monitoring routines
    class MonMsg *get_monbuf(int size);
    void global_stats(class MonMsg *, int conn_id);
    void area_stats(class MonMsg *, int conn_id);
    void interface_stats(class MonMsg *, int conn_id);
    void vl_stats(class MonMsg *, int conn_id);
    void neighbor_stats(class MonMsg *, int conn_id);
    void vlnbr_stats(class MonMsg *, int conn_id);
    void lsa_stats(class MonMsg *, int conn_id);
    void rte_stats(class MonMsg *, int conn_id);
    void opq_stats(class MonMsg *, int con_id);
    void lllsa_stats(class MonMsg *, int conn_id);

    // Utility routines
    void clear_config();
    SpfIfc *find_ifc(uns32 addr, int phyint = -1);
    SpfIfc *next_ifc(uns32 addr, int phyint);
    SpfIfc *find_ifc(Pkt *pdesc);
    SpfIfc *find_vl(aid_t transit_id, rtid_t endpt);
    SpfIfc *next_vl(aid_t transit_id, rtid_t endpt);
    SpfIfc *find_nbr_ifc(InAddr nbr_addr);
    SpfNbr *find_nbr(InAddr nbr_addr, int phyint);
    SpfNbr *next_nbr(InAddr nbr_addr, int phyint);
    int run_fsm(FsmTran *table, int& i_state, int event);
    int	ospf_getpkt(Pkt *pkt, int type, uns16 size);
    void ospf_freepkt(Pkt *pkt);
    void delete_down_neighbors();
    void app_join(int phyint, InAddr group);
    void app_leave(int phyint, InAddr group);
    void phy_attach(int phyint);
    void phy_detach(int phyint, InAddr if_addr);
    void calc_my_addr();
    LShdr *orig_buffer(int ls_len);
    void free_orig_buffer(LShdr *);
    inline int mospf_enabled();
    inline bool	mc_abr();
    inline bool	shutting_down();
    inline int donotage();
    inline InAddr my_addr();

    // Database routines
    AVLtree *FindLSdb(SpfIfc *, SpfArea *ap, byte lstype);
    LSA	*FindLSA(SpfIfc *, SpfArea *, byte lstype, lsid_t lsid, rtid_t rtid);
    LSA	*myLSA(SpfIfc *, SpfArea *, byte lstype, lsid_t lsid);
    LSA	*AddLSA(SpfIfc *,SpfArea *, LSA *current, LShdr *hdr, bool changed);
    void DeleteLSA(LSA *lsap);
    LSA *NextLSA(aid_t, byte, lsid_t, rtid_t);
    LSA *NextLSA(InAddr, int, aid_t, byte, lsid_t, rtid_t);
    void update_lsdb_xsum(LSA *, bool add);
    Range *GetBestRange(INrte *rte);
    SpfArea *FindArea(aid_t id);
    SpfArea *NextArea(aid_t &id);
    inline SpfArea *SummaryArea();	// summary-LSAs from this area used
    void ParseLSA(LSA *lsap, LShdr *hdr);
    void UnParseLSA(LSA *lsap);
    LShdr *BuildLSA(LSA *lsap, LShdr *hdr=0);
    void send_updates();
    bool maxage_free(byte lstype);
    void flush_self_orig(AVLtree *tree);
    void flush_donotage();
    void shutdown_continue();
    void rl_orig();
    void delete_from_ifmap(SpfIfc *ip);
    void upload_opq(LSA *);

    // Database aging
    void dbage();
    void deferred_lsas();
    void checkages();
    void refresh_lsas();
    void maxage_lsas();
    void refresh_donotages();
    void free_maxage_lsas();
    void donotage_changes();
    void schedule_refresh(LSA *);
    void do_refreshes();

    // LSA origination
    int	self_originated(SpfNbr *, LShdr *hdr, LSA *database_copy);
    int	get_lsid(INrte *rte, byte lstype, SpfArea *ap, lsid_t &id);
    seq_t ospf_get_seqno(byte lstype, LSA *lsap, int forced);
    LSA	*lsa_reorig(SpfIfc *,SpfArea *ap, LSA *olsap, LShdr *hdr, int forced);
    void age_prematurely(LSA *);

    void sl_orig(INrte *rte, bool transit_changes_only=false);
    void reoriginate_ASEs();
    void build_update(Pkt *pkt, LShdr *hdr, uns16 mtu, bool demand);
    void add_to_update(LShdr *hdr, bool demand);
    void redo_aggregate(INrte *rangerte, SpfArea *ap);
    void EnterOverflowState();

    // Routing calculations
    void rtsched(LSA *newlsa, RTE *old_rte);
    void full_calculation();
    void dijk_init(PriQ &cand);
    void host_dijk_init(PriQ &cand);
    void add_cand_node(SpfIfc *ip, TNode *node, PriQ &cand);
    void dijkstra();
	void update_brs();
    void invalidate_ranges();
    void rt_scan();
    void update_area_ranges(INrte *rte);
    void advertise_ranges();
    void do_all_ases();
    void krt_sync();
   
    // Logging routines
    bool spflog(int, int);
    void log(int);
	/* ATUL */
    //void log(char *);
    void log(Pkt *pdesc);
    void log(InPkt *iphdr);
    void log(class LShdr *);
    void log(class SpfArea *);
    void log(class SpfIfc *);
    void log(class SpfNbr *);
    void log(InAddr *addr);
    void log(class LSA *);
    void log(class INrte *);
    void log(InAddr *addr, InMask *mask);

  public:
    // Version numbers
    enum {
	vmajor = 2,	// Major version number
	vminor = 16,	// Minor version number
    };

    void log(char *);
    // Entry points into the OSPF code
    OSPF(uns32 rtid, SPFtime grace);
    ~OSPF();
    void rxpkt(int phyint, InPkt *pkt, int plen);
    int	timeout();
    void tick();
    void monitor(struct MonMsg *msg, byte type, int size, int conn_id);
    void rxigmp(int phyint, InPkt *pkt, int plen);
    void phy_up(int phyint);
    void phy_down(int phyint);
    void krt_delete_notification(InAddr net, InMask mask);
    void remnant_notification(InAddr net, InMask mask);
    MPath *ip_lookup(InAddr dest);
    InAddr ip_source(InAddr dest);
    InAddr if_addr(int phyint);
    void shutdown(int seconds);
    void logflush();
    inline rtid_t my_id();
    inline int n_extLSAs();
    inline uns32 xsum_extLSAs();
    // Configuration routines
    void cfgOspf(struct CfgGen *msg);
    void cfgArea(struct CfgArea *msg, int status);
    void cfgRnge(struct CfgRnge *msg, int status);
    void cfgIfc(struct CfgIfc *msg, int status);
    void cfgHost(struct CfgHost *msg, int status);
    void cfgVL(struct CfgVL *msg, int status);
    void cfgAuKey(struct CfgAuKey *key, int status);
    void cfgNbr(struct CfgNbr *msg, int status);
    void cfgExRt(struct CfgExRt *msg, int status);
    void cfgStart();
    void cfgDone();
	void TraverseTree(AVLitem* ptr,int level, AVLitem* _root);

    friend class IfcIterator;
    friend class AreaIterator;
    friend class DBageTimer;
    friend class Timer;
    friend class ITimer;
    friend class SpfNbr;
    friend class ConfigItem;
    friend class SpfIfc;
    friend class PhyInt;
    friend class LocalOrigTimer;
    friend class SpfArea;
    friend class LSA;
    friend class TNode;
    friend class PPIfc;
    friend class DRIfc;
    friend class RTE;
    friend class netLSA;
    friend class HostAddr;
    friend class Range;
    friend class INrte;
    friend class FWDrte;
    friend class StaticNbr;
    friend class FWDtbl;
    friend class MPath;
    friend void lsa_flush(class LSA *);
    friend SpfNbr *GetNextAdj();
};

// Declaration of the single OSPF protocol instance
extern OSPF *ospf;

// Inline functions
inline rtid_t OSPF::my_id()
{
    return(myid);
}
inline bool OSPF::shutting_down()
{
    return(shutdown_phase > 0);
}
inline SpfArea *OSPF::SummaryArea()
{
    return(summary_area);
}
inline int OSPF::donotage()
{
    if (wo_donotage != 0)
        return(false);
    else if (summary_area)
        return(summary_area->donotage());
    else
        return(true);
}
inline int OSPF::n_extLSAs()
{
    //return(extLSAs.size());
    return 0;
}
inline uns32 OSPF::xsum_extLSAs()
{
    //return(ase_xsum);
    return 0;
}
inline InAddr OSPF::my_addr()
{
    return(myaddr);
}

// non-class-related function declarations
void lsa_flush(LSA *lsap);
int flooding_scope(byte lstype);
