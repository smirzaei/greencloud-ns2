/*
 * herosscheduler.h
 *
 * Energy
 *
 *  Please cite
 * 	HEROS: Energy-Efficient Load Balancing for Heterogeneous Data Centers
 *	Mateusz Guzek, Dzmitry Kliazovich, Pascal Bouvry
 *	IEEE CLOUD 2015
 *	Available upon request at: http://hdl.handle.net/10993/21417
 *	(exact bibliography reference TBA)
 *
 *  Created on: Jan 23, 2014
 *      Author: Guzek:Mateusz
 */

#ifndef HEROSSCHEDULER_H_
#define HEROSSCHEDULER_H_

#include "bestscorescheduler.h"



class HerosScheduler: public BestScoreScheduler {
public:
	HerosScheduler();
	virtual ~HerosScheduler();
	virtual TskComAgent* scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers);
private:
	virtual double calculateScore(ResourceProvider* rp);
	double static densLoadFactor(double load,double epsilon);
	double static linkLoadFactor(double load);
	double static performancePerWatt(ResourceProvider* rp);
public:
	double static performancePerWattMax(ResourceProvider* rp);
private:
	double static herosTransformation(ResourceProvider* rp,double alpha, double beta, double gamma);
	double epsilon;

};

bool herosComparator (const ProviderScore &first,const ProviderScore &second);

#endif /* HEROSSCHEDULER_H_ */
