
/* Various utility definitions used by OSPF
 */


/* Definition of a data packet, used to pass datagrams around
 * in OSPF. One of these structures is created when either
 * a) an OSPF packets is received from the IP layer or 
 * b) a packet is allocated so that it can be later sent.
 */

struct Pkt {		// As received from IP
    InPkt *iphdr;	// The IP header
    int	phyint;		// Associated physical interface
    bool llmult;	// Link level multicast?
    bool hold;		// Don't free
    bool xsummed;	// Body already checksummed?
    // Initialized by OSPF
    SpfPkt *spfpkt;	// OSPF packet header
    byte *end;		// End of packet
    int	bsize;		// Size of packet buffer
    // Modified as passed through OSPF
    byte *dptr;		// Current data pointer
    uns16 body_xsum;	// Checksum of packet body

    Pkt();
    Pkt(int phy, InPkt *inpkt);
    bool partial_checksum();
};

/* The OSPF FSM transition. An array of these forms an OSPF
 * FSM.
 */

struct FsmTran {
    int states;		// Set of states to match
    int	event;		// Event to match
    int	action;		// Resulting action to take
    int	new_state;	// New state
};

/* Utility routines implemented in spfutil.C
 */

uns16 fletcher(byte *message, int mlen, int offset);
uns16 incksum(uns16 *, int len, uns16 seed=0);

// Standard min/max functions

#ifndef MAX
inline int MAX(int a, int b) { return((a < b) ? b : a); }
#endif
#ifndef MIN
inline int MIN(int a, int b) { return((a < b) ? a : b); }
#endif
