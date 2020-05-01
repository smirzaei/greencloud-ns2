/*
 * corescheduler.cc
 *
 *  @date Aug 29, 2013
 *  @author Guzek:Mateusz
 */

#include "corescheduler.h"
#include "cloudtask.h"
#include "resourceprovider.h"

CoreScheduler::CoreScheduler(Capacity * nominal_mips_,Capacity * available_mips_) : current_mips_(0.0), host_scheduler_(NULL), tskAllocFailed_(0) {
	this->nominal_mips_=nominal_mips_;
	this->available_mips_=available_mips_;
	tasks_alloc_list_.clear();
	tasks_alloc_assigned_.clear();
	hosted_vcores_schedulers.clear();

}



CoreScheduler::~CoreScheduler() {
	std::vector <TaskAlloc*>::iterator task;
	for(task = tasks_alloc_list_.begin(); task!=tasks_alloc_list_.end() ;task++){
		delete (*task);
	}
	tasks_alloc_list_.~vector();
	for(task = tasks_alloc_assigned_.begin(); task!=tasks_alloc_assigned_.end() ;task++){
		delete (*task);
	}
	tasks_alloc_assigned_.~vector();
}

void CoreScheduler::setProvider(ResourceProvider * provider){
	this->provider = provider;
}

void CoreScheduler::setDVFS(int eDVFS_enabled_){
	this->eDVFS_enabled_ = eDVFS_enabled_;
}

double CoreScheduler::getCurrentMIPS(){
	return current_mips_;
}
double CoreScheduler::getCurrentMIPSRecursive(){
	double result = getCurrentMIPS();
	std::vector<CoreScheduler*>::iterator iter;
	int i = 1;
	for(iter = hosted_vcores_schedulers.begin(); iter != hosted_vcores_schedulers.end();iter++){
		result += (*iter)->getCurrentMIPSRecursive();
		i++;
	}
	return result;
}
double CoreScheduler::getNominalMIPS(){
	return *nominal_mips_;
}
double CoreScheduler::getAvailableMIPS(){
	return *available_mips_;
}

ResourceProvider * CoreScheduler::getProvider(){
	return provider;
}

int CoreScheduler::getAllTasksNumber(){
	return tasks_alloc_list_.size() + tasks_alloc_assigned_.size();
}

void CoreScheduler::assignTask(TaskAlloc* tskobj){
	tasks_alloc_assigned_.push_back(tskobj);
	tskobj->setCoreScheduler(this);
}

void CoreScheduler::executeTask(TaskAlloc* tskobj){
	tasks_alloc_list_.push_back(tskobj);	// add to the active tasks links
	tskobj->setCoreScheduler(this);
}

bool CoreScheduler::removeFromAssginedList(TaskAlloc* tskobj) {
	vector<TaskAlloc*>::iterator iter;
	for (iter = tasks_alloc_assigned_.begin(); iter != tasks_alloc_assigned_.end();iter++)
	{
		/* task received remove from in-fly list */
		if ((*iter) == tskobj) {
			tasks_alloc_assigned_.erase(iter);
			return true;
		}
	}
	return false;
}

bool CoreScheduler::removeAllocationsFromAssginedList(CloudTask* tskobj) {
	vector<TaskAlloc*>::iterator iter;
	bool found = false;
	for (iter = tasks_alloc_assigned_.begin(); iter != tasks_alloc_assigned_.end();)
	{
		/* task received remove from in-fly list */
		if ((*iter)->cloudTask->id_ == tskobj->id_) {
			iter = tasks_alloc_assigned_.erase(iter);
			found = true;
		} else {
			iter++;
		}
	}
	return found;
}

void CoreScheduler::startTaskExecution(CloudTask* tskobj) {
	vector<TaskAlloc*>::iterator iter;
	for (iter = tasks_alloc_assigned_.begin(); iter != tasks_alloc_assigned_.end();)
	{
		/* task received remove from in-fly list */
		if ((*iter)->cloudTask->id_ == tskobj->id_) {
			executeTask((*iter));
			(*iter)->setExecTime(Scheduler::instance().clock());
			iter = tasks_alloc_assigned_.erase(iter);

		} else {
			iter++;
		}
	}
	if(!tskobj->started_){
		tskobj->started_ = true;
	}
	this->updateTskList();
}


CloudTask* CoreScheduler::removeTaskAlloc( vector<TaskAlloc*>::iterator& iter,bool executed)
{
	CloudTask* result = (*iter)->cloudTask;
	this->provider->updateEnergyAndConsumption();

	(*iter)->cloudTask->removeTaskAlloc(*iter);
	if(executed){
		iter = tasks_alloc_list_.erase(iter);
	}
	else {
		//The tasks that fails while being on assigned list
		//was never executed, so cannot finish successfully
		iter = tasks_alloc_assigned_.erase(iter);
	}
	return result;
}

void CoreScheduler::removeTaskAlloc( TaskAlloc* ta)
{
	this->provider->updateEnergyAndConsumption();
	tskAllocFailed_++;
	tasks_alloc_list_.erase(remove(tasks_alloc_list_.begin(),tasks_alloc_list_.end(),ta),
			tasks_alloc_list_.end()); /*erase-remove idiom*/
	tasks_alloc_assigned_.erase(remove(tasks_alloc_assigned_.begin(),tasks_alloc_assigned_.end(),ta),
			tasks_alloc_assigned_.end()); /*erase-remove idiom*/
}

