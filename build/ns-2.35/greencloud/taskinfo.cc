/*
 * taskinfo.cc
 *
 *  Created on: Apr 8, 2014
 *      Author: greencloud
 */

#include "taskinfo.h"

TaskInfo::TaskInfo(CloudTask* ct, double release_time, double due_time):
task_(ct),task_id_(ct->id_), release_time_(release_time), due_time_(due_time), server_finish_time_(-1), dc_exit_time_(-1) {


}

TaskInfo::~TaskInfo() {

}

CloudTask* TaskInfo::getTask(){
	return task_;
}
void TaskInfo::deleteTask(){
	delete task_;
	task_ = NULL;
}
double TaskInfo::getReleaseTime(){
	return release_time_;
}
double TaskInfo::getDueTime(){
	return due_time_;
}
double TaskInfo::getServerFinishTime(){
	return server_finish_time_;
}
double TaskInfo::getDcExitTime(){
	return dc_exit_time_;
}
int TaskInfo::getTaskId(){
	return task_id_;
}
void TaskInfo::setServerFinishTime(double time){
	server_finish_time_ = time;
}
void TaskInfo::finalizeDcExitTime(double time){
	dc_exit_time_ = time;
}

ResourceProvider* TaskInfo::getResourceProvider(){
	return rp_;
}

void TaskInfo::setResourceProvider(ResourceProvider* rp){
	rp_ = rp;
}

