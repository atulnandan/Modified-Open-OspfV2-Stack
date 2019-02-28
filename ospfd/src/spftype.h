
/* Definitions of commonly used OSPF-specific
 * data types
 */

typedef uns32 rtid_t;		// Router ID
typedef uns32 lsid_t;		// Link State ID
typedef uns32 aid_t;		// Area IDs
typedef	uns32 autyp_t;		// Authentication type
				// Fields in link state header
typedef	uns16 age_t;		// LS age, 16-bit unsigned
typedef	int32 seq_t;		// LS Sequence Number, 32-bit signed
typedef	uns16 xsum_t;		// LS Checksum, 16-bit unsigned
