/*
 * herosscheduler.cc
 *
 *  Created on: Jan 23, 2014
 *      Author: Guzek:Mateusz
 */

#include "herosscheduler.h"
#include "dcrack.h"

HerosScheduler::HerosScheduler() : epsilon(0.1){


}

HerosScheduler::~HerosScheduler() {

}


double HerosScheduler::calculateScore(ResourceProvider* rp){
	double result = 0;

	result = herosTransformation(rp,110,0.90,1.2);
//	std::cerr << "Heros transformation result" << result << "\n";
	result *= pow(linkLoadFactor(rp->getRootHost()->rack_->link_load),2);
//	std::cerr << "Final result" << result << "\n";
	return result;
}


double HerosScheduler::densLoadFactor(double load,double epsilon){
	return 1/(1+exp(-10*(load-0.5))) - 1/(1+exp((-10/epsilon)*(load-(1-(epsilon/2)))));
}

double HerosScheduler::performancePerWatt(ResourceProvider* rp){
	if(rp->getRootHost()->eCurrentConsumption_==0)
	{
		return 0;
	}
	return (rp->getResTypeUtil(Computing)) * rp->getTotalCap(Computing) / rp->getRootHost()->eCurrentConsumption_ ;
}

double HerosScheduler::performancePerWattMax(ResourceProvider* rp){

	double result =  rp->getTotalCap(Computing) / rp->getRootHost()->powerModel->getMaxPower();
	//	std::cerr << "Ppw max: " <<result << "\n";
	return result;
}

double HerosScheduler::herosTransformation(ResourceProvider* rp,double alpha, double beta, double gamma){

	double maxl = rp->getTotalCap(Computing);
	//	std::cerr << "Ppw current: " <<performancePerWatt(rp) << "\n";
	double result =  performancePerWatt(rp);
			if(rp->getResTypeUtil(Computing) > beta/2){
			result -= gamma * performancePerWatt(rp) *
			1/
			(
					1+exp
					(
							-(alpha/maxl)*(rp->getResTypeUtil(Computing)*maxl - (beta * maxl ) )
					)
			);
			}

	return result;
}

double HerosScheduler::linkLoadFactor(double load){
	return exp(-(pow(2*load,2)));
}



TskComAgent* HerosScheduler::scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers){
	//	std::cerr<< "HEROS is making decision:\n";
	vector<ProviderScore> scored_providers_;
	vector <ResourceProvider*>::iterator iter;
	for (iter = providers.begin(); iter!=providers.end(); iter++)
	{
		if ((*iter)->testSchedulingPossibility(task)){
			scored_providers_.push_back(ProviderScore((*iter),calculateScore((*iter)),linkLoadFactor((*iter)->getRootHost()->rack_->link_load)));
		}
	}
	if(scored_providers_.empty()){
		return NULL;
	} else {
		vector <ProviderScore>::iterator sp;
		sort(scored_providers_.begin(),scored_providers_.end(),herosComparator);
		vector<ProviderScore>::reverse_iterator rsp = scored_providers_.rbegin();
		ProviderScore best = *rsp;
		int max_n = 0;
		for (; rsp != scored_providers_.rend(); rsp++ ) {
			if(!herosComparator((*rsp),best)){
				++max_n;
			} else {
				break;
			}
		}
		if(max_n!=1){
		int selected = (double)rand() / (double)RAND_MAX * max_n +1;
			best = scored_providers_.at(scored_providers_.size()-selected);
		}

		scored_providers_.clear();
//		std::cerr<< "Selected prov: " << best.provider_->id_ << "\tScore:" << best.score_ <<"\tSelected task:" << task->id_ << "\n";

		return best.provider_->getTskComAgent();
	}
}

bool herosComparator (const ProviderScore &first,const ProviderScore &second){
	if(first.score_ != second.score_){
		 return first.score_ < second.score_;
	} else {
		return HerosScheduler::performancePerWattMax(first.provider_)*first.comm_potential_ < HerosScheduler::performancePerWattMax(second.provider_)*second.comm_potential_;
	}
}

