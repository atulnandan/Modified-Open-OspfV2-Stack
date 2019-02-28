#include <stdio.h>
#include <stddef.h>
#if 0
#include "avl.h"
#include "machdep.h"
#include "spftype.h"
#include "ip.h"
#include "arch.h"
#include "lshdr.h"
#include "lsa.h"
#include "priq.h"
#include "timer.h"
#include "rte.h"
typedef unsigned short uns16;
typedef unsigned int InAddr;
#endif

#include "ospfinc.h"
 
class AddressMap : public AVLitem {
  public:
    uns16 port;
    InAddr home;
    AddressMap(InAddr, InAddr);
};

/* Contructor for an entry in the address map, that maps
 * unicast addresses to group addresses to simulate
 * data-link address translation.
 */

AddressMap::AddressMap(InAddr addr, InAddr home) : AVLitem(addr, home)

{

}

//PriQ timerq;
//SPFtime sys_etime;

int main()
{
	AVLtree	address_map;	//IP Address to group mapping
	AddressMap	*entry = NULL;

	//First Entry
	entry = new AddressMap(0xa000001, 0xa000001);
	address_map.add(entry);
	entry->port = 5033;
	entry->home = 0xa000001;

	//Next Entry
	entry = new AddressMap(0xa000008, 0xa000008);
	address_map.add(entry);
	entry->port = 5044;
	entry->home = 0xa000008;

	//Next Entry
	entry = new AddressMap(0xa000009, 0xa000009);
	address_map.add(entry);
	entry->port = 5055;
	entry->home = 0xa000009;

	//Next Entry
	entry = new AddressMap(0xa000002, 0xa000002);
	address_map.add(entry);
	entry->port = 5066;
	entry->home = 0xa000002;

	entry = (AddressMap *)address_map.find(0xa000009, 0xa000009);
	if(entry)
	{
		printf("Deleted index:0%x", entry->index1());
		address_map.remove(entry);
	}
	printf("\nDone.\n");
	
	PPIfc *ifc;
	ifc = new PPIfc(0xa000001,1);
	ifc->start_hellos();
	return 0;
}

