/*
 * randomscheduler.cc
 *
 *  	@date Jan 9, 2014
 *      @author Guzek:Mateusz
 */

#include "randomscheduler.h"

RandomScheduler::RandomScheduler() {


}

RandomScheduler::~RandomScheduler() {

}

TskComAgent* RandomScheduler::scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers){
	return ProbabilisticScheduler::scheduleTask(task,providers);
}

double RandomScheduler::calculateScore(ResourceProvider* rp){
	double result = 1;
	return result;
}

