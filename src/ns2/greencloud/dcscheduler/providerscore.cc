/*
 * providerscore.cc
 *
 *  Created on: Jan 23, 2014
 *      Author: greencloud
 */

#include "providerscore.h"

ProviderScore::ProviderScore(ResourceProvider* provider, double score): provider_(provider),score_(score){


}

ProviderScore::ProviderScore(ResourceProvider* provider, double score, double comm_potential_): provider_(provider),score_(score), comm_potential_(comm_potential_){


}



ProviderScore::~ProviderScore() {

}

bool ProviderScore::operator<(const ProviderScore &other) const {
   return this->score_ < other.score_;
}


