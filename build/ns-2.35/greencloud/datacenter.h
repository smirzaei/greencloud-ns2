#ifndef ns_datacenter_h
#define ns_datacenter_h

#include "object.h"
#include "dchost.h"
#include "vm.h"
#include "tskagent.h"
#include "resourcespec.h"
#include "dcresource.h"
#include "powermodel/powermodel.h"
#include "vmmigration.h"
#include "dcscheduler/dcscheduler.h"
#include "dcscheduler/greenscheduler.h"
#include "dcscheduler/roundrobinscheduler.h"
#include "dcscheduler/randdens.h"
#include "dcscheduler/bestdens.h"
#include "dcscheduler/randomscheduler.h"
#include "dcscheduler/herosscheduler.h"

#include <tclcl.h>
#include <vector>
#include <math.h> 
#include <iostream> 
#include <numeric>


class DataCenter : public TclObject {
public:
	DataCenter();
	virtual ~DataCenter();
	void clear();
	void addHostPointer(DcHost *newhst); 
	void addVmPointer(VM* newvm);
	void addHostTaskAgentPointer(TskComAgent *newagnt);
	void addVmTaskAgentPointer(TskComAgent *newagnt);
	void addResourceSpecificationPointer(ResourceSpec *newresspec); /**< Registers resource used in Data Center.*/
	void addVirtualResourceSpecificationPointer(ResourceSpec *newresspec); /**< Registers virtual resource used in Data Center.*/
	void addPModelPointer(PowerModel* newPModel); /**< Register power model in Data Center.*/

	int initiallyConfigureVms(); /**< TODO: Dynamically configure the initial state of VMs.*/
	int setScheduler( const char* scheduler_name); /**< Release old DcScheduler, and create and set new by name*/
	void migrateVm(VM* vm, ResourceProvider* target);
	int configureResource(DcResource* confRes, const char* spec_name); /*Check existence of Resource specification named and adds pointer to it configured resource.*/
	int configureVirtualResource(DcResource* confRes, const char* spec_name); /*Check existence of Resource specification named and adds pointer to it configured resource.*/
	void printResourceSpecs();

	virtual void receivedTsk(int tsksize, CloudTask * pTask, const char* flags = 0);
	virtual int command(int argc, const char*const* argv);


	/* Stats */
	int tskSubmitted_;
	int tskFailed_;
	double avgLoad_;
	double avgLoadMem_;
	double avgLoadStor_;
	double avgPower_;

protected:
	vector <ResourceProvider*> host_list;
	vector <TskComAgent*> host_agent_list;
	vector <ResourceProvider*> vm_list;
	vector <TskComAgent*> vm_agent_list;
	vector <PowerModel*> power_model_list;

	DcScheduler* dcScheduler;

	/* Two dimensional structure: Vector of resource types, that are itself vectors of specification for given type.
	 * this structure contains all resource specifications available in data center.*/
	vector < ResourceSpec* > resource_specification_list;
	vector < ResourceSpec* > virt_resource_specification_list;

	DcHost *newhost_;
	/* Schedulers */
	int numHostTskAgents_;
	int numVmTskAgents_;

	VmMigration* tmp_migration_;

	/*Round robin should be adapted to the new resource policies. (i.e. add availability checking or data-center level queues for hosts)*/
	TskComAgent *scheduleRoundRobin(CloudTask *tsk);
	TskComAgent *scheduleRoundRobin(CloudTask *tsk,std::vector<TskComAgent*> agent_list);
	TskComAgent *scheduleGreen(CloudTask *tsk);
	TskComAgent *scheduleGreenVmOnly(CloudTask *tsk);
	void computeLoad();

	bool scheduleOnVms_;
	void setVmScheduling(bool scheduleOnVms);

private:
	void createNewMigration();
};

#endif
