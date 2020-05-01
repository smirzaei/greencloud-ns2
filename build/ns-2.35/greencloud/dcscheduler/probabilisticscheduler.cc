/*
 * probabilisticscheduler.cc
 *
 *  	@date Jan 9, 2014
 *      @author Guzek:Mateusz
 */

#include "probabilisticscheduler.h"

ProbabilisticScheduler::ProbabilisticScheduler() {


}

ProbabilisticScheduler::~ProbabilisticScheduler() {

}

TskComAgent* ProbabilisticScheduler::scheduleTask(CloudTask* task, std::vector<ResourceProvider* > providers){
	//1. calculate mDENS score array
	//2. generate random number in [0:summed mDENS score]
	//3. binary search on the array
	//4. return the selected host
	TskComAgent* selected = NULL;
	std::vector<double>  mdens_score(providers.size());
	std::vector<double>   mdens_score_cumulative(providers.size());
	vector <ResourceProvider*>::iterator res_p;
	vector <double>::iterator score;
	vector <double>::iterator cumul;
	for (res_p = providers.begin(), score = mdens_score.begin(), cumul = mdens_score_cumulative.begin();
			res_p!=providers.end();
			res_p++, score++, cumul++)
	{
		(*score) = calculateScore((*res_p));
		if(cumul==mdens_score_cumulative.begin()){
			(*cumul) = (*score);
		} else {
			(*cumul) = (*score) + (*(cumul-1));
		}
//		std::cout << "score:" << (*score)<< " cumul: " << (*cumul) << "\n";
	}
//	srand(time(0));
	double r = ( (double)rand() / (double)RAND_MAX ) * mdens_score_cumulative.at(mdens_score_cumulative.size()-1);
//	std::cerr << "Random: " << r  << "\n";
	vector <double>::iterator lb = lower_bound(mdens_score_cumulative.begin(),mdens_score_cumulative.end(),r);
	int sel_ind = lb - mdens_score_cumulative.begin();
//	std::cerr << "Lb: "<< (*lb) << " ind: " << sel_ind << "\n";
//	abort();
	selected = providers.at(sel_ind)->getTskComAgent();
	return selected;
}


