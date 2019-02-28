
/* Class implementing system functions, such as time of day.
 */

class OspfSysCalls {
public:
    InPkt *getpkt(uns16 len);
    void freepkt(InPkt *pkt);
    OspfSysCalls();
    virtual ~OspfSysCalls();

    virtual void sendpkt(InPkt *pkt, int phyint, InAddr gw=0)=0;
    virtual void sendpkt(InPkt *pkt)=0;
    virtual bool phy_operational(int phyint)=0;
    virtual void phy_open(int phyint)=0;
    virtual void phy_close(int phyint)=0;
    virtual void join(InAddr group, int phyint)=0;
    virtual void leave(InAddr group, int phyint)=0;
    virtual void ip_forward(bool enabled)=0;
    virtual void set_multicast_routing(bool on)=0;
    virtual void set_multicast_routing(int phyint, bool on)=0;
    virtual void rtadd(InAddr, InMask, MPath *, MPath *, bool)=0; 
    virtual void rtdel(InAddr, InMask, MPath *ompp)=0;
    virtual void upload_remnants()=0;
    virtual void monitor_response(struct MonMsg *, uns16, int, int)=0;
    virtual char *phyname(int phyint)=0;
    virtual void sys_spflog(int msgno, char *msgbuf)=0;
    virtual void store_hitless_parms(int, int, struct MD5Seq *) = 0;
    virtual void halt(int code, char *string)=0;
};

/* Class used to indicate MD5 sequence numbers in use
 * on a particular interface.
 */

struct MD5Seq {
    int phyint;
    InAddr if_addr;
    uns32 seqno;
};


extern OspfSysCalls *sys;
extern SPFtime sys_etime;


