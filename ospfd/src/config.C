
#include "ospfinc.h"

/* Constructor for a ConfigItem. Enqueue onto the
 * doubly linked list of configuration data within the
 * main ospf class.
 */

ConfigItem::ConfigItem()

{
    updated = true;
    prev = 0;
    next = cfglist;
    cfglist = this;
    if (next)
	next->prev = this;
}

/* Destructor for a ConfigItem. Remove from the
 * doubly linked list of configuration items.
 */

ConfigItem::~ConfigItem()

{
    if (next)
	next->prev = prev;
    if (prev)
	prev->next = next;
    else
	cfglist = next;
}

/* A reconfiguration sequence has started.
 * Go through and mark all the configuration items
 * so that we can tell which don't get updated
 * (those we will either delete or return to their
 * default values).
 */

void OSPF::cfgStart()

{
    ConfigItem *item;

    for (item = cfglist; item; item = item->next)
	item->updated = false;
}

/* A reconfiguration sequence has ended.
 * Those configuration items that have not been
 * updated are either deleted or returned to their
 * default values.
 * We have to be a little careful traversing the list
 * of configured items because the removal of one
 * item by ConfigItem::clear_config() may cause
 * other items to get deleted further down the list.
 */

void OSPF::cfgDone()

{
    ConfigItem **prev;
    ConfigItem *item;

    for (prev = &cfglist; (item = *prev); ) {
	if (!item->updated)
	    item->clear_config();
	if (item == *prev)
	    prev = &item->next;
    }
}
