/*
 * roundrobinscheduler.h
 *
 *  	@date Jan 8, 2014
 *      @author Guzek:Mateusz
 */

#ifndef ROUNDROBINSCHEDULER_H_
#define ROUNDROBINSCHEDULER_H_

#include "dcscheduler.h"

class RoundRobinsScheduler : public DcScheduler{
public:
	RoundRobinsScheduler();
	virtual ~RoundRobinsScheduler();
	virtual TskComAgent* scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers);
};

#endif /* ROUNDROBINSCHEDULER_H_ */
