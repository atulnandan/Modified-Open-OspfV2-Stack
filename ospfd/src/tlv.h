
/* This file contains the class used to build and parse
 * TLVs within the body of OSPF LSAs.
 */

class TLVbuf {
    byte *buf;
    int blen;
    TLV *current;

    int tlen(int vlen);
    TLV *reserve(int vlen);
  public:
    // Parse routines
    TLVbuf(byte *body, int blen);
    bool next_tlv(int & type);
    bool get_int(int32 & val);
    bool get_short(uns16 & val);
    bool get_byte(byte & val);
    char *get_string(int & len);
    // Build routines
    TLVbuf();
    void reset();
    void put_int(int type, int32 val);
    void put_short(int type, uns16 val);
    void put_byte(int type, byte val);
    void put_string(int type, char *str, int len);
    inline byte *start();
    inline int length();
};

inline byte *TLVbuf::start()
{
    return(buf);
}
inline int TLVbuf::length()
{
    return(((byte *) current) - buf);
}

