/*
 * resourceconsumer.cc
 *
 *  @date Mar 11, 2013
 *  @author Guzek:Mateusz
 */

#include "resourceconsumer.h"

ResourceConsumer::ResourceConsumer() {
	res_demands.clear();
}

ResourceConsumer::ResourceConsumer(unsigned int size,std::vector<Resource *> demand, bool isTask, bool isVM) : isTask(isTask), isVM(isVM), size_(size){

	res_demands.clear();
	res_demands = std::vector<ResDemand *>(demand.size(),NULL);
	std::vector <Resource*>::iterator iter;
	std::vector <ResDemand*>::iterator iter2;
	for (iter = demand.begin(), iter2=res_demands.begin(); iter!=demand.end(); iter++,iter2++)
	{
		(*iter2)=new ResDemand(*(*iter));
	}

	for (iter = demand.begin() ; iter!=demand.end(); iter++)
	{
		delete (*iter);
	}
}

ResourceConsumer::~ResourceConsumer() {
	std::vector <ResDemand*>::iterator iter2;
	for (iter2 = res_demands.begin() ; iter2!=res_demands.end(); iter2++)
	{
		delete (*iter2);
	}
}

unsigned int ResourceConsumer::getSize()
{return size_;};

void ResourceConsumer::setSize(unsigned int size)
{size_ = size;};
