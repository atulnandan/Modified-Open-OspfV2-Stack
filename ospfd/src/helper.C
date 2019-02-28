
#include "ospfinc.h"
#include "monitor.h"
#include "system.h"
#include "ifcfsm.h"
#include "nbrfsm.h"
#include "phyint.h"

/* This file contains the routines implementing
 * helper mode for hitless restart.
 */

/* If we are in helper mode, we should be advertising
 * the neighbor as fully adjacent, even if it isn't.
 */

bool SpfNbr::adv_as_full()

{
    if (n_state == NBS_FULL)
        return(true);
    if (n_helptim.is_running())
        return(true);
    return(false);
}

/* If we are helping a neighbor perform a hitless restart,
 * its timer should be running.
 */

bool SpfNbr::we_are_helping()

{
    return(n_helptim.is_running());
}

/* If the helper timer expires, we exit helper mode with
 * a reason of "timeout".
 */

void HelperTimer::action()

{
    np->exit_helper_mode("Timeout");
}

/* When exiting helper mode, we need to reoriginate
 * router-LSAs, network-LSAs, and rerun the Designated Router
 * calculation for the associated interface.
 * If the neighbor we were helping is DR, make sure it
 * stays DR until we receive its next Hello Packet.
 */

void SpfNbr::exit_helper_mode(char *reason, bool actions)

{
    if (ospf->spflog(LOG_HELPER_STOP, 5)) {
        ospf->log(this);
	ospf->log(":");
	ospf->log(reason);
    }
    n_helptim.stop();
    n_ifp->if_helping--;
    ospf->n_helping--;
    /* If neighbor is not yet full again, do the
     * processing that should have been done when the
     * neighjbor initially went out of FULL state,
     */
    if (n_state != NBS_FULL) {
        SpfArea *tap;
    }
    // Neighbor stays DR until next Hello
    else if (n_ifp->if_dr_p == this)
        n_dr = n_ifp->if_dr;
    // Caller may take actions itself
	ospf->logflush();
	ospf->log("SpfNbr::exit_helper_mode: actions:");
	ospf->log(actions);
    if (actions) {
        // Recalculate Designated Router
        n_ifp->run_fsm(IFE_NCHG);
	// Re-originate router-LSA
	n_ifp->area()->rl_orig();
	ospf->logflush();
	ospf->log("SpfNbr::exit_helper_mode: Calling nl_orig().");
	// And network-LSA
	n_ifp->nl_orig(false);
    }
}

