/*
 * scorescheduler.h
 *
 *  Created on: Jan 23, 2014
 *      Author: greencloud
 */

#ifndef SCORESCHEDULER_H_
#define SCORESCHEDULER_H_

#include "dcscheduler.h"

class ScoreScheduler: public DcScheduler {
public:
	ScoreScheduler();
	virtual ~ScoreScheduler();
private:
	virtual double calculateScore(ResourceProvider* rp) = 0;
};

#endif /* SCORESCHEDULER_H_ */
