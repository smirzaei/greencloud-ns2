/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#ifndef ns_clouduser_h
#define ns_clouduser_h
#include <iostream>
#include <iomanip>
#include <math.h>

#include "tclcl.h"
#include "ranvar.h"
#include "datacenter.h"
#include "cloudtask.h"
#include "taskinfo.h"



class CloudUser {
public:
	CloudUser();
	virtual ~CloudUser();
	CloudTask *createTask();
	void setRandomized(int i);
	int process_command(int argc, const char*const* argv);
	int id_;	/**< Task ID */

	/* Task properties */
	double tskmips_;	/**< Generated task computing demand */
	double memory_;		/**< Task memory_ demand */
	double storage_;	/**< Generated task computing demand */
	unsigned int tsksize_; /**< Size of task description sent to a server for execution in bytes */
	double tskmaxduration_; /**< Task execution deadline */

	int toutputsize_;	/**< Task output size in bytes (sent out of the data center) */
	int tintercom_;		/**< Size of inter-task communication */

	int randomized_;

	double mean_response_time_;
	double sd_response_time_;
	int unfinished_tasks_;

protected:
	DataCenter *dc_;
	int taskcounter_;
	ExponentialRandomVariable random_tskmips_;
	std::vector<TaskInfo*> tasks_info_;
	void printTasksStatus();
	void postSimulationTestTasks();
	void calculateStatistics();
};

#endif