void CoreScheduler::removeFailedTaskAlloc(vector<TaskAlloc*>::iterator& iter,bool executed)
{
	if((*iter)->cloudTask->failed_==false){
		(*iter)->cloudTask->fail(this->provider);
	}
	// REMOVE ALL TASK ALLOCATIONS FOR THE CLOUDTASK OF THIS TASK_ALLOC
	if(executed){
		iter = tasks_alloc_list_.begin();
	} else {
		iter = tasks_alloc_assigned_.begin();
	}
}


// TODO the deadline check should happen before removal of the completed tasks!
void CoreScheduler::removeCompleted()
{
	std::vector<TaskAlloc*>::iterator iter;

	/* remove completed tasks from the execution list */
	for (iter = tasks_alloc_list_.begin(); iter != tasks_alloc_list_.end();)
	{
		/* task should be completed and remove it from the list */
		if ((*iter)->getMIPS() <= 1) {
			CloudTask* ct = removeTaskAlloc(iter,true);
			//check if finished:
			if(ct->isFinished()){
				provider->releaseAllocation(ct);
				provider->sendTaskOutput(ct);
			}
		}
		/*Task run over its deadline, remove it and mark as failed. */
		else if(Scheduler::instance().clock() >= (*iter)->cloudTask->getDeadline()) {
			removeFailedTaskAlloc(iter,true);
		} else if ((*iter)->cloudTask->failed_==true){
			std::cerr << "This should not happen!\n";
		}
		else {
			iter++;
		}
	}
}

double CoreScheduler::getMostUrgentTaskRate()
{
	std::vector<TaskAlloc*>::iterator iter;

	/* update what is already computing */

	for (iter = tasks_alloc_list_.begin(); iter != tasks_alloc_list_.end(); iter++)
	{
		(*iter)->updateMIPS();	// update what is already computing
	}
	/* remove completed */
	removeCompleted();

	/* Compute highest MIPS/deadline ratio */
	double maxrate = 0.0;

	/* get most urgent task rate from the execution list */
	for (iter = tasks_alloc_list_.begin(); iter != tasks_alloc_list_.end();)
	{
		if((double)((*iter)->getDeadline() - Scheduler::instance().clock())>0 && (*iter)->cloudTask->failed_==false){
			double rate = (double)(*iter)->getMIPS()/(double)((*iter)->getDeadline() - Scheduler::instance().clock());
			if(rate>maxrate){
				maxrate = rate;
			}
			iter++;
		}
		else {
			removeFailedTaskAlloc(iter,true);
		}
	}

	/* get most urgent task rate from the in-fly list */
	for (iter = tasks_alloc_assigned_.begin(); iter != tasks_alloc_assigned_.end();)
	{
		if((double)((*iter)->getDeadline() - Scheduler::instance().clock())>0 && (*iter)->cloudTask->failed_==false){
			double rate = (double)(*iter)->getMIPS()/(double)((*iter)->getDeadline() - Scheduler::instance().clock());
			if(rate>maxrate){
				maxrate = rate;
			}
			iter++;
		} else {
			removeFailedTaskAlloc(iter,false);
		}
	}
	if (maxrate > getAvailableMIPS()){return getAvailableMIPS();}
	else{return maxrate;}
}



void CoreScheduler::updateTskComputingRates(double c_mips)
{
	vector<TaskAlloc*>::iterator iter;


	for (iter = tasks_alloc_list_.begin(); iter != tasks_alloc_list_.end(); iter++)
	{
		/* each task with then update mips left */
		(*iter)->setComputingRate((double)c_mips/tasks_alloc_list_.size());
	}
}

void CoreScheduler::updateTskList(double c_mips)
{

	vector<TaskAlloc*>::iterator iter;
	if (tasks_alloc_list_.size()==0) return;
//	std::cout << "Prov: " << provider->id_ << "\n";
	/* update task computing rates to see which tasks are completed */
	updateTskComputingRates(c_mips);
	removeCompleted();

	/* set server computing rate */
	setComputingRate();

	/* compute next deadline */
	double nextDeadline = DBL_MAX;
	for (iter = tasks_alloc_list_.begin(); iter != tasks_alloc_list_.end(); iter++)
	{
		if (nextDeadline > (*iter)->execTime()){
			nextDeadline = (*iter)->execTime();
		}
	}


	provider->scheduleNextExent(nextDeadline);

}

void CoreScheduler::updateTskList(){
	updateTskList(this->current_mips_);
}

void CoreScheduler::setComputingRate()
{
	if (eDVFS_enabled_) {
		/* Max requested rate times the number of active taks */
		current_mips_ = getMostUrgentTaskRate()* tasks_alloc_list_.size();
	} else {
		/* no energy saving */
		if (tasks_alloc_list_.size() != 0){current_mips_ = getAvailableMIPS();}
		else {(current_mips_) = 0;}
	}
	/* new computing rate, report it to tasks */
	updateTskComputingRates(current_mips_);
}

void CoreScheduler::addVcoreScheduler(CoreScheduler* cs){
	hosted_vcores_schedulers.push_back(cs);
	cs->host_scheduler_ = this;
}

void CoreScheduler::removeVcoreScheduler(CoreScheduler* cs){
	hosted_vcores_schedulers.erase(remove(hosted_vcores_schedulers.begin(),hosted_vcores_schedulers.end(),cs),
			hosted_vcores_schedulers.end()); /*erase-remove idiom*/
	cs->host_scheduler_ = NULL;
}

CoreScheduler * CoreScheduler::getHostScheduler(){
	return host_scheduler_;
}

