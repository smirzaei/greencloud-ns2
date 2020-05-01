/*
 * randomscheduler.h
 *
 *  	@date Jan 9, 2014
 *      @author Guzek:Mateusz
 */

#ifndef RANDOMSCHEDULER_H_
#define RANDOMSCHEDULER_H_

#include <algorithm>

#include "probabilisticscheduler.h"

class RandomScheduler : public ProbabilisticScheduler{
public:
	RandomScheduler();
	virtual ~RandomScheduler();
	virtual TskComAgent* scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers);
private:
	virtual double calculateScore(ResourceProvider* rp);

};

#endif /* RANDOMSCHEDULER_H_ */
