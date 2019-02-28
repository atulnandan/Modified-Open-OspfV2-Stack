/*
 *   OSPFD routing simulation controller
 *   Linux-specific portion
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/param.h>
#include <unistd.h>
#include <tcl.h>
#include <tk.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include "../src/ospfinc.h"
#include "../src/monitor.h"
#include "../src/system.h"
#include "tcppkt.h"
#include "linux.h"
#include "sim.h"
#include "simctl.h"
#include <time.h>

// External references
extern SimCtl *sim;

/* Start a simulated router.
 * Router uses two sockets, one for monitoring and
 * one to receive OSPF packets.
 * The former we allocate so that they are in well known
 * places, the latter the router allocates.
 */
/* ATUL */
int StartRouter(ClientData, Tcl_Interp *, int, const char *argv[])
{
    return(TCL_OK);
}

int StartRouterCopy(char *args[])

{
    char server_port[8];
	/* ATUL */
    //sprintf(server_port, "%d", (int) sim->assigned_port);
    sprintf(server_port, "%d", (int) sim->get_assigned_port());
    // Child is now a simulated router
    if (fork() == 0) {
        execlp("ospfd_sim", "ospfd_sim", args[0], server_port, 0);
	// Should never get this far
	perror("execl ospfd_sim failed");
	exit(1);
    }

    //return(TCL_OK);
    return(1);
}
