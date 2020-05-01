/*
 * scheduler.h
 *
 *  	@date Jan 8, 2014
 *      @author Guzek:Mateusz
 */

#ifndef DCSCHEDULER_H_
#define DCSCHEDULER_H_

#include "tskagent.h"
#include "cloudtask.h"
#include "resourceprovider.h"

class DcScheduler {
public:
	DcScheduler();
	virtual ~DcScheduler();
	virtual TskComAgent* scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers) = 0;
};

#endif /* SCHEDULER_H_ */
