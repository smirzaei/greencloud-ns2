/*
 * vm.cc
 *
 *  @date Jun 12, 2013
 *  @author Guzek:Mateusz
 */

#include "vm.h"

static class VMClass : public TclClass {
public:
	VMClass() : TclClass("VM") {}
	TclObject* create(int argc, const char*const*argv) {
		return (new VM());
	}
} class_vm;

VM::VM() {

	/* It should be always false for VMs */
	isTask = false;
	/* It should be always true for VMs */
	isVM = true;

	bind("id_", &id_);
	bind("ntasks_", &ntasks_);
	bind("currentLoad_", &currentLoad_);
	bind("currentLoadMem_", &currentLoadMem_);
	bind("currentLoadStor_", &currentLoadStor_);
	bind("tskFailed_", &tskFailed_);
	bind("eDVFS_enabled_", &eDVFS_enabled_);			/* ON when DVFS is enabled */
	state = Ready;

}

VM::~VM() {

}


vm_state VM::getVmState() {return state;};

void VM::setHost(ResourceProvider * newHost){host = newHost;};





void VM::addResource(DcResource* res){

	ResourceProvider::addResource(res);
	res_demands.push_back(new ResDemand(*res,res));

}

int VM::command(int argc, const char*const* argv)
{

	if (argc == 2) {
		if (strcmp(argv[1], "start") == 0) {

			state = Running;
			return (TCL_OK);
		} else if (strcmp(argv[1], "stop") == 0) {

			state = Stopped;
			return (TCL_OK);
		} else if (strcmp(argv[1], "print") == 0) {
			/* print general info */
			print();
			return (TCL_OK);
		}
	}
	return (ResourceProvider::command(argc, argv));
}

void VM::updateMIPS(){
	return;
}

void VM::updateEnergyAndConsumption(){
	if(host!= NULL){
		host->updateEnergyAndConsumption();
	} else {
		std::cerr << "ERROR: Task is allocated on an unallocated VM!\n";
	}
	return;
}

void VM::print(){
	std::cout << "VM:\t";
	std::cout << id_;
	std::cout << "\n";
	if(host!=NULL){
		std::cout << "Hosted on" << host->id_;
	} else {
		std::cout << "Not hosted";
	}
	std::cout << "\n";
	std::cout << "Resources provisions:\n";
	std::vector <std::vector<DcResource*> >::iterator iter_out;
	for(iter_out = resource_list.begin(); iter_out!=resource_list.end() ;iter_out++){
		std::vector <DcResource*>::iterator iter;
		for (iter = iter_out->begin(); iter!=iter_out->end(); iter++)
		{
			(*iter)->print();
		}
	}
	std::cout << "Resources demands:\n";
	std::vector<ResDemand*>::iterator iter_dem;
	for(iter_dem = res_demands.begin(); iter_dem!=res_demands.end() ;iter_dem++){
		(*iter_dem)->print();
	}
	std::cout << "\n";

}

void VM::printTasklist(){
	std::vector<CloudTask *>::iterator iter;
	std::cout <<"VM " <<this->id_ << "\n";

	ResourceProvider::printTasklist();
}

