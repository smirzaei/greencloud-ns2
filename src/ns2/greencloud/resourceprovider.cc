/*#
 * resourceprovider.cc
 *
 *  	@date Mar 11, 2013
 *      @author Guzek:Mateusz
 */

#include "resourceprovider.h"
#include "vm.h"
#include "tskcomsink.h"
#include "dchost.h"

double ResourceProvider::MTU=1500.0;
double ResourceProvider::useful_bytes=1460.0;
double ResourceProvider::uplink_overhead=ResourceProvider::MTU/ResourceProvider::useful_bytes; // used to include the tcp headers of uplink communication


ResourceProvider::ResourceProvider() : id_(0), ntasks_(0),
		currentLoad_ (0.0), currentLoadMem_(0.0), currentLoadStor_(0.0), currentLoadNet_(0.0),
		eDVFS_enabled_(0.0), tskFailed_(0),tskComAgent(NULL), 	host(NULL), started_(false), eventTime(DBL_MAX)
{

	for(int i = 0; i <= LastResType; i++){
		resource_list.push_back(std::vector <DcResource*>());
	}
	for(int i = 0; i <= LastResType; i++){
		resource_utilization[i] = 0.0;
	}
	hosted_vms_.clear();
	status_ = EVENT_IDLE;
	poagent_ = NULL;
}

ResourceProvider::~ResourceProvider() {
	std::vector <std::vector<DcResource*> >::iterator iter;
	for(iter = resource_list.begin(); iter!=resource_list.end() ;iter++){
		 std::vector<DcResource*>::iterator iter2;
			for(iter2 = iter->begin(); iter2!=iter->end() ;iter2++){
				delete (*iter2);
			}
	}
	delete poagent_;

}


int ResourceProvider::tryToAllocate(ResourceConsumer * rc){
	std::vector <ResDemand*>::iterator u_res;

	// TODO (possible) 1. Sort the provider resources according to the free capacity (descending)
	// TODO (possible) 2. Sort the consumer ... (the same).

	/*For each resource demand of consumer:*/
	for (u_res = rc->res_demands.begin() ; u_res!=rc->res_demands.end(); u_res++)
	{
		//		/*For dynamic consumers (e.g. tasks) do not reserve computing and networking resource.
		//		 * For non-dynamic resources (e.g. VMs): reserve computing and networking. */
		if(((*u_res)->getType()!=Computing && (*u_res)->getType()!=Networking ) || rc->isTask == false){
			bool possible = false;

			std::vector <Capacity>::iterator req_cap_cons = (*u_res)->capacity.begin();
			std::vector <Capacity *>::iterator loc_cap_cons = (*u_res)->capacity_location.begin();
			std::vector <CoreScheduler*>::iterator u_core;
			if((*u_res)->getType()==Computing && (*u_res)->supported_virtual_resource){
				u_core = ((CPU*)((*u_res)->supported_virtual_resource))->cores_schedulers_.begin();
			}

			std::vector <DcResource*>::iterator p_res;
			std::vector <CoreScheduler*>::iterator p_core;
			/*For each DcResource of provider:*/
			for(p_res = resource_list[(*u_res)->getType()].begin(); p_res!=resource_list[(*u_res)->getType()].end() ;p_res++){

				/*Check architecture*/
				if((*u_res)->getArch() <= (*p_res)->getArch()){

					if((*u_res)->capacity.empty()==true){
						possible = true;
					}

					std::vector <Capacity>::iterator aval_cap_prov = (*p_res)->capacity.begin();
					if((*u_res)->getType()==Computing && (*u_res)->supported_virtual_resource){
						p_core = ((CPU*)(*p_res))->cores_schedulers_.begin();
					}
					/*Case of empty capacity vector - check only arch constraint.*/

					for(; aval_cap_prov!= (*p_res)->capacity.end() ; ){
						if((*aval_cap_prov)>=(*req_cap_cons)){
							(*aval_cap_prov)-=(*req_cap_cons);
							/*Demands capacities are linked with the supported resources capacities (1 to 1),
							 *  so the intermediary ResDemand is neglected:*/
							if(rc->isVM){
								(*aval_cap_prov).virtual_capacities.push_back(req_cap_cons->virtual_capacities.at(0));
							}
							(*loc_cap_cons)=&(*aval_cap_prov);
							if((*u_res)->getType()==Computing && (*u_res)->supported_virtual_resource){
								(*p_core)->addVcoreScheduler((*u_core));
							}
							req_cap_cons++;
							loc_cap_cons++;
							if(req_cap_cons== (*u_res)->capacity.end()){
								possible = true;
								break;
							}
							if((*u_res)->getType()==Computing && (*u_res)->supported_virtual_resource){
								u_core++;
							}

						} else {
							aval_cap_prov++;
							if((*u_res)->getType()==Computing && (*u_res)->supported_virtual_resource){
								p_core++;
							}
						}
					}

				}
				/*Resource architecture rejected:*/
				else {
					//std::cerr << "Arch, Requested: " << (*u_res)->getArch()  << "\tProvided: " <<(*p_res)->getArch() << "\n";
				}
				if(possible == true){
					/*Break the main loop of scanning the provider resources.*/
					break;
				}
			}
			if(possible==false){
				//std::cerr << "Impossible to allocate here.\n";
				releaseAllocation(rc);
				return false;
			}
		}
	}
	//std::cerr << "Allocation success.\n";
	return true;
}

