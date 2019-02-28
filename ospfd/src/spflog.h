
/* Defines used by the OSPF logging routines
 */

enum {
    CFG_START = 1,      // Starting OSPF
    CFG_ADD_AREA,       // Adding area
    CFG_DEL_AREA,       // Deleting area
    CFG_ADD_IFC,        // Adding interface
    CFG_DEL_IFC,        // Deleting interface
    CFG_HTLRST,		// Hitless restart

    RCV_SHORT = 100,	// Received packet too short
    RCV_BADV,		// Received bad OSPF version number
    RCV_NO_IFC,		// No matching receive interface
    RCV_NO_NBR,		// No matching neighbor
    RCV_AUTH,		// Authentication failure
    RCV_NOTDR,		// Addressed to AllDRouters, we're not
    ERR_LSAXSUM,	// Bad LSA checksum
    ERR_EX_IN_STUB,	// AS-external-LSAs in stub area
    ERR_BAD_LSA_TYPE,	// Unrecognized LS type
    ERR_OLD_ACK,	// Newer instance on rxmt list
    ERR_NEWER_ACK,	// Older instance on rxmt list
    ERR_IFC_FSM,	// Case not handled by interface FSM
    ERR_NBR_FSM,	// Case not handled in nbr FSM
    ERR_SYS,		// Error in system interface
    ERR_DONOTAGE,	// Received DoNotAge when incapable
    ERR_NOADDR,		// Send fails: no valid source address
    IGMP_RCV_SHORT,	// Received IGMP packet too short
    IGMP_RCV_XSUM,	// Received IGMP packet bad xsum
    IGMP_RCV_NOIFC,	// No matching interface for received IGMP

    LOG_DRCH = 200,	// DR/Backup election
    LOG_RCVPKT,		// Received OSPF packet
    LOG_TXPKT,		// Transmitted OSPF packet
    LOG_RXNEWLSA,	// Received New LSA
    LOG_LSAORIG,	// Originated LSA
    LOG_SELFORIG,	// Receive self-originated LSA
    LOG_MINARRIVAL,	// LSA Fails min-arrival test
    DUP_ACK,		// Ack, but no instance on rxmt list
    LOG_RXMTUPD,	// Retransmit LS update
    LOG_RXMTDD,		// Retransmit DD description
    LOG_RXMTRQ,		// Retransmit LS request
    LOG_LSAFLUSH,	// Flushing LSA
    LOG_LSAFREE,	// Freeing LSA
    LOG_LSAREFR,	// Refreshing LSA
    LOG_LSAMAXAGE,	// LSA hits MaxAge
    LOG_LSADEFER,	// Deferred LS origination
    LOG_LSAWRAP,	// Sequence number wrap
    LOG_ADDRT,		// Add/modify route to routing table
    LOG_ADDREJECT,	// Add reject route to routing table
    LOG_DELRT,		// Delete route from routing table
    LOG_JOIN,		// Join group on physical interface
    LOG_LEAVE,		// Leave group on physical interface
    IFC_STATECH,	// Interface state change
    NBR_STATECH,	// Nbr state change
    LOG_BADMTU,		// MTU mismatch in Database Exchange
    LOG_IMPORT,		// Importing external route
    LOG_DNAREFR,	// Refreshing DoNotAge LSA
    LOG_JOINED,		// Host (not us) joined group
    LOG_LEFT,		// Host (not us) left group
    LOG_QUERIER,	// Change of IGMP querier
    MCACHE_REQ,		// Multicast cache request
    LOG_NEWGRP,		// New group
    LOG_GRPEXP,		// Group expired
    IGMP_RCV,		// Received IGMP packet
    LOG_SPFDEBUG,	// Debug statements
    LOG_KRTSYNC,	// Synch kernel routing entry
    LOG_REMNANT,	// Deleting remnant routing entry
    LOG_DEBUGGING,	// Debug statements
    LOG_HITLESSPREP,	// Preparing for hitless restart
    LOG_PHASEFAIL,	// Hitless restart preparation failure
    LOG_PREPDONE,	// Hitless restart preparation done
    LOG_RESIGNDR,	// Resigned DR
    LOG_GRACEACKS,	// Grace-LSAs acked
    LOG_GRACERX,	// Grace-LSA received
    LOG_HELPER_START,	// Enter helper mode
    LOG_HELPER_STOP,	// Leave helper mode
    LOG_GRACE_REJECT,	// Reject grace request
    LOG_HTLEXIT,	// Exiting hitless restart
    MAXLOG,		// KEEP THIS LAST!!!!
};

/* Error codes used in calling OspfSysCalls::halt(), terminating
 * the program for a specified reason.
 * 0 is reserved to indicated that shutdown was requested
 * by the user.
 */

enum {
    HALT_DBCORRUPT = 1,	// Link-state database corrupted
    HALT_LSTYPE,	// Bad LS type in database add function
    HALT_IFCRM,		// Interface remove failed
    HALT_RTCOST,	// Inconsistent routing table cost
    HALT_AUTYPE,	// Bad authentication type in generate
};
