/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#ifndef ns_tskcomsink_h
#define ns_tskcomsink_h

#include <tclcl.h>

#include "agent.h"
#include "config.h"
#include "packet.h"
#include "resourceprovider.h"
#include "ip.h"
#include "rtp.h"
#include "cloudtask.h"
#include "bytecounter.h"
#include "provideroutagent.h"

class TskOutSink : public TcpSink, public ByteCounter {
public:
	TskOutSink();
	virtual ~TskOutSink();
	void addResourceProvider(ResourceProvider *newrp);
	virtual int command(int argc, const char*const* argv);
	virtual void recv(Packet* pkt, Handler*);

protected:
	int nlost_;
	int npkts_;
	int expected_;
	int bytes_;
	int seqno_;
	double last_packet_time_;
	ProviderOutAgent* poa_ ;

	ResourceProvider *res_provider_;
};

#endif 
