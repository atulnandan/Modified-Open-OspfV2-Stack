
/* Routines dealing with physical interfaces.
 * There may be one or more OSPF interfaces defined
 * over each physical interface; OSPF interfaces
 * are operated upon in spfifc.C.
 */

#include "ospfinc.h"
#include "phyint.h"
#include "ifcfsm.h"
#include "system.h"
#include "igmp.h"

/* Constructor for a physical interface.
 */

PhyInt::PhyInt(int phyint) 
  : AVLitem((uns32 ) phyint, 0), qrytim(this), strqtim(this), oqtim(this)

{
    operational = true;
    my_addr = 0;
    mospf_ifp = 0;
    igmp_querier = 0;
    igmp_enabled = false;
    // Initialize IGMP configurable constants to RFC 2236 defaults
    robustness_variable = 2;
    query_interval = 125;
    query_response_interval = 100;// Tenths of seconds
    group_membership_interval = robustness_variable * query_interval;
    // Next is more liberal than spec, to survive Querier changes
    group_membership_interval += (query_response_interval*3)/20;
    other_querier_present_interval = robustness_variable * query_interval;
    other_querier_present_interval += query_response_interval/20;
    startup_query_interval = query_interval/4;
    startup_query_count = robustness_variable;
    last_member_query_interval = 10;// Tenths of seconds
    last_member_query_count = robustness_variable;
}

/* OSPFD application attaches to a physical interface.
 */

void OSPF::phy_attach(int phyint)

{
    PhyInt *phyp;

    if (!(phyp = (PhyInt *)phyints.find((uns32) phyint, 0))) {
	phyp = new PhyInt((uns32) phyint);
	phyints.add(phyp);
	sys->phy_open(phyint);
    }
    
    phyp->ref();
}

/* OSPFD application leaves a group on a physical interface.
 */

void OSPF::phy_detach(int phyint, InAddr if_addr)

{
    PhyInt *phyp;

    if ((phyp = (PhyInt *)phyints.find((uns32) phyint, 0))) {
	phyp->deref();
	if (phyp->not_referenced()) {
	    sys->phy_close(phyint);
	    phyints.remove(phyp);
	    delete phyp;
	}
	else {
	    if (phyp->igmp_querier == if_addr)
	        phyp->igmp_querier = 0;
	}
    }
}

/* OSPFD application joins a group on a physical interface.
 */

void OSPF::app_join(int phyint, InAddr group)

{
    AVLitem *member;

    if (!(member = ospfd_membership.find((uns32) phyint, group))) {
	member = new AVLitem((uns32) phyint, group);
	ospfd_membership.add(member);
	sys->join(group, phyint);
	if (spflog(LOG_JOIN, 4)) {
	    log(&group);
	    log(" Ifc ");
	    log(sys->phyname(phyint));
	}
    }
    
    member->ref();
}

/* OSPFD application leaves a group on a physical interface.
 */

void OSPF::app_leave(int phyint, InAddr group)

{
    AVLitem *member;

    if ((member = ospfd_membership.find((uns32) phyint, group))) {
	member->deref();
	if (member->not_referenced()) {
	    sys->leave(group, phyint);
	    ospfd_membership.remove(member);
	    member->chkref();
	    if (spflog(LOG_LEAVE, 4)) {
		log(&group);
		log(" Ifc ");
		log(sys->phyname(phyint));
	    }
	}
    }
}

/***************************************************************
 *   An implemention of Version 2 of the IGMP
 *   protocol.
 **************************************************************/

/* Start the IGMP querier duties by sending out a sequence
 * of General Queries at a higher rate.
 */

void PhyInt::start_query_duties()

{
    oqtim.stop();
    send_query(0);
    if (startup_query_count > 1) {
        strqtim.startup_queries = startup_query_count - 1;
        strqtim.start(startup_query_interval*Timer::SECOND, false);
    }
}

/* Stop IGMP query duties by stopping any timers which
 * might generate General Queries.
 */

void PhyInt::stop_query_duties()

{
    strqtim.stop();
    qrytim.stop();
}

/* The Startup Query Timer.
 * "startup_queries" will be set right before the timer is started,
 * so that it will have the most recently configured value.
 */

StartupQueryTimer::StartupQueryTimer(PhyInt *p)

{
    phyp = p;
}

void StartupQueryTimer::action()

{
    phyp->send_query(0);
    if (--startup_queries <= 0) {
        stop();
	phyp->qrytim.start(phyp->query_interval*Timer::SECOND, false);
    }
}

/* The regular query timer. Goes off continuously, sending
 * out Host Membership Queries at a regular interval.
 */

IGMPQueryTimer::IGMPQueryTimer(PhyInt *p)

{
    phyp = p;
}

void IGMPQueryTimer::action()

{
    phyp->send_query(0);
}

/* The timer measuring whether we should replace the
 * current querier, when/if it disappears.
 */

IGMPOtherQuerierTimer::IGMPOtherQuerierTimer(PhyInt *p)

{
    phyp = p;
}

void IGMPOtherQuerierTimer::action()

{
    phyp->igmp_querier = 0;
}


/* Send a Host Membership Query.
 * It is a general query if the group is 0.
 * Otherwise, it is a specific query sent in response
 * to a receive leave group message.
 * We're not quite to spec, as we don'y include a
 * router alert option!
 */

void PhyInt::send_query(InAddr group)

