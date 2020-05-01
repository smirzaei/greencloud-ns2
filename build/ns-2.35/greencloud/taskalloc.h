/*
 * taskalloc.h
 *
 *  @date Sep 26, 2013
 *  @author Guzek:Mateusz
 */

#ifndef TASKALLOC_H_
#define TASKALLOC_H_

#include "cloudtask.h"
class CoreScheduler;

class TaskAlloc {
	//TODO Try to optimize iterations over task allocations. (esp. usage of erase and resetting iterators)
public:
	CloudTask* cloudTask; //cloudTask

	CoreScheduler* core;
	double executedSince_;	/**< last time instance of task execution */


	TaskAlloc(CloudTask * ct, int rd, int cap);
	virtual ~TaskAlloc();

	double getMIPS();
	double getDeadline();
	void setExecTime(double execTime) {executedSince_ = execTime;};
	void setCoreScheduler(CoreScheduler * cs);
	CoreScheduler* getCoreScheduler();
	bool operator==(const TaskAlloc &other) const;
	void setComputingRate(double rate);
	double execTime();
	void updateMIPS();
	void removeAfterFailure();

	void print();
private:
	int rd; /**< resource demand */
	int cap; /**< capacity */
};

#endif /* TASKALLOC_H_ */