bool ResourceProvider::releaseAllocation(ResourceConsumer * rc){
	if((*rc).res_demands.empty()){
		std::cerr << "Nothing to release \n";
		return true;
	}	//	std::cerr << "Something to release \n";

	std::vector <ResDemand*>::iterator u_res;
	for (u_res = rc->res_demands.begin() ; u_res!=rc->res_demands.end(); u_res++)
	{

		if(((*u_res)->getType()!=Computing && (*u_res)->getType()!=Networking ) || rc->isTask == false){
			std::vector <Capacity>::iterator consumption;
			std::vector <Capacity *>::iterator location;
			for(consumption=(*u_res)->capacity.begin(),
					location=(*u_res)->capacity_location.begin();
					consumption!=(*u_res)->capacity.end();
					consumption++,location++){
				if((*location)==NULL){
				} else {
					**location = (**location)+(*consumption);
					*location = NULL;
				}
			}
		}
		if(rc->isTask==false){
			if((*u_res)->supported_virtual_resource != NULL){
				if(	(*u_res)->supported_virtual_resource->getType()==Computing){
					CPU* cpu = (CPU*)(*u_res)->supported_virtual_resource;
					std::vector<CoreScheduler* >::iterator cs;
					for(cs = cpu->cores_schedulers_.begin(); cs != cpu->cores_schedulers_.end(); cs++){
						CoreScheduler* host_cs = (*cs)->getHostScheduler();
						if(host_cs!=NULL){
							host_cs->removeVcoreScheduler(*cs);
						}
					}
				}
			}
		}

	}
	if(rc->isTask==false){
		// non-task (VM  or migration) specific cleanup. Handled in the respective classes.
	}

	return true;
}

ResourceProvider* ResourceProvider::getHost(){
	return host;
}


DcHost* ResourceProvider::getRootHost(){
	if(host == NULL){
		DcHost* root = static_cast<DcHost*>(this);
		return root;
	} else {
		return host->getRootHost();
	}

}

bool ResourceProvider::addVM(VM* newVm){

	this->updateEnergyAndConsumption();

	if(tryToAllocate(newVm)){
		((newVm))->setHost(this);
		hosted_vms_.push_back(newVm);
		return true;
	} else {
		return false;
	}
}

bool ResourceProvider::removeVM(VM* vm){
	this->updateEnergyAndConsumption();
	if(releaseAllocation(vm)){
		(vm)->setHost(NULL);
		hosted_vms_.erase(remove(hosted_vms_.begin(),hosted_vms_.end(),vm),
				hosted_vms_.end()); /*erase-remove idiom*/
		return true;
	} else {
		return false;
	}
}





