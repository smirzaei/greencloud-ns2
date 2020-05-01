/*
 * multidens.cc
 *
 *  	@date Jan 8, 2014
 *      @author Guzek:Mateusz
 */

#include "randdens.h"
#include "dcrack.h"

RandDENS::RandDENS() : epsilon(0.1){

}

RandDENS::~RandDENS() {

}

TskComAgent* RandDENS::scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers){
	return ProbabilisticScheduler::scheduleTask(task,providers);
}

double RandDENS::calculateScore(ResourceProvider* rp){
	double result = 0;
	double load;
	for(int i = FirstResType; i <= LastResType ; i++){
		load = rp->getResTypeUtil(static_cast<res_type>(i));
		result+= densLoadFactor(load,epsilon);
	}
	result=result/(LastResType+1); // normalize according to the number of dimensions

	result += linkLoadFactor(rp->getRootHost()->rack_->link_load);

	return result;
}

double RandDENS::densLoadFactor(double load,double epsilon){
	return 1/(1+exp(-10*(load-0.5))) - 1/(1+exp((-10/epsilon)*(load-(1-(epsilon/2)))));
}

double RandDENS::linkLoadFactor(double load){
	return exp(-(load*load));
}
