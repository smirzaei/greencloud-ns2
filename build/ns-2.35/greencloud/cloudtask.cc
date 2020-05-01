/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#ifndef lint
static const char rcsid[] =
		"@(#) $Header: /cvsroot/nsnam/ns-2/common/taskobject.cc,v 1.43 $";
#endif

#include "cloudtask.h"
#include "taskalloc.h"
#include "corescheduler.h"
#include "resourceprovider.h"
#include "clouduser.h"

CloudTask::CloudTask() : id_(0),   scheduled_(false), started_(false), failed_(false), output_(0), intercom_(0)
{
	isTask = true;
}


CloudTask::CloudTask(unsigned int size, double duration, std::vector<Resource*> demand, CloudUser* clouduser) :
ResourceConsumer(size, demand, true, false),id_(0), scheduled_(false), started_(false), failed_(false), user_(clouduser), output_(0), intercom_(0)
{

	currProcRate_=0.0;
	deadline_ = Scheduler::instance().clock() + duration;
	isTask = true;
	for(unsigned int rd = 0; rd < res_demands.size();rd++){
		if(res_demands.at(rd)->getType()==Computing){
			for(unsigned int cap = 0; cap < res_demands.at(rd)->capacity.size();cap++){
				task_allocations_.push_back(new TaskAlloc(this,rd,cap));
			}
		}
	}

}

CloudTask::~CloudTask()
{
	std::vector <ResDemand*>::iterator iter;
	for (iter = res_demands.begin() ; iter!=res_demands.end(); iter++)
	{
		delete (*iter);
	}
}

void CloudTask::setMIPS(int rd, int cap,double mips) {
	res_demands.at(rd)->capacity.at(cap) = mips;
}

double CloudTask::getMIPS(int rd, int cap) {
	if(res_demands.at(rd)->getType() != Computing){
		std::cerr << "MIPS requested for non-Computing resource";
		abort();
	}
	return res_demands.at(rd)->capacity.at(cap);}

void CloudTask::fail(ResourceProvider* provider){
//	std::cerr << "Task\t" << id_ <<"\tfailed on provider:\t" << provider->id_ <<"\n";
	failed_ = true;
	if(this->started_==true){
	provider->releaseAllocation(this);
	}
	provider->tskFailed_++;
	releaseAllTaskAllocs();
	task_allocations_.clear();
}

void CloudTask::releaseAllTaskAllocs(){
	std::vector<TaskAlloc*>::iterator iter;
	for(iter = task_allocations_.begin(); iter!=task_allocations_.end(); iter++){
		if((*iter)->getCoreScheduler()!=NULL){
		(*iter)->getCoreScheduler()->removeTaskAlloc((*iter));
		}
	}
}
void CloudTask::removeTaskAlloc(TaskAlloc* ta){
	task_allocations_.erase(remove(task_allocations_.begin(),task_allocations_.end(),ta),
			task_allocations_.end()); /*erase-remove idiom*/
}

bool CloudTask::isFinished(){
	std::vector <ResDemand*>::iterator u_res;

	/* //Check if all computational load is finished */
	if(task_allocations_.size()>0){
		return false;
	} else {
		return true;
	}
}


void CloudTask::printCompCapacites(){
	std::vector <ResDemand*>::iterator u_res;
	std::cerr << "Capacities: ";
	for (u_res = res_demands.begin() ; u_res!=res_demands.end(); u_res++)
	{
		if((*u_res)->getType()==Computing){
			std::vector <Capacity>::iterator cap;
			for(cap=(*u_res)->capacity.begin();cap!=(*u_res)->capacity.end();cap++){
				std::cerr << (*cap) << " ";
			}
		}
	}
	std::cerr << "\n";
}


