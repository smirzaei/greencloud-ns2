/*
 * providerscore.h
 *
 *  Created on: Jan 23, 2014
 *      Author: greencloud
 */

#ifndef PROVIDERSCORE_H_
#define PROVIDERSCORE_H_
#include "resourceprovider.h"
class ProviderScore {
public:
	ProviderScore(ResourceProvider* provider, double score);
	ProviderScore(ResourceProvider* provider, double score, double comm_potential_);
	virtual ~ProviderScore();
	ResourceProvider* provider_;
	double score_;
	double comm_potential_;
	bool operator<(const ProviderScore &other) const;
};

#endif /* PROVIDERSCORE_H_ */
