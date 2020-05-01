/*
 * multidens.h
 *
 *  	@date Jan 8, 2014
 *      @author Guzek:Mateusz
 */

#ifndef BESTDENS_H_
#define BESTDENS_H_

#include <algorithm>
#include <math.h>

#include "bestscorescheduler.h"

/**
 * BestDENS scheduler.
 * To meaningfully use it, enableDVFS on the resource providers used by this scheduler.
 * TODO: add networking part of DENS.
 */
class BestDENS : public BestScoreScheduler {
public:
	BestDENS();
	virtual ~BestDENS();
	virtual	TskComAgent* scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers);
private:
	double epsilon;
	virtual double calculateScore(ResourceProvider* rp);
	double densLoadFactor(double load,double epsilon);
	double linkLoadFactor(double load);

};

#endif /* MULTIDENS_H_ */
