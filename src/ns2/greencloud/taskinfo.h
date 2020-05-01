/*
 * taskinfo.h
 *
 *  Created on: Apr 8, 2014
 *      Author: greencloud
 */

#ifndef TASKINFO_H_
#define TASKINFO_H_
#include "cloudtask.h"
#include "resourceprovider.h"

class TaskInfo {
public:
	TaskInfo(CloudTask* ct, double release_time, double due_time);
	virtual ~TaskInfo();

	CloudTask* getTask();
	void deleteTask();
	double getReleaseTime();
	double getDueTime();
	double getServerFinishTime();
	double getDcExitTime();
	ResourceProvider * getResourceProvider();
	void setResourceProvider(ResourceProvider * rp);
	int getTaskId();
	void setServerFinishTime(double time);
	void finalizeDcExitTime(double time);
protected:
	CloudTask* task_;
	int task_id_;
	double release_time_;
	double due_time_;
	double server_finish_time_;
	double dc_exit_time_;
	ResourceProvider* rp_;
};

#endif /* TASKINFO_H_ */
