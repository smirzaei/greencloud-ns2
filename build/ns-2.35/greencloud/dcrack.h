#ifndef ns_dcrack_h
#define ns_dcrack_h

#include <vector>
#include "dchost.h"
#include "queue-monitor.h"
#include "timer-handler.h"

class DcRack : public TclObject, public TimerHandler  {
public:
	DcRack();
	virtual ~DcRack();
	virtual int command(int argc, const char*const* argv);

	int rack_id_;			/**< rack ID */
	double stat_interval;	/**< interval for updating load statistics */
	double link_load;		/**< last measured average load at the uplink */
	double uplink_B;		/**< uplink bandwidth capacity in Gb/s */

protected:
	vector <DcHost*> hosts_list_;				/**< list of hosts in a rack */
	vector <QueueMonitor*> qmon_uplink_list; 	/**< uplink queue monitors */

	/* Stats */
#if defined(HAVE_INT64)
	int64_t breceived_;
	int64_t breceived_old_;
#else /* no 64-bit integer */
	int breceived_;				/**< bytes received */
	int breceived_old_;
#endif
	void updatestats();
	virtual void expire(Event* e);
	void addHost(DcHost *hst);

};

#endif
