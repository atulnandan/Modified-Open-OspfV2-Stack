
/* Necessary definitions for version 2 of the IGMP protocol
 */

/* The IGMPv2 packet header
 */

struct IgmpPkt {
    byte ig_type;	// IGMP packet type
    byte ig_rsptim;	// Response time (in tenths of seconds)
    uns16 ig_chksum;	// Packet checksum
    InAddr ig_group; 	// Multicast group
};

/* IGMPv2 packet types
 */

const byte IGMP_MEMBERSHIP_QUERY = 0x11;
const byte IGMP_V1MEMBERSHIP_REPORT = 0x12;
const byte IGMP_MEMBERSHIP_REPORT = 0x16;
const byte IGMP_LEAVE_GROUP = 0x17;

/* IGMPv2 addresses.
 */

const InAddr IGMP_ALL_SYSTEMS = 0xE0000001;
const InAddr IGMP_ALL_ROUTERS = 0xE0000002;

/* Is the multicast address routable?
 */

inline bool LOCAL_SCOPE_MULTICAST(InAddr group)

{
    return((group & 0xffffff00) == 0xe0000000);
}
