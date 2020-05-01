/*
 * vmmigration.cc
 *
 *  @date Nov 13, 2013
 *  @author Guzek:Mateusz
 */

#include "vmmigration.h"

static class VmMigrationClass : public TclClass {
public:
	VmMigrationClass() : TclClass("VmMigration") {}
	TclObject* create(int argc, const char*const*argv) {
		return (new VmMigration());
	}
} class_vmmigration;

VmMigration::VmMigration() {

}

void VmMigration::initalizeMigration(VM* vm,ResourceProvider* target) {

	migrated_vm_ = vm;
	target_ =  target;

	this->size_ = vm->size_;
	this->isTask = false;
	this->isVM = false;
	res_demands.clear();
	res_demands = std::vector<ResDemand *>(vm->res_demands.size(),NULL);

	std::vector <ResDemand*>::iterator iter;
	std::vector <ResDemand*>::iterator iter2;
	for (iter = vm->res_demands.begin(), iter2=res_demands.begin(); iter!=vm->res_demands.end(); iter++,iter2++)
	{
		(*iter2)=new ResDemand(*(*iter),NULL);
	}


	// Create new migration sink and source;

	Tcl& tcl = Tcl::instance();
	// Create communication source and sink:
	tcl.evalf("set vmmigrationsource_($next_migration_id) [new Agent/TCP/ProvOutAgent]");
	tcl.evalf("set vmmigrationsink_($next_migration_id) [new Agent/TCPSink/VmMigrationSink]");

	//Attach source and sink in network topology
	tcl.evalf("$ns attach-agent $servers_(%d) $vmmigrationsource_($next_migration_id)",migrated_vm_->getHost()->id_);
	tcl.evalf("$ns attach-agent $servers_(%d) $vmmigrationsink_($next_migration_id)",target_->id_);
	tcl.evalf("$ns connect $vmmigrationsource_($next_migration_id) $vmmigrationsink_($next_migration_id)");


	//Attach source and sink to host to track network interface utilization
	tcl.evalf("$hosts_(%d) attach-vm-mig-source $vmmigrationsource_($next_migration_id)",migrated_vm_->getHost()->id_);
	tcl.evalf("$hosts_(%d) attach-vm-mig-sink   $vmmigrationsink_($next_migration_id)",target_->id_);



	//Set source and destination in migration object itself:
	tcl.evalf("$vmmigration_($next_migration_id) set-source $vmmigrationsource_($next_migration_id)");
	tcl.evalf("$vmmigration_($next_migration_id) set-sink   $vmmigrationsink_($next_migration_id)");



	tcl.evalf("incr next_migration_id");

}

VmMigration::~VmMigration() {
	//  TODO: How about memory management:
	// 	delete vm_migration_sender_;
	// 	delete vm_migration_sink_;

}

void VmMigration::startMigration(){
	migrated_vm_->getHost()->updateEnergyAndConsumption();
	target_->updateEnergyAndConsumption();

	if(target_->tryToAllocate(this)){
		//		std::cerr << "Migration object successfully allocated on target.\n";
	} else {
		std::cerr << "Error: Migration object NOT successfully allocated on target.\n";
		return;
	}

	// Create, attach and allocate migration agents: source and sink
	//TODO: change mig_s to USED capacity instead of total... (but right now there is no overhead of VM, so it could be 0 bytes for idle machine.)
	int mig_s = migrated_vm_->getTotalCap(Memory); // migration size
	int p_s = vm_migration_sender_->getPacketSize(); // packet size
	vm_migration_sink_->seq_expected_ = mig_s % p_s ? ( mig_s / p_s ) + 1: mig_s / p_s; // Ceiling of the number of the last packet
	vm_migration_sender_->sendmsg(mig_s, (void*) 1);
}

