
/*
 * Link state database aging.
 * When an LSA is received/originated, it is placed in a bin
 * permanently. Bin granularity is one second. Then, as the database
 * aging time fires, the bins requiring attention (those at
 * MinLSInterval, CheckAge, LSRefreshTime and MaxAge) shift.
 * This avoids having to actually update ages in LSAs.
 *
 * LSA is aging if both it is a) in the database and b) it does not
 * have its DoNotAge bit set and c) ls_age is not equal to MaxAge.
 * Including DoNotAge LSAs in the age
 * bins allows us to use the same mechanims for when a) performing
 * CheckAge processing on DoNotAge LSAs and b) when flushing DoNotAge
 * LSAs after their originator has been unreachable for MaxAge. DoNotAge
 * LSAs are always inserted into the Age0Bin, regardless of their received
 * LS age value,
 */

inline	uns16 Age2Bin(age_t x)
{
    if (x <= LSA::Bin0)
	return (LSA::Bin0 - x);
    else
	return (MaxAge+1 + LSA::Bin0 - x);
}

inline	age_t Bin2Age(uns16 x)
{
    return((age_t) Age2Bin(x));
}

inline	int LSA::is_aging()
{
    if (!valid())
	return(false);
    else if ((lsa_rcvage & DoNotAge) != 0)
	return(false);
    else if (lsa_rcvage == MaxAge)
	return(false);
    else
	return(true);
}

inline	age_t LSA::lsa_age()
{
    return (is_aging() ? Bin2Age(lsa_agebin) : lsa_rcvage); 
}

inline	age_t LSA::since_received()
{
    if (is_aging())
	return (lsa_age() - lsa_rcvage);
    else
	return (Bin2Age(lsa_agebin));
}
