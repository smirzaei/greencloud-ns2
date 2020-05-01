/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#ifndef lint
static const char rcsid[] =
		"@(#) $Header: /cvsroot/nsnam/ns-2/tools/TskOutSink.cc,v 1.18 2000/09/01 03:04:06 haoboy Exp $ (LBL)";
#endif

#include <tclcl.h>

#include "agent.h"
#include "config.h"
#include "packet.h"
#include "ip.h"
#include "rtp.h"
#include "bytecounter.h"
#include "tskoutsink.h"
#include "dchost.h"


static class TskOutSinkClass : public TclClass {
public:
	TskOutSinkClass() : TclClass("Agent/TCPSink/TskOutSink") {}
	TclObject* create(int, const char*const*) {
		return (new TskOutSink());
	}
} class_tsk_outsink;

TskOutSink::TskOutSink() : TcpSink(new Acker()), poa_(NULL)
{	

}

TskOutSink::~TskOutSink()
{
	res_provider_ = NULL;
}

void TskOutSink::recv(Packet* pkt, Handler* h)
{
	/* Get TskObject and start its execution */
	CloudTask *recvTskObj = (CloudTask*)hdr_cmn::access(pkt)->pt_obj_addr();
	if (recvTskObj) { /* Valid pointer and can be executed */
//		std::cout << "Task id:" << recvTskObj->id_ << "exits the DC at: "<< Scheduler::instance().clock() <<"\n";
		recvTskObj->info_->finalizeDcExitTime(Scheduler::instance().clock());
		poa_->tryToSend();
		recvTskObj->info_->getResourceProvider()->getRootHost()->updateEnergyAndConsumption();
	}
	TcpSink::recv(pkt,h);
}


int TskOutSink::command(int argc, const char*const* argv)
{
	if (argc == 2) {
		if (strcmp(argv[1], "clear") == 0) {
			expected_ = -1;
			return (TCL_OK);
		}
	}
	if (argc == 3) {
		if (strcmp(argv[1], "connect-tskoutagent") == 0) {
			ProviderOutAgent *poa =(ProviderOutAgent*)(TclObject::lookup(argv[2]));
			if(poa){
					poa_ = poa;
				return (TCL_OK);
			}
			return (TCL_ERROR);
		}
	}

	return (Agent::command(argc, argv));
}


