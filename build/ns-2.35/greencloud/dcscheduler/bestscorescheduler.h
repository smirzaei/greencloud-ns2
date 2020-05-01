/*
 * bestscorescheduler.h
 *
 *  Created on: Jan 23, 2014
 *      Author: greencloud
 */

#ifndef BESTSCORESCHEDULER_H_
#define BESTSCORESCHEDULER_H_

#include "scorescheduler.h"
#include "providerscore.h"

class BestScoreScheduler: public ScoreScheduler {
public:
	BestScoreScheduler();
	virtual ~BestScoreScheduler();
	virtual TskComAgent* scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers);
private:
	virtual double calculateScore(ResourceProvider* rp) = 0;
};

#endif /* BESTSCORESCHEDULER_H_ */
