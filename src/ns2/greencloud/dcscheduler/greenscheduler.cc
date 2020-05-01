/*
 * greenscheduler.cc
 *
 *  	@date Jan 8, 2014
 *      @author Guzek:Mateusz
 */

#include "greenscheduler.h"


GreenScheduler::GreenScheduler() {


}

GreenScheduler::~GreenScheduler() {

}

TskComAgent* GreenScheduler::scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers){
	vector <ResourceProvider*>::iterator iter;

	for (iter = providers.begin(); iter!=providers.end(); iter++)
	{
		if ((*iter)->trySchedulingTsk(task))
			return (*iter)->getTskComAgent();
	}
	return NULL;
}