void VmMigration::finalizeMigration(){
	migrated_vm_->getHost()->updateEnergyAndConsumption();
	target_->updateEnergyAndConsumption();

	Tcl& tcl = Tcl::instance();

	tcl.evalf("$hosts_(%d) detach-vm-mig-source $vmmigrationsource_(%d)",migrated_vm_->getHost()->id_,id_);
	tcl.evalf("$hosts_(%d) detach-vm-mig-sink   $vmmigrationsink_(%d)",target_->id_,id_);

	tcl.evalf("$ns detach-agent $servers_(%d) $vmmigrationsource_(%d)",migrated_vm_->getHost()->id_,id_);
	tcl.evalf("$ns detach-agent $servers_(%d) $vmmigrationsink_(%d) ",target_->id_,id_);


	target_->releaseAllocation(this);

	ResourceProvider* source = migrated_vm_->getHost();
	source->removeVM(migrated_vm_);
	//	Re-attach tasks sinks
	tcl.evalf("$ns detach-agent $servers_(%d) $vmtsksink_(%d) ",source->id_,migrated_vm_->id_);
	tcl.evalf("$ns attach-agent $servers_(%d) $vmtsksink_(%d) ",target_->id_,migrated_vm_->id_);

	//	Re-attach tasks com agents:
	tcl.evalf("$ns detach-agent $switch_C1_([expr %d/($top(NServers)/$NTSwitches)]) $vmtskcomagnt_C_(%d)",source->id_,migrated_vm_->id_);
	tcl.evalf("$ns attach-agent $switch_C1_([expr %d/($top(NServers)/$NTSwitches)]) $vmtskcomagnt_C_(%d)",target_->id_,migrated_vm_->id_);
	//  Connect com agents with sinks:
	tcl.evalf("$ns connect $vmtskcomagnt_C_(%d) $vmtsksink_(%d)",migrated_vm_->id_,migrated_vm_->id_);

	//  Re-attach tasks output agent:
	tcl.evalf("$ns detach-agent $servers_(%d) $vmtskoutputagent_(%d) ",source->id_,migrated_vm_->id_);
	tcl.evalf("$ns attach-agent $servers_(%d) $vmtskoutputagent_(%d) ",target_->id_,migrated_vm_->id_);
	//  Re-attach tasks output sink:
	tcl.evalf("$ns detach-agent $switch_C1_([expr %d/($top(NServers)/$NTSwitches)]) $vmtskoutputsink_(%d)",source->id_,migrated_vm_->id_);
	tcl.evalf("$ns attach-agent $switch_C1_([expr %d/($top(NServers)/$NTSwitches)]) $vmtskoutputsink_(%d)",target_->id_,migrated_vm_->id_);
	//Connect:
	tcl.evalf("$ns connect $vmtskoutputagent_(%d) $vmtskoutputsink_(%d)",migrated_vm_->id_,migrated_vm_->id_);


	tcl.evalf("$vms_(%d) attach-agent $vmtskoutputagent_(%d)",migrated_vm_->id_,migrated_vm_->id_);

	if(target_->addVM(migrated_vm_)){
//				std::cerr << "Migrated VM successfully allocated on target.\n";
	} else {
		std::cerr << "Error: Migration object NOT successfully allocated on target.\n";
		return;
	}

	// 	Migration finalization finished.
	//	IFF the migration object is no longer needed:
	delete this; // Nothing related to migration object after this point!
}

int VmMigration::command(int argc, const char*const* argv){
	if (argc == 3) {
		if (strcmp(argv[1], "set-sink") == 0) {
			VmMigrationSink *vms = dynamic_cast<VmMigrationSink*> (TclObject::lookup(argv[2]));
			if(vms){
				vm_migration_sink_ = vms;
				vm_migration_sink_->setVmMigration(this);
				return (TCL_OK);
			}
			return (TCL_ERROR);
		} 	else if (strcmp(argv[1], "set-source") == 0) {
			TcpAgent *vma = dynamic_cast<TcpAgent*> (TcpAgent::lookup(argv[2]));
			if(vma){
				vm_migration_sender_=vma;
				return (TCL_OK);
			}
			return (TCL_ERROR);
		} else if (strcmp(argv[1], "set-id") == 0) {
			char* stat;
			int num = strtol(argv[2], &stat, 10);
			if (!*stat){
				id_=num;
				return (TCL_OK);
			}
			else {
				return (TCL_ERROR);
			}
		}
	}
	return TCL_ERROR;
}

