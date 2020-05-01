/*
 * multidens.h
 *
 *  	@date Jan 8, 2014
 *      @author Guzek:Mateusz
 */

#ifndef MULTIDENS_H_
#define MULTIDENS_H_

#include <algorithm>
#include <math.h>

#include "probabilisticscheduler.h"

/**
 * RandDENS scheduler.
 * To meaningfully use it, enableDVFS on the resource providers used by this scheduler.
 */
class RandDENS : public ProbabilisticScheduler {
public:
	RandDENS();
	virtual ~RandDENS();
	virtual	TskComAgent* scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers);
private:
	double epsilon;
	virtual double calculateScore(ResourceProvider* rp);
	double densLoadFactor(double load,double epsilon);
	double linkLoadFactor(double load);

};

#endif /* MULTIDENS_H_ */