void ResourceProvider::recv(ResourceConsumer *rcobj)
{
	this->updateEnergyAndConsumption();
	if(rcobj->isTask==true){
		vector<CloudTask*>::iterator iter;
		CloudTask* tskobj = (CloudTask*) rcobj;

		ntasks_ ++;			// update total number of the received tasks

		if(tskobj->scheduled_==false){
			if(trySchedulingTsk(tskobj)==false){
				tskobj->fail(this);
//								std::cout << "Unscheduled task failed due to insufficient resources";
				return;
			}
		}
		std::vector <CoreScheduler*>::iterator core_s;
		std::vector <DcResource*>::iterator cpu_iter;

		/*If it is posible to allocate:*/
		if(tryToAllocate(tskobj)){
			for(cpu_iter=resource_list[Computing].begin(); cpu_iter != resource_list[Computing].end(); cpu_iter++){
				CPU* cpu = (CPU*) (*cpu_iter);
				for(core_s=cpu->cores_schedulers_.begin(); core_s != cpu->cores_schedulers_.end(); core_s++){
					(*core_s)->startTaskExecution(tskobj);
				}
			}
			/*Otherwise task fails!*/
		} else {
			tskobj->fail(this);
			std::cout << "Task failed due to insufficient resources";
			return;
		}

	} else {
		std::cerr <<"It is not a task!";
		return;
	}
}



void ResourceProvider::nextEvent(double delay)
{
   if(Scheduler::instance().clock()>=eventTime)
       eventTime=DBL_MAX;
   if(Scheduler::instance().clock()+delay>=eventTime)
       return;

   eventTime=Scheduler::instance().clock()+delay;

   if (status_ == EVENT_PENDING) {
       _cancel();
       status_ = EVENT_IDLE;
   }

   event_.handler_ = this;
   event_.time_ = Scheduler::instance().clock();

   _sched(delay);
   status_ = EVENT_PENDING;
}

void ResourceProvider::handle(Event* event)
{
	std::vector <CoreScheduler*>::iterator core_s;
	std::vector <DcResource*>::iterator cpu_iter;
	for(cpu_iter=resource_list[Computing].begin(); cpu_iter != resource_list[Computing].end(); cpu_iter++){
		CPU* cpu = (CPU*) (*cpu_iter);
		for(core_s=cpu->cores_schedulers_.begin(); core_s != cpu->cores_schedulers_.end(); core_s++){
			(*core_s)->updateTskList();
		}
	}


}

double ResourceProvider::getFreeCapRecursive(res_type type){
	double free_cap = getFreeCap(type);

	std::vector <ResourceConsumer*>::iterator vm_iter;
	for(vm_iter=hosted_vms_.begin(); vm_iter != hosted_vms_.end(); vm_iter++){
		VM* vm = static_cast<VM*>(*vm_iter);
		free_cap += vm->getFreeCapRecursive(type);
	}
	return free_cap;
}
double ResourceProvider::getFreeCap(res_type type){
	double free_cap = 0;
	std::vector <DcResource*>::iterator dc_res;
	for(dc_res=resource_list[type].begin(); dc_res != resource_list[type].end(); dc_res++){
		std::vector <Capacity>::iterator free_cap_iter;
		for(free_cap_iter = (*dc_res)->capacity.begin();
				free_cap_iter != (*dc_res)->capacity.end();
				free_cap_iter++){
			free_cap +=	*free_cap_iter;
		}
	}

	return free_cap;
}

