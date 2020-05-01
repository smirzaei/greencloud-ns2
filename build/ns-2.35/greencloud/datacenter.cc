/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#ifndef lint
static const char rcsid[] =
		"@(#) $Header: /cvsroot/nsnam/ns-2/common/datacenter.cc,v 1.43 $";
#endif

#include <stdlib.h>
#include "datacenter.h"
#include "cloudtask.h"

static class DataCenterClass : public TclClass {
public:
	DataCenterClass() : TclClass("DataCenter") {}
	TclObject* create(int argc, const char*const*argv) {
		return (new DataCenter());
	}
} class_datacenter;

DataCenter::DataCenter() : tskSubmitted_ (0),tskFailed_ (0), avgLoad_(0.0), avgLoadMem_(0.0),avgLoadStor_(0.0), avgPower_(0.0), dcScheduler(NULL),  numHostTskAgents_(0), numVmTskAgents_(0),scheduleOnVms_(false)

{
	bind("tskSubmitted_", &tskSubmitted_);
	bind("avgLoad_", &avgLoad_);
	bind("avgLoadMem_", &avgLoadMem_);
	bind("avgLoadStor_", &avgLoadStor_);
	bind("avgPower_", &avgPower_);
	bind("tskFailed_", &tskFailed_);
//	dcScheduler = new GreenScheduler();
}


DataCenter::~DataCenter()
{
	clear();
}

void DataCenter::clear(){

	//TODO: this is the stub of the function, it should be finished to fully clear a DataCenter object.
	host_list.clear();
	host_agent_list.clear();
	vector <ResourceSpec*>::iterator rs;
	for(rs = resource_specification_list.begin();rs!= resource_specification_list.end(); rs++){
		delete (* rs);
	}
	resource_specification_list.clear();
	vector <ResourceProvider*>::iterator iter;
	for(iter = host_list.begin();iter!= host_list.end(); iter++){
		delete (* iter);
	}
	for(iter = vm_list.begin();iter!= vm_list.end(); iter++){
		delete (* iter);
	}
	host_list.clear();
	vm_list.clear();
}

void DataCenter::setVmScheduling(bool scheduleOnVms){
	this->scheduleOnVms_ = scheduleOnVms;
}

void DataCenter::addHostPointer(DcHost *newhst)
{	
	host_list.push_back(newhst);	
}

void DataCenter::addVmPointer(VM* newvm){
	vm_list.push_back(newvm);
}

void DataCenter::addHostTaskAgentPointer(TskComAgent *newagnt)
{
	newagnt->set_pkttype(PT_CLOUD_USER);
	host_agent_list.push_back(newagnt);
}

void DataCenter::addVmTaskAgentPointer(TskComAgent *newagnt)
{
	newagnt->set_pkttype(PT_CLOUD_USER);
	vm_agent_list.push_back(newagnt);
}

void DataCenter::addResourceSpecificationPointer(ResourceSpec *newresspec)
{
	resource_specification_list.push_back(newresspec);
}

void DataCenter::addPModelPointer(PowerModel* newPModel){
	power_model_list.push_back(newPModel);
}


void DataCenter::addVirtualResourceSpecificationPointer(ResourceSpec *newresspec)
{
	virt_resource_specification_list.push_back(newresspec);
}

int DataCenter::initiallyConfigureVms()
{
	char output[100];
	// Create the output for the Tcl interpreter.
	sprintf(output, "puts \"Configuring VMs...\"");
	Tcl& tcl = Tcl::instance();
	tcl.eval(output);
	return 0;
}

int DataCenter::setScheduler(const char * scheduler_name){
	if(dcScheduler!=NULL){
	delete dcScheduler;
	}
	std::cout << "Selected DC scheduler: " << scheduler_name << "\n";
	if(strcmp(scheduler_name, "Green") == 0){
		dcScheduler = new GreenScheduler();
		return 1;
	} else if(strcmp(scheduler_name, "RoundRobin") == 0){
		dcScheduler = new RoundRobinsScheduler();
		return 1;
	} else if(strcmp(scheduler_name, "Random") == 0){
		dcScheduler = new RandomScheduler();
		return 1;
	} else if(strcmp(scheduler_name, "RandDENS") == 0){
		dcScheduler = new RandDENS();
		return 1;
	} else if(strcmp(scheduler_name, "BestDENS") == 0){
		dcScheduler = new BestDENS();
		return 1;
	} else if(strcmp(scheduler_name, "HEROS") == 0){
		dcScheduler = new HerosScheduler();
		return 1;
	}


	std::cerr << "Unknown scheduler type: " << scheduler_name;
	abort();

}

void DataCenter::createNewMigration() {
	// Create and allocate VmMigration object
	Tcl& tcl = Tcl::instance();
	tcl.evalf("set vmmigration_($next_migration_id) [new VmMigration]");
	tcl.evalf("$vmmigration_($next_migration_id) set-id $next_migration_id");
	tcl.evalf(
			"$DCenter get-newest-migration $vmmigration_($next_migration_id)");
}

