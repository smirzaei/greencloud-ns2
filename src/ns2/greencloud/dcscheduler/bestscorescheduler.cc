/*
 * bestscorescheduler.cc
 *
 *  Created on: Jan 23, 2014
 *      Author: Guzek:Mateusz
 */

#include "bestscorescheduler.h"

BestScoreScheduler::BestScoreScheduler() {


}

BestScoreScheduler::~BestScoreScheduler() {

}

TskComAgent* BestScoreScheduler::scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers){
	vector<ProviderScore> scored_providers_;
	vector <ResourceProvider*>::iterator iter;
	for (iter = providers.begin(); iter!=providers.end(); iter++)
	{
		if ((*iter)->testSchedulingPossibility(task)){
			scored_providers_.push_back(ProviderScore((*iter),calculateScore((*iter))));
		} else {
//			std::cerr << "Provider full!\n";
		}
	}
	if(scored_providers_.empty()){
		return NULL;
	} else {
		ProviderScore best = (*max_element(scored_providers_.begin(),scored_providers_.end()));
		scored_providers_.clear();
		return best.provider_->getTskComAgent();
	}
}