double ResourceProvider::getUsedNet(bool in, bool out){
	double result = 0;
	if(in){
		double elapsed_time = Scheduler::instance().clock() - this->tskComSink_->getLastBytesSinceTime();
		if(elapsed_time>0){
		double down_link_util = this->tskComSink_->resetBytesSince();
		std::vector<VmMigrationSink*>::iterator vms;
		for(vms = vm_migration_sinks_.begin();vms!= vm_migration_sinks_.end();vms++){
			double recent_bytes = (*vms)->resetBytesSince();
			down_link_util += recent_bytes; 		}
		result += (down_link_util/elapsed_time);
		}
	}
	if(out){
		double elapsed_time = Scheduler::instance().clock() - this->poagent_->updateTime();
		if(elapsed_time>0){
		double up_link_util = this->poagent_->updateAgentDataBytes();
		std::vector<ProviderOutAgent*>::iterator poa;
		for(poa = vm_migration_sources_.begin();poa!= vm_migration_sources_.end();poa++){
			double recent_bytes = (*poa)->updateAgentDataBytes();
			up_link_util += recent_bytes;
		}
		result += ((up_link_util*ResourceProvider::uplink_overhead)/elapsed_time);
		}
	}

	return result;
}

double ResourceProvider::getUsedNetRecursive(bool in, bool out){
	double used_net = getUsedNet(in,out);
	std::vector <ResourceConsumer*>::iterator vm_iter;
	for(vm_iter=hosted_vms_.begin(); vm_iter != hosted_vms_.end(); vm_iter++){
		VM* vm = static_cast<VM*>(*vm_iter);
		used_net += vm->getUsedNetRecursive(in,out);
	}

	return used_net;
}

double ResourceProvider::getTotalCap(res_type type){
	double total_cap = 0;
	std::vector <DcResource*>::iterator dc_res;
	for(dc_res=resource_list[type].begin(); dc_res != resource_list[type].end(); dc_res++){
		std::vector <Capacity>::iterator total_cap_iter;
		for(total_cap_iter = (*dc_res)->specification->capacity.begin();
				total_cap_iter != (*dc_res)->specification->capacity.end();
				total_cap_iter++){
			total_cap += *total_cap_iter;
		}

	}
	return total_cap;

}

double ResourceProvider::getResTypeUtil(res_type type){
	if(type==Networking){
		return resource_utilization[Networking];
	} else {
		return updateResTypeUtil(type);
	}
}

double ResourceProvider::updateResTypeUtil(res_type type){
	if(type==Computing){
		resource_utilization[Computing] =  getCurrentLoad();
		return resource_utilization[Computing];
	} else if(type == Networking){
		double total_cap = getTotalCap(type) * 2; // Bidirectional links
		double used_net_bytes = getUsedNetRecursive(true,true);
		double result = used_net_bytes/total_cap;
		resource_utilization[Networking] =currentLoadNet_ = result;
		return resource_utilization[Networking];
	} else {
		double total_cap = getTotalCap(type);
		if(total_cap==0){
			return 0; //There is no components of this resource type
		}
		double free_cap = getFreeCapRecursive(type);
		double result =  1 - (free_cap/total_cap);
		if(type == Memory){
			currentLoadMem_ = result;
		} else if(type==Storage){
			currentLoadStor_ = result;
		}
		resource_utilization[type] = result;
		return result;
	}
}

double ResourceProvider::getCurrentLoad()
{
	double nominal_mips = 0;
	double current_mips = 0;
	std::vector <DcResource*>::iterator cpu_iter;
	for(cpu_iter=resource_list[Computing].begin(); cpu_iter != resource_list[Computing].end(); cpu_iter++){
		DcResource* res = *cpu_iter;
		CPU* cpu  = (CPU*) res;
		nominal_mips += cpu->getNominalMIPS();
		current_mips += cpu->getCurrentMIPS();
	}
	currentLoad_ = current_mips/nominal_mips;

	return currentLoad_;
}

int ResourceProvider::testSchedulingPossibility(CloudTask *tskobj){
	int result = trySchedulingTsk(tskobj);
	if(result){
		releaseAllocation(tskobj);
		tskobj->releaseAllTaskAllocs();
		tskobj->scheduled_=false;
	}
	return result;
}

