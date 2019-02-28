
/* Declaration of OSPF timer classes.
 */

/* Implementation of a timer
 * Base class implements a single shot timer.
 * Derived class implements an interval timer
 */

class Timer : public PriQElt {
protected:
    int	active:1;
    uns32 period;		// Period in milliseconds
public:
    enum { SECOND = 1000};
    Timer() {
	active=false;
	period = 0;
    }

    static int random_period(int period);
    void stop();
    void restart(int millseconds=0);// Stop and start again
    inline int is_running();
    inline uns32 interval();
    int milliseconds_to_firing();
    virtual void start(int milliseconds, bool randomize=true);
    virtual void fire();
    virtual void action() = 0;
    virtual ~Timer();
};

// Inline functions
inline int Timer::is_running()
{
    return(active);
}
inline uns32 Timer::interval()
{
    return(active ? period : 0);
}

/* Implementation of a interval timer
 * When start, offsets initial interval in
 * an attempt to randomize. When fires, requeues
 * the timer automatically.
 */

class ITimer : public Timer {
public:
    virtual void start(int milliseconds, bool randomize=true);
    virtual void fire();
    virtual void action() = 0;
};

/* Time constants as used in OSPF timer queue. Time is divided
 * into seconds and microseconds since the program started.
 */

class SPFtime {
public:
    uns32 sec;		// Seconds since start
    uns32 msec;		// Milliseconds since start
};

bool	time_less(SPFtime &a, SPFtime &b);
void	time_add(SPFtime &a, SPFtime &b, SPFtime *result);
void	time_add(SPFtime &a, int milliseconds, SPFtime *result);
bool	time_equal(SPFtime &a, SPFtime &b);
int	time_diff(SPFtime &a, SPFtime &b);

