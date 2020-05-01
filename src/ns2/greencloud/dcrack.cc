/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#include "dcrack.h"

static class DcRackClass : public TclClass {
public:
	DcRackClass() : TclClass("DcRack") {}
	TclObject* create(int argc, const char*const*argv) {
		return (new DcRack());
	}
} class_dcrack;

DcRack::DcRack(): rack_id_(0), stat_interval (0.0), breceived_(0), breceived_old_(0)
{
	bind("rack_id_", &rack_id_);
	bind("breceived_", &breceived_);
	bind("stat_interval", &stat_interval);
	bind("uplink_B", &uplink_B);
}

DcRack::~DcRack()
{
	hosts_list_.~vector();
	qmon_uplink_list.~vector();
}

void DcRack::updatestats(){

	vector <QueueMonitor*>::iterator iter;
	breceived_old_ = breceived_;
	breceived_ = 0;

	/* Update statistics on the number of bytes received */
	for (iter = qmon_uplink_list.begin(); iter!=qmon_uplink_list.end(); iter++)
	{
		breceived_ += (*iter)->bdepartures_tot();
	}
	if(breceived_-breceived_old_<0){
		std::cerr << "ERROR in DcRack.cc: Byte counter overflow, consider resetting bdepartures_tot in qmon";
		abort();
	}
	link_load  = (breceived_-breceived_old_)*8/(stat_interval*uplink_B);

	//TODO: fix the problem
	if(link_load > 1 ){
		link_load=1;
	}
//	std:cerr << "Rack " << this->rack_id_ << " link load\t" << link_load <<"\n";
}

void  DcRack::expire(Event* e){

	updatestats();
	this->resched(stat_interval);
}

void DcRack::addHost(DcHost *hst){
	hosts_list_.push_back(hst);
	hst->rack_=this;
}

int DcRack::command(int argc, const char*const* argv)
{
	if (argc == 2) {
		if (strcmp(argv[1], "update-stats") == 0) {
			updatestats();
			return (TCL_OK);
		} else if (strcmp(argv[1], "start") == 0) {
			expire(new Event());
			return (TCL_OK);
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "add-dchost") == 0) {
			DcHost *hst = dynamic_cast<DcHost*> (TclObject::lookup(argv[2]));
			if(hst){
				addHost(hst);
				return (TCL_OK);
			}
			return (TCL_ERROR);
		}
		else if (strcmp(argv[1], "add-uplink-qmon") == 0) {
			QueueMonitor *uplinkqmon = dynamic_cast<QueueMonitor*> (TclObject::lookup(argv[2]));
			if(uplinkqmon){
				qmon_uplink_list.push_back(uplinkqmon);

				return (TCL_OK);
			}
			return (TCL_ERROR);
		}
	}
	return (DcRack::command(argc, argv));
}