int ResourceProvider::trySchedulingTsk(CloudTask *tskobj)
{

	/* get minimum processing rate required by the task */
	if(tryToAllocate(tskobj)){
		releaseAllocation(tskobj);
	} else {
		return false;
	}

	std::vector<TaskAlloc*> tmp_task_allocs;
	tmp_task_allocs.clear();

	std::vector<TaskAlloc*>::iterator iter;
	for(iter = tskobj->task_allocations_.begin() ; iter != tskobj->task_allocations_.end();iter++){
		TaskAlloc* task_alloc = (*iter);
		bool core_found = false;
		tmp_task_allocs.push_back(task_alloc);

		double tskrate = (double)task_alloc->getMIPS()/(task_alloc->getDeadline() - Scheduler::instance().clock());

		std::vector <CoreScheduler*>::iterator core_s;
		std::vector <DcResource*>::iterator cpu_iter;
		for(cpu_iter=resource_list[Computing].begin(); cpu_iter != resource_list[Computing].end(); cpu_iter++){
			CPU* cpu = (CPU*) (*cpu_iter);
			for(core_s=cpu->cores_schedulers_.begin(); core_s != cpu->cores_schedulers_.end(); core_s++){

				double maxrate = (*core_s)->getMostUrgentTaskRate();
				if (tskrate > maxrate){maxrate = tskrate;}
				if (maxrate*((*core_s)->getAllTasksNumber() + 1) <= (*core_s)->getAvailableMIPS()){
					/* task can be scheduled, add to the in-fly list */
					(*core_s)->assignTask(task_alloc);
					core_found = true;
					break;
				}
			}
			if(core_found){break;}
		}
		if(core_found==false){
				tskobj->releaseAllTaskAllocs();
			//Release all tmp_task_allocs
//			std::vector<TaskAlloc*>::iterator failed_alloc;
//			for(failed_alloc = tmp_task_allocs.begin();failed_alloc != tmp_task_allocs.end(); failed_alloc++){
//				CoreScheduler* core_of_failed = (*failed_alloc)->getCoreScheduler();
//				if(core_of_failed!=NULL){
//					core_of_failed->removeFromAssginedList((*failed_alloc));
//				}
//			}
			return false;
		}
	}
	tskobj->scheduled_=true;
	return true;

}



void ResourceProvider::addResource(DcResource* res){

	resource_list[res->getType()].push_back(res);
	if(res->getType()==Computing){
		CPU* cpu_res = static_cast<CPU*>(res);
		cpu_res->setDVFS(eDVFS_enabled_);
		cpu_res->setProvider(this);
	}
	if(res->getType()==Networking){
		NIC* nic_res = static_cast<NIC*>(res);
		nic_res->setRp(this);
	}

}

void ResourceProvider::setTskComSink(TskComSink* tcs){
	this->tskComSink_ =tcs;
}

inline void  ResourceProvider::_sched(double delay) {
	(void)Scheduler::instance().schedule(this, &event_, delay);
}
inline void ResourceProvider:: _cancel() {
	(void)Scheduler::instance().cancel(&event_);
	// no need to free event_ since it's statically allocated
}


void ResourceProvider::printTasklist(){
	std::cout << "printTasklist Status: (FUNCTION UNDER CONSTRUCTION)\n";

}
void ResourceProvider::setAgent(ProviderOutAgent* agent){
	poagent_ = agent;
}

TcpAgent* ResourceProvider::getAgent(){
	return poagent_;

}

void ResourceProvider::setTskComAgent(TskComAgent* agnt){
	this->tskComAgent = agnt;
}

TskComAgent* ResourceProvider::getTskComAgent(){
	return this->tskComAgent;
}

void ResourceProvider::sendTaskOutput(CloudTask* task){
	if ((getAgent()) && (task->getOutput() != 0)) {
		/*Record finish time of task on the server.*/
		task->info_->setServerFinishTime(Scheduler::instance().clock());
		task->info_->setResourceProvider(this);
		/*Send task output.*/
		poagent_->sendmsg(task->getOutput(),task);
	}
}


void ResourceProvider::scheduleNextExent(double nextDeadline){
	/* reschedule next update */
	if (nextDeadline != DBL_MAX) nextEvent(nextDeadline);


}

void ResourceProvider::attachSink(VmMigrationSink* vm_mig_sink){
	vm_migration_sinks_.push_back(vm_mig_sink);
}


