/*
 * corescheduler.h
 *
 *  @date Aug 29, 2013
 *  @author Guzek:Mateusz
 */

#ifndef CORESCHEDULER_H_
#define CORESCHEDULER_H_

#include <stdlib.h>
#include <vector>
#include <iostream>
#include "taskalloc.h"

class CloudTask;
class TaskAlloc;
class ResourceProvider;



/*CoreScheduler is a class responsible for scheduling and load calculation of a core.
 * It stores the information about the nominal, available and current mips on the level of scheduler.
 * The tasks lists stores the tasks that are executed/assigned on/to the core.
 * Additionally, it stores the pointers to the hosted schedulers of vcores to enable tracking the load of a core. */
class CoreScheduler {
public:
	CoreScheduler(Capacity * nominal_mips_,Capacity * available_mips_);
	virtual ~CoreScheduler();

	void setProvider(ResourceProvider * provider);
	void setDVFS(int eDVFS_enabled_);
	double getCurrentMIPS();
	double getCurrentMIPSRecursive();
	double getNominalMIPS();
	double getAvailableMIPS();
	ResourceProvider * getProvider();

	void updateTskList(double c_mips);
	void updateTskList();
	void removeCompleted();
	CloudTask* removeTaskAlloc(std::vector<TaskAlloc*>::iterator& iter,bool executed);
	void removeTaskAlloc( TaskAlloc* ta);
	void removeFailedTaskAlloc(std::vector<TaskAlloc*>::iterator& iter,bool executed);
	void updateTskComputingRates( double c_mips);
	double getMostUrgentTaskRate();
	void setComputingRate();
	int getAllTasksNumber();

	void assignTask(TaskAlloc* tskobj);
	void executeTask(TaskAlloc* tskobj);
	bool removeFromAssginedList(TaskAlloc* tskobj);
	bool removeAllocationsFromAssginedList(CloudTask* tskobj);
	void startTaskExecution(CloudTask* tskobj);

	void addVcoreScheduler(CoreScheduler* cs);
	void removeVcoreScheduler(CoreScheduler* cs);
	CoreScheduler * getHostScheduler();



private:
	Capacity * nominal_mips_; 	/**< Maximal available MIPS from ResourceSpec */
	Capacity * available_mips_; 	/**< MIPS unavailable for this scheduler (reserved for hosted vms)*/
	double current_mips_;		/**< MIPS currently used by this scheduler*/

	ResourceProvider * provider; /**< The resorce provider that uses the scheduler*/
	std::vector<CoreScheduler *> hosted_vcores_schedulers; 	/**< List of schedulers of hosted vcores*/
	std::vector<TaskAlloc *> tasks_alloc_list_; 		/**<  execution list */
	std::vector<TaskAlloc *> tasks_alloc_assigned_;	/**<  in-fly list */
	CoreScheduler * host_scheduler_; /**< The scheduler that hosts this (next level) scheduler*/
	int eDVFS_enabled_; /**< DVFS flag, influences the scheduling policy*/
	int tskAllocFailed_; /**< Number of TaskAllocations that failed on this scheduler (not used yet)*/
};



#endif /* CORESCHEDULER_H_ */
