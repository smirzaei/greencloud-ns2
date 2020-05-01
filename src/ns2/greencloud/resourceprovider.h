/**
 * resourceprovider.h
 *
 *  	@date Mar 11, 2013
 *      @author Guzek:Mateusz
 */

#ifndef RESOURCEPROVIDER_H_
#define RESOURCEPROVIDER_H_

#include <stdlib.h>
#include <vector>
#include <string.h>


#include "resource.h"
#include "dcresource.h"
#include "cpu.h"
#include "nic.h"
#include "resourceconsumer.h"
#include "cloudtask.h"

#include "scheduler.h"
#include "agent.h"
#include "tcp.h"
#include "provideroutagent.h"
#include "vmmigrationsink.h"
#include "corescheduler.h"
#include "tskagent.h"
#include "taskinfo.h"

class DcHost;
class VM;
class TskComSink;
class VmMigrationSink;


class ResourceProvider : public Handler {
public:
	ResourceProvider();
	virtual ~ResourceProvider();
	std::vector <std::vector <DcResource*> > resource_list; /*The resources are ordered by type*/
	void setTskComSink(TskComSink* tcs);

	int tryToAllocate(ResourceConsumer * rc); /* Check for space for new consumer. At the moment the resources are selected in a greedy way.*/
	bool releaseAllocation(ResourceConsumer * rc); /*Release the resources allocated resource consumer.*/

	bool addVM(VM* newVm);
	bool removeVM(VM* vm);
	ResourceProvider* getHost();
	DcHost* getRootHost();


	void recv(ResourceConsumer *rcobj);
	virtual int command(int argc, const char*const* argv);


	double getResTypeUtil(res_type type);
//	void computeCurrentLoad();	/* updates current load of the server after task is received or its execution completed */



	virtual void print()=0;
	virtual void printTasklist();

	virtual void addResource(DcResource* res);
	double getTotalCap(res_type type);

	virtual void updateEnergyAndConsumption()= 0;

	int id_;


	/* Stats */
	int ntasks_;
	double currentLoad_;
	double currentLoadMem_;
	double currentLoadStor_;
	double currentLoadNet_;


	int testSchedulingPossibility(CloudTask *tskobj);
	int trySchedulingTsk(CloudTask *tskobj);
	void sendTaskOutput(CloudTask* task);
	void scheduleNextExent(double nextDeadline);


	int eDVFS_enabled_;


	enum EventStatus { EVENT_IDLE, EVENT_PENDING, EVENT_HANDLING };

	int tskFailed_;
	static double uplink_overhead;
	static double MTU;
	static double useful_bytes;

	TskComAgent *tskComAgent;
	TskComAgent* getTskComAgent();


protected:
	double resource_utilization[LastResType+1]; /**< These values are for reading*/
	std::vector <ResourceConsumer*> hosted_vms_;		/**< hosted vm list */
	std::vector <VmMigrationSink*> vm_migration_sinks_;
	std::vector <ProviderOutAgent *> vm_migration_sources_;
	TskComSink * tskComSink_;
	ProviderOutAgent* poagent_;

	ResourceProvider * host;


	virtual void handle(Event* event);
	void updateEvent();
	void nextEvent(double delay);

	double getCurrentLoad();
	double updateResTypeUtil(res_type type);

	double getFreeCap(res_type type);
	double getFreeCapRecursive(res_type type);

	double getUsedNet(bool in, bool out);
	double getUsedNetRecursive(bool in, bool out);

	void setTskComAgent(TskComAgent* agnt);
	void setAgent(ProviderOutAgent* agent);
	TcpAgent* getAgent();

	void attachSink(VmMigrationSink* vm_mig_sink);
	void attachSource(ProviderOutAgent* tcp_agent);
	void detachSink(VmMigrationSink* vm_mig_sink);
	void detachSource(ProviderOutAgent* tcp_agent);



	bool started_;
	int status_;
	Event event_;
	double eventTime;


private:

	inline void _sched(double delay);
	inline void _cancel();

};


#endif /* RESOURCEOPROVIDER_H_ */