void ResourceProvider::attachSource(ProviderOutAgent* poa){
	vm_migration_sources_.push_back(poa);
}

void ResourceProvider::detachSink(VmMigrationSink* vm_mig_sink){
	vm_migration_sinks_.erase(remove(vm_migration_sinks_.begin(),vm_migration_sinks_.end(),vm_mig_sink),
			vm_migration_sinks_.end()); /*erase-remove idiom*/
}


void ResourceProvider::detachSource(ProviderOutAgent* poa){
	vm_migration_sources_.erase(remove(vm_migration_sources_.begin(),vm_migration_sources_.end(),poa),
			vm_migration_sources_.end()); /*erase-remove idiom*/
}

int ResourceProvider::command(int argc, const char*const* argv)
{
	Tcl& tcl = Tcl::instance();

	if (argc == 2) {
		return (TCL_ERROR);

	} else if (argc == 3) {
		if (strcmp(argv[1], "attach-agent") == 0) {
			setAgent((ProviderOutAgent*) TclObject::lookup(argv[2]));
			if (getAgent() == 0) {
				tcl.resultf("no such agent %s", argv[2]);
				return(TCL_ERROR);
			}
			return(TCL_OK);
		}
		else if (strcmp(argv[1], "set-taskcomagent") == 0) {
			TskComAgent *agnt = dynamic_cast<TskComAgent*> (TclObject::lookup(argv[2]));
			if(agnt){
				setTskComAgent(agnt);
				return (TCL_OK);
			}
			return (TCL_ERROR);
		}
		else 	if (strcmp(argv[1], "attach-vm-mig-sink") == 0) {
			VmMigrationSink* vm_mig_sink = ((VmMigrationSink*) TclObject::lookup(argv[2]));
			attachSink(vm_mig_sink);
			if (getAgent() == 0) {
				tcl.resultf("no such agent %s", argv[2]);
				return(TCL_ERROR);
			}
			return(TCL_OK);
		} else 	if (strcmp(argv[1], "attach-vm-mig-source") == 0) {
			ProviderOutAgent* vm_migration_source =  ((ProviderOutAgent*) TclObject::lookup(argv[2]));
			attachSource(vm_migration_source);
			if (getAgent() == 0) {
				tcl.resultf("no such agent %s", argv[2]);
				return(TCL_ERROR);
			}
			return(TCL_OK);
		}  else 	if (strcmp(argv[1], "detach-vm-mig-sink") == 0) {
			VmMigrationSink* vm_mig_sink = ((VmMigrationSink*) TclObject::lookup(argv[2]));
			detachSink(vm_mig_sink);
			if (getAgent() == 0) {
				tcl.resultf("no such agent %s", argv[2]);
				return(TCL_ERROR);
			}
			return(TCL_OK);
		} else 	if (strcmp(argv[1], "detach-vm-mig-source") == 0) {
			ProviderOutAgent* vm_migration_source =  ((ProviderOutAgent*) TclObject::lookup(argv[2]));
			detachSource(vm_migration_source);
			if (getAgent() == 0) {
				tcl.resultf("no such agent %s", argv[2]);
				return(TCL_ERROR);
			}
			return(TCL_OK);
		} else if (strcmp(argv[1], "add-resource") == 0) {
			DcResource* res = (DcResource*) TclObject::lookup(argv[2]);
			if (res == NULL) {
				tcl.resultf("no such resource %s", argv[2]);
				return(TCL_ERROR);
			}
			addResource(res);
			return(TCL_OK);
		} else if (strcmp(argv[1], "add-vm") == 0) {
			VM* vm = (VM*) TclObject::lookup(argv[2]);
			if (vm == NULL) {
				tcl.resultf("no such vm %s", argv[2]);
				return(TCL_ERROR);
			}
			if(addVM(vm)){
				return(TCL_OK);
			} else {
				/* It was impossible to allocate vm on the host.*/
				std::cerr << "ERROR: A VM was allocated on a machine that has not enough resources. (Creation was called from Tcl)";
				return(TCL_ERROR);
			}
		}
	}
	return (TCL_ERROR);
}
