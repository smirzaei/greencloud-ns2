#ifndef ns_cloudtask_h
#define ns_cloudtask_h

#include "scheduler.h"
#include "resourceconsumer.h"
#include <stdlib.h>
#include <float.h>
#include <vector>

class TaskAlloc;
class ResourceProvider;
class CloudUser;
class TaskInfo;

class CloudTask : public ResourceConsumer {
public:
	CloudTask();
	CloudTask(unsigned int size, double duration, std::vector<Resource*> demand, CloudUser* clouduser);
	virtual ~CloudTask();

	int getID() {return id_;};
	double getMIPS(int rd, int cap);
	double getDeadline() {return deadline_;};
	int getOutput() {return output_;};

	void setMIPS(int rd, int cap,double mips);
	void setID(int id) {id_ = id;};
	void setDeadline(double deadline) {deadline_ = deadline;};
	void setOutput(int output) {output_ = output;};
	void setIntercom(int intercom) {intercom_ = intercom;};
	/*
	 * Checks if all allocations are successfully finished.
	 */
	bool isFinished();

	void printCompCapacites();
	/**
	 * Removes task allocation.
	 */
	void removeTaskAlloc(TaskAlloc* ta);
	/**
	 * Perform failure - related actions.
	 * Typically used after failure of one of the task allocations is detected.
	 */
	void fail(ResourceProvider* provider);
	/**
	 * Remove all task allocations from the schedulers.
	 */
	void releaseAllTaskAllocs();

	int id_;
	bool scheduled_; /**< true if task has been scheduled */
	bool started_;  /**< true if task has started its execution */
	double deadline_;	/**< task deadline */
	bool failed_;
	/**
	 * Task allocations are objects that create
	 * many-to-many relationship between computational capacities of a task and core schedulers.
	 */
	std::vector<TaskAlloc*> task_allocations_;
	TaskInfo* info_;

protected:
	void handler(Event *);
	CloudUser * user_; /**< The cloud user that created the task. */

	int output_;	/**< amount of data in bytes sent out of the data center upon task completion. */
	int intercom_;	/**< amount of data in bytes to be transferred to another data center application. */

};

#endif