{
    InPkt *iphdr;
    IgmpPkt *ig_pkt;
    InAddr dest;
    byte max_response;
    int plen;

    if (!IAmQuerier())
        return;
    // General queries
    if (group == 0) {
        dest = IGMP_ALL_SYSTEMS;
	max_response = query_response_interval;
    }
    // Group-specific queries
    else {
        dest = group;
	max_response = last_member_query_interval;
    }
    plen = sizeof(InPkt) + sizeof(IgmpPkt);
    iphdr = (InPkt *) new byte[plen];
    iphdr->i_vhlen = IHLVER;
    iphdr->i_tos = 0;
    iphdr->i_len = hton16(plen);
    iphdr->i_id = 0;
    iphdr->i_ffo = 0;
    iphdr->i_ttl = 1;
    iphdr->i_prot = PROT_IGMP;
    iphdr->i_chksum = 0;
    iphdr->i_src = hton32(my_addr);
    iphdr->i_dest = hton32(dest);
    iphdr->i_chksum = ~incksum((uns16 *) iphdr, sizeof(*iphdr));
    ig_pkt = (IgmpPkt *) (iphdr+1);
    ig_pkt->ig_type = IGMP_MEMBERSHIP_QUERY;
    ig_pkt->ig_rsptim = max_response;
    ig_pkt->ig_chksum = 0;
    ig_pkt->ig_group = hton32(group);
    ig_pkt->ig_chksum = ~incksum((uns16 *) ig_pkt, sizeof(*ig_pkt));
    sys->sendpkt(iphdr, (int)index1());

    delete [] ((byte *) iphdr);
}

/* Receive an IGMP packet. If it hasn't been
 * associated with an interface by the kernel, we attempt
 * to do so by looking at the source address.
 * Then the packet is dispatched based on its IGMP
 * type.
 */

void OSPF::rxigmp(int phyint, InPkt *pkt, int plen)

{
    IgmpPkt *igmpkt;
    int rcv_err;
    int iphlen;
    InAddr src;
    PhyInt *phyp;
    InAddr group;

    rcv_err = 0;
    iphlen = (pkt->i_vhlen & 0xf) << 2;
    igmpkt = (IgmpPkt *) (((byte *) pkt) + iphlen);
    src = ntoh32(pkt->i_src);

    if (plen < ntoh16(pkt->i_len))
	rcv_err = IGMP_RCV_SHORT;
    else if (ntoh16(pkt->i_len) < (int) (iphlen + sizeof(IgmpPkt)))
	rcv_err = IGMP_RCV_SHORT;
    else if (incksum((uns16 *) igmpkt, sizeof(IgmpPkt)) != 0)
        rcv_err = IGMP_RCV_XSUM;
    else if (phyint == -1) {
        SpfIfc *rip;
        if ((rip = find_nbr_ifc(src)))
	    phyint = rip->if_phyint;
	else
	    rcv_err = IGMP_RCV_NOIFC;
    }
        
    if (rcv_err) {
	if (spflog(rcv_err, 3))
	    log(pkt);
	return;
    }

    if (!(phyp = (PhyInt *)phyints.find((uns32) phyint, 0))) {
	if (spflog(IGMP_RCV_NOIFC, 3))
	    log(pkt);
	return;
    }

    group = ntoh32(igmpkt->ig_group);
    if (spflog(IGMP_RCV, 1)) {
	log(igmpkt->ig_type);
	log(pkt);
    }

    // Dispatch on IGMP packet type
    switch (igmpkt->ig_type) {
      case IGMP_MEMBERSHIP_QUERY:
	phyp->received_igmp_query(src, group, igmpkt->ig_rsptim);
	break;
      case IGMP_V1MEMBERSHIP_REPORT:
	phyp->received_igmp_report(group, true);
	break;
      case IGMP_MEMBERSHIP_REPORT:
	phyp->received_igmp_report(group, false);
	break;
      case IGMP_LEAVE_GROUP:
	phyp->received_igmp_leave(group);
	break;
      default:		// Unexpected packet type
	break;
    }
}

/* Constructor for a group membership entry.
 */

GroupMembership::GroupMembership(InAddr group, int phyint)
: AVLitem(group, phyint), leave_tim(this), exp_tim(this), v1_tim(this)

{
    v1members = false;
}

/* Constructors for the group-membership timers.
 */

LeaveQueryTimer::LeaveQueryTimer(GroupMembership *g)
{
    gentry = g;
}

GroupTimeoutTimer::GroupTimeoutTimer(GroupMembership *g)
{
    gentry = g;
}

V1MemberTimer::V1MemberTimer(GroupMembership *g)
{
    gentry = g;
}

/* Process a received IGMP Host Membership Query.
 * Two things can result. We can let the send become
 * the querier, as a result of having a lower IP address.
 * Also, on queries for specific groups, we can
 * reset the group membership timeout.
 */

void PhyInt::received_igmp_query(InAddr src, InAddr group, byte max_response)

{
}

/* Process a received Host Membership Report. If
 * necessary, create a new group membership. In any case
 * reset the membership timeouts.
 */

void PhyInt::received_igmp_report(InAddr group, bool v1)

{
}

/* Group membership has timed out. Destroy the membership entry
 * and schedule a new group-membership-LSA.
 */

void GroupTimeoutTimer::action()

{
    InAddr group=gentry->index1();
    int phyint=gentry->index2();

    gentry->leave_tim.stop();
    gentry->exp_tim.stop();
    gentry->v1_tim.stop();
}

/* There are no longer an members speaking IGMP Version 1.
 */

void V1MemberTimer::action()

{
    gentry->v1members = false;
}

/* Process a received leave message. If the membership
 * exist, and isn't already in "Checking Membership" state,
 * start the Last Member Query timer.
 */

void PhyInt::received_igmp_leave(InAddr group)

{
}

/* Leave query timer. If more queries need to be sent, then do
 * so. Otherwise set the expiration timer to something short.
 */

void LeaveQueryTimer::action()

{
}
