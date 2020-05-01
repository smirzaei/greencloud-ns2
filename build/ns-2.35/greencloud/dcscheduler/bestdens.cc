/*
 * bestdens.cc
 *
 *  	@date Oct 29, 2014
 *      @author Guzek:Mateusz
 */

#include "bestdens.h"
#include "dcrack.h"

BestDENS::BestDENS() : epsilon(0.1){

}

BestDENS::~BestDENS() {

}

TskComAgent* BestDENS::scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers){
	return BestScoreScheduler::scheduleTask(task,providers);
}

double BestDENS::calculateScore(ResourceProvider* rp){
	double result = 0;

	return result;
}

double BestDENS::densLoadFactor(double load,double epsilon){
	return 0;
}

double BestDENS::linkLoadFactor(double load){
	return 0;
}

