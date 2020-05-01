/*
 * greenscheduler.h
 *
 *  	@date Jan 8, 2014
 *      @author Guzek:Mateusz
 */

#ifndef GREENSCHEDULER_H_
#define GREENSCHEDULER_H_

#include "dcscheduler.h"

class GreenScheduler : public DcScheduler{
public:
	GreenScheduler();
	virtual ~GreenScheduler();
	virtual TskComAgent* scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers);
};

#endif /* GREENSCHEDULER_H_ */
