/*
 * roundrobinscheduler.cc
 *
 *  	@date Jan 8, 2014
 *      @author Guzek:Mateusz
 */

#include "roundrobinscheduler.h"

RoundRobinsScheduler::RoundRobinsScheduler() {


}

RoundRobinsScheduler::~RoundRobinsScheduler() {

}

TskComAgent* RoundRobinsScheduler::scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers){
	int j = task->id_ % providers.size();

	return (providers.at(j)->getTskComAgent());
}
