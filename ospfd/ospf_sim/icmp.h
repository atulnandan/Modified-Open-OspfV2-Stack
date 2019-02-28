
/* Necessary defintions for the ICMP protocol
 * We use this in the simulator's ping and traceroute
 * utilities.
 */

/* The ICMP packet header
 */

struct IcmpPkt {
    byte ic_type;	// ICMP type
    byte ic_code; 	// code (subtype)
    uns16 ic_chksum;	// Header checksum
    uns16 ic_id;	// Identification (ping)
    uns16 ic_seqno;	// Sequence number (ping)
};

const int ICMP_ERR_RETURN_BYTES = 128;

/* ICMP packet types and codes
 * We define only those that we use.
 */

const byte ICMP_TYPE_ECHOREPLY = 0;
const byte ICMP_TYPE_UNREACH = 3;
const byte ICMP_CODE_UNREACH_HOST = 1;
const byte ICMP_CODE_UNREACH_PORT = 3;
const byte ICMP_TYPE_ECHO = 8;
const byte ICMP_TYPE_TTL_EXCEED = 11;
