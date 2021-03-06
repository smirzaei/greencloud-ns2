/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#ifndef lint
static const char rcsid[] =
		"@(#) $Header: /cvsroot/nsnam/ns-2/apps/tskagent.cc,v 1.21 2005/08/26 05:05:28 tomh Exp $ (Xerox)";
#endif

#include "tskagent.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
//#include "random.h"
#include "address.h"
#include "rtp.h"
#include "ip.h"


static class TskComAgentClass : public TclClass {
public:
	TskComAgentClass() : TclClass("Agent/TskComAgent") {}
	TclObject* create(int, const char*const*) {
		return (new TskComAgent());
	}
} class_tsk_comagent;

TskComAgent::TskComAgent() : Agent(PT_UDP), seqno_(-1)
{
	bind("packetSize_", &size_);	
}

TskComAgent::TskComAgent(packet_t type) : Agent(type)
{
	bind("packetSize_", &size_);
}

void TskComAgent::sendmsg(int nbytes, AppData* data, void * pTaskObj, const char* flags)
{
	Packet *p; 
	int n;

	assert (size_ > 0);

	n = nbytes / size_;
	int initialseqno = seqno_;

	if (nbytes == -1) {
		printf("Error: sendmsg() for Tsk should not be -1\n");
		return;
	}	

	// If they are sending data, then it must fit within a single packet.
	if (data && nbytes > size_) {
		printf("Error: data greater than maximum Tsk packet size\n");
		return;
	}

	double	local_time = Scheduler::instance().clock();
	while (n-- > 0) {		
		p = allocpkt();
		hdr_cmn::access(p)->size() = size_;
		hdr_cmn::access(p)->pt_obj_addr() = 0;
		if (initialseqno == seqno_){
			/* Add pointer to TaskObj for the first packet in the bulk */
			hdr_cmn::access(p)->pt_obj_addr() = pTaskObj;
		}
		hdr_rtp* rh = hdr_rtp::access(p);
		rh->flags() = 0;
		rh->seqno() = ++seqno_;
		hdr_cmn::access(p)->timestamp() = 
				(u_int32_t)(SAMPLERATE*local_time);
		if (flags && (0 ==strcmp(flags, "NEW_BURST")))
			rh->flags() |= RTP_M;
		p->setdata(data);
		target_->recv(p);	
	}
	n = nbytes % size_;
	if (n > 0) {
		p = allocpkt();
		hdr_cmn::access(p)->size() = n;
		hdr_cmn::access(p)->pt_obj_addr() = 0;
		if (initialseqno == seqno_){
			/* Add pointer to TaskObj for the first packet in the bulk */
			hdr_cmn::access(p)->pt_obj_addr() = pTaskObj;
		}
		hdr_rtp* rh = hdr_rtp::access(p);
		rh->flags() = 0;
		rh->seqno() = ++seqno_;
		hdr_cmn::access(p)->timestamp() = 
				(u_int32_t)(SAMPLERATE*local_time);
		if (flags && (0 == strcmp(flags, "NEW_BURST")))
			rh->flags() |= RTP_M;
		p->setdata(data);
		target_->recv(p);
	}
	idle();
}
void TskComAgent::recv(Packet* pkt, Handler*)
{
	if (app_ ) {
		// If an application is attached, pass the data to the app
		hdr_cmn* h = hdr_cmn::access(pkt);
		app_->process_data(h->size(), pkt->userdata());
	} else if (pkt->userdata() && pkt->userdata()->type() == PACKET_DATA) {
		// otherwise if it's just PacketData, pass it to Tcl
		//
		// Note that a Tcl procedure Agent/Udp recv {from data}
		// needs to be defined.  For example,
		//
		// Agent/Udp instproc recv {from data} {puts data}

		PacketData* data = (PacketData*)pkt->userdata();

		hdr_ip* iph = hdr_ip::access(pkt);
		Tcl& tcl = Tcl::instance();
		tcl.evalf("%s process_data %d {%s}", name(),
				iph->src_.addr_ >> Address::instance().NodeShift_[1],
				data->data());
	}
	Packet::free(pkt);
}


int TskComAgent::command(int argc, const char*const* argv)
{
	//	Tcl& tcl = Tcl::instance();
	// 	if (argc == 4) {
	// 		if (strcmp(argv[1], "send") == 0) {
	// 			PacketData* data = new PacketData(1 + strlen(argv[3]));
	// 			strcpy((char*)data->data(), argv[3]);
	// 			sendmsg(atoi(argv[2]), data, 0);
	// 			return (TCL_OK);
	// 		}
	// 	} else if (argc == 5) {
	// 		if (strcmp(argv[1], "sendmsg") == 0) {
	// 			PacketData* data = new PacketData(1 + strlen(argv[3]));
	// 			strcpy((char*)data->data(), argv[3]);
	// 			sendmsg(atoi(argv[2]), data, 0, argv[4]);
	// 			return (TCL_OK);
	// 		}
	// 	}

	return (Agent::command(argc, argv));
}