void DataCenter::migrateVm(VM* vm, ResourceProvider* target){
	createNewMigration(); // Creates and sets new migrations as tmp_migration_
	tmp_migration_->initalizeMigration(vm,target);

	tmp_migration_->startMigration();
	// The finalization of migration will be done by the migration object.
	// The data center (probably) does not need to be informed about that, as the vm agent will be re-attached to the new location.
	tmp_migration_ = NULL;
}

void DataCenter::receivedTsk(int tsksize, CloudTask * pTask, const char* flags)
{
	/* Update Stats */
	tskSubmitted_ ++;

	/* Schedule it */
	TskComAgent *tagent_ ;
	if(scheduleOnVms_)// You can switch to another scheduler or providers list using this variable.
	{
		tagent_ = dcScheduler->scheduleTask((CloudTask *) pTask,vm_list); }
	else {
		tagent_ = dcScheduler->scheduleTask((CloudTask *) pTask,host_list);
	}
	if(tagent_==NULL){
		tskFailed_++;
	} else {
		tagent_->sendmsg(tsksize, pTask, flags);
	}
}

TskComAgent *DataCenter::scheduleRoundRobin(CloudTask *tsk)
{
	int j = tskSubmitted_% numHostTskAgents_;

	return (host_agent_list.at(j));
}

TskComAgent *DataCenter::scheduleGreen(CloudTask *tsk)
{
	vector <ResourceProvider*>::iterator iter;

	for (iter = host_list.begin(); iter!=host_list.end(); iter++)
	{
		if ((*iter)->trySchedulingTsk(tsk))
			return (*iter)->getTskComAgent();
//			return (host_agent_list.at((*iter)->id_));
	}
	return NULL;
}

TskComAgent *DataCenter::scheduleRoundRobin(CloudTask *tsk,std::vector<TskComAgent*> agent_list)
{
	int j = tskSubmitted_% agent_list.size();

	return (agent_list.at(j));
}

TskComAgent *DataCenter::scheduleGreenVmOnly(CloudTask *tsk)
{
	vector <ResourceProvider*>::iterator iter;

	for (iter = vm_list.begin(); iter!=vm_list.end(); iter++)
	{
		if ((*iter)->trySchedulingTsk(tsk))
			return (*iter)->getTskComAgent();
//			return (vm_agent_list.at((*iter)->id_));
	}
	return NULL;
}

int DataCenter::command(int argc, const char*const* argv)
{
	if (argc == 2) {
		if (strcmp(argv[1], "compute-load") == 0) { 
			computeLoad();
			return (TCL_OK);
		} else 	if (strcmp(argv[1], "print-resspec") == 0) {
			printResourceSpecs();
			return (TCL_OK);
		} else if (strcmp(argv[1], "initially-configure-vms") == 0) {
			if(initiallyConfigureVms() == 0){
				return (TCL_OK);
			} else {
				return (TCL_ERROR);
			}
		} else 	if (strcmp(argv[1], "clear") == 0) {
			clear();
			return (TCL_OK);
		} else 	if (strcmp(argv[1], "schedule-on-vms") == 0) {
			setVmScheduling(true);
			return (TCL_OK);
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "add-dchost") == 0) { 
			DcHost *hst = dynamic_cast<DcHost*> (TclObject::lookup(argv[2]));
			if(hst){
				addHostPointer(hst);
				return (TCL_OK);
			}
			return (TCL_ERROR);
		} 	else	if (strcmp(argv[1], "add-vm") == 0) {
			VM *vm = dynamic_cast<VM*> (TclObject::lookup(argv[2]));
			if(vm){
				addVmPointer(vm);
				return (TCL_OK);
			}
			return (TCL_ERROR);
		}
		else if (strcmp(argv[1], "add-hosttaskagent") == 0) {
			TskComAgent *agnt = dynamic_cast<TskComAgent*> (TclObject::lookup(argv[2]));
			if(agnt){
				addHostTaskAgentPointer(agnt);
				numHostTskAgents_ = (int) host_agent_list.size();
				return (TCL_OK);
			}
			return (TCL_ERROR);
		} else if (strcmp(argv[1], "add-vmtaskagent") == 0) {
			TskComAgent *agnt = dynamic_cast<TskComAgent*> (TclObject::lookup(argv[2]));
			if(agnt){
				addVmTaskAgentPointer(agnt);
				numVmTskAgents_= (int) vm_agent_list.size();
				return (TCL_OK);
			}
			return (TCL_ERROR);
		}
		else if (strcmp(argv[1], "add-resspec") == 0) {
			ResourceSpec *resspec = dynamic_cast<ResourceSpec*> (TclObject::lookup(argv[2]));
			if(resspec){
				addResourceSpecificationPointer(resspec);
				return (TCL_OK);
			}
			return (TCL_ERROR);
		}	else if (strcmp(argv[1], "add-vresspec") == 0) {
			ResourceSpec *resspec = dynamic_cast<ResourceSpec*> (TclObject::lookup(argv[2]));
			if(resspec){
				addVirtualResourceSpecificationPointer(resspec);
				return (TCL_OK);
			}
			return (TCL_ERROR);
		}	else if (strcmp(argv[1], "add-pmodel") == 0) {
			PowerModel *pmodel = dynamic_cast<PowerModel*> (TclObject::lookup(argv[2]));
			if(pmodel){
				addPModelPointer(pmodel);
				return (TCL_OK);
			}
			return (TCL_ERROR);
		}  else if (strcmp(argv[1], "get-newest-migration") == 0) {
			VmMigration *mig = dynamic_cast<VmMigration*> (TclObject::lookup(argv[2]));
			if(mig){
				tmp_migration_=mig;
				return (TCL_OK);
			}
			return (TCL_ERROR);
		} else if (strcmp(argv[1], "set-scheduler") == 0) {
			if(setScheduler(argv[2])){
				return (TCL_OK);
			}
			return (TCL_ERROR);
		}
	} else if (argc == 4) {
		if (strcmp(argv[1], "configure-resource") == 0) {
			DcResource *res = dynamic_cast<DcResource*> (TclObject::lookup(argv[2]));
			if(res){
				if(configureResource(res,argv[3])==0){
					return (TCL_OK);
				} else {
					return (TCL_ERROR);
				}
			}
			return (TCL_ERROR);
		} else 	if (strcmp(argv[1], "configure-vresource") == 0) {
			DcResource *res = dynamic_cast<DcResource*> (TclObject::lookup(argv[2]));
			if(res){
				if(configureVirtualResource(res,argv[3])==0){
					return (TCL_OK);
				} else {
					return (TCL_ERROR);
				}
			}
			return (TCL_ERROR);
		} else 	if (strcmp(argv[1], "migrate-vm") == 0) {
			VM *vm = dynamic_cast<VM*> (TclObject::lookup(argv[2]));
			ResourceProvider *target = dynamic_cast<ResourceProvider*> (TclObject::lookup(argv[3]));
			if(vm && target){
				migrateVm( vm, target);
				return (TCL_OK);
			} else {
				return (TCL_ERROR);
			}
		}
		return (TCL_ERROR);


	}
	return (DataCenter::command(argc, argv));	
}

