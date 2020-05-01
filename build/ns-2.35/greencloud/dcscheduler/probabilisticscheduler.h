/*
 * probabilisticscheduler.h
 *
 *  	@date Jan 9, 2014
 *      @author Guzek:Mateusz
 */

#ifndef PROBABILISITCSCHEDULER_H_
#define PROBABILISITCSCHEDULER_H_

#include <algorithm>

#include "scorescheduler.h"

class ProbabilisticScheduler : public ScoreScheduler{
public:
	ProbabilisticScheduler();
	virtual ~ProbabilisticScheduler();
	virtual TskComAgent* scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers);
private:
	virtual double calculateScore(ResourceProvider* rp) = 0;

};

#endif /* PROBABILISITCSCHEDULER_H_ */
