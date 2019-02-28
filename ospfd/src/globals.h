
/*
 * Global data used by the OSPF implementation
 */

extern INtbl *inrttbl;	// Global instance of the IP routing table
extern FWDtbl *fa_tbl;	// Forwarding address table
extern INrte *default_route; // The default routing entry (0/0)
extern ConfigItem *cfglist; // List of configurable classes