void DataCenter::computeLoad()
{
	/* Traverse servers and compute their average load */
	vector <ResourceProvider*>::iterator iter;

	double avgLoad = 0;
	double avgLoadMem = 0;
	double avgLoadStor = 0;
	double avgPower = 0;
	for (iter = host_list.begin(); iter!=host_list.end(); iter++)
	{

		avgLoad 	+= (*iter)->getResTypeUtil(Computing);
		avgLoadMem 	+= (*iter)->getResTypeUtil(Memory);
		avgLoadStor += (*iter)->getResTypeUtil(Storage);
		avgPower += ((DcHost*)(*iter))->eCurrentConsumption_;

	}

	for (iter = vm_list.begin(); iter!=vm_list.end(); iter++)
	{
				(*iter)->getResTypeUtil(Computing);
				(*iter)->getResTypeUtil(Memory);
				(*iter)->getResTypeUtil(Storage);
	}



	avgLoad_ = avgLoad / host_list.size();
	avgLoadMem_= avgLoadMem / host_list.size();
	avgLoadStor_= avgLoadStor / host_list.size();
	avgPower_= avgPower / host_list.size();
}

void DataCenter::printResourceSpecs(){
	vector <ResourceSpec*>::iterator iter;
	for (iter = resource_specification_list.begin(); iter!=resource_specification_list.end(); iter++)
	{
		(*iter)->print();
	}

}

int DataCenter::configureResource(DcResource* confRes, const char* spec_name){
	vector <ResourceSpec*>::iterator iter;
	int result = 1;
	std::string test = spec_name;
	for (iter = resource_specification_list.begin(); iter!=resource_specification_list.end(); iter++)
	{
		if((*iter)->name_==test){
			result = 0;
			break;
		}
	}
	if(result==0){
		confRes->setSpecification(*iter);
	} else {
		std::cerr << "ERROR: The requested resource specification is not registered in the data center!\n";
	}
	return result;
}

int DataCenter::configureVirtualResource(DcResource* confRes, const char* spec_name){
	vector <ResourceSpec*>::iterator iter;
	int result = 1;
	std::string test = spec_name;
	for (iter = virt_resource_specification_list.begin(); iter!=virt_resource_specification_list.end(); iter++)
	{
		if((*iter)->name_==test){
			result = 0;
			break;
		}
	}
	if(result==0){
		confRes->setSpecification(*iter);
	} else {
		std::cerr << "ERROR: The requested virtual resource specification is not registered in the data center!\n";
	}
	return result;
}


