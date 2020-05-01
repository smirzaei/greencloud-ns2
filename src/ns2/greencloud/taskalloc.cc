/*
 * taskalloc.cc
 *
 *  @date Sep 26, 2013
 *  @author Guzek:Mateusz
 */

#include "taskalloc.h"
#include "corescheduler.h"


TaskAlloc::~TaskAlloc() {

}

TaskAlloc::TaskAlloc(CloudTask * ct, int rd, int cap) :  core(NULL),executedSince_(0.0){
	this->cloudTask = ct;
	this->rd = rd;
	this->cap = cap;
}
double TaskAlloc::getMIPS() {
	return cloudTask->getMIPS(rd,cap);
}
double TaskAlloc::getDeadline() {
	return cloudTask->getDeadline();
}

void TaskAlloc::setCoreScheduler(CoreScheduler * cs){
	this->core=cs;
}
CoreScheduler* TaskAlloc::getCoreScheduler(){
	return this->core;
}

bool TaskAlloc::operator==(const TaskAlloc &other) const {
	if(cloudTask==other.cloudTask && rd == other.rd && cap == other.cap){
		return true;
	} else {
		return false;
	}
}

void TaskAlloc::setComputingRate(double rate)
{
	/* update what has already been computed */
	updateMIPS();
	cloudTask->res_demands.at(rd)->current_performance.at(cap)= rate;
}

void TaskAlloc::print(){
	std::cerr << "id "<< cloudTask->id_ << " "<< rd << " ,c: " << cap << " mips left: " << getMIPS() << "\texec since: " << executedSince_;
}

void TaskAlloc::updateMIPS()
{
	ResDemand* res_dem = cloudTask->res_demands.at(rd);

	double operationsComputed = (res_dem)->current_performance.at(cap)*(Scheduler::instance().clock() - executedSince_);

	if((res_dem)->capacity.at(cap) > operationsComputed){
		(res_dem)->capacity.at(cap) -= operationsComputed;
	} else {
		(res_dem)->capacity.at(cap) = 0;
	}
//	std::cout << "--\t Task: " << this->cloudTask->id_ << " MIPS: " << getMIPS() << "\n";


	executedSince_ = Scheduler::instance().clock();
}



double TaskAlloc::execTime()
{
	if (cloudTask->res_demands.at(rd)->current_performance.at(cap)){
		return ((double)(cloudTask->res_demands.at(rd)->capacity.at(cap))/cloudTask->res_demands.at(rd)->current_performance.at(cap));
	}
	else{
		return DBL_MAX;
	}
}

