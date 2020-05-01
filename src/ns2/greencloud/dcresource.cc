/*
 * dcresource.cc
 *
 *  @date Feb 27, 2013
 *  @author Guzek:Mateusz
 */

#include "dcresource.h"

static class DcResourceClass : public TclClass {
public:
	DcResourceClass() : TclClass("DcResource") {}
	TclObject* create(int argc, const char*const*argv) {
		return (new DcResource());
	}
} class_dcresource;


DcResource::DcResource()  {

}

DcResource::~DcResource() {

}

/*Set specification, initialize usage as vector of 0.*/
int DcResource::setSpecification(ResourceSpec* resspec){
	if(resspec==NULL){
		std::cerr << "ERROR: Null pointer passed as ResourceSpec.";
		return 1;
	}
	specification=resspec;
	capacity = resspec->capacity;
	used_power_state_ = specification->power_states.at(0);
	type=resspec->type;
	arch=resspec->arch;
	total_cap = 0;
	std::vector<Capacity>::iterator iter;
	for(iter = resspec->capacity.begin();iter!= resspec->capacity.end();iter++){
		total_cap += iter->value;
	}
	return 0;
}

double DcResource::getUtilization(){
	double free = 0;

	std::vector<Capacity>::iterator iter;
	for(iter=capacity.begin(); iter!= capacity.end();iter++){
		free += iter->getValueRecursive();

	}
	return 1-(free/total_cap);
}


double DcResource::getPower(){
	int n = 1;
	double* utilization = new double[n];
	utilization[0] = getUtilization();
	double result =  specification->getPowerModel()->estimate(n,utilization);
	delete utilization;
	return result;
}


double DcResource::getMaxPower(){
	int n = 1;
	double* utilization = new double[n];
	utilization[0] = 1;
	double result =  specification->getPowerModel()->estimate(n,utilization);
	delete utilization;
	return result;
}





void DcResource::print(){
	std::cout << "DcResource";
	std::cout << "\n";
	specification->print();
	std::cout << "Available capacities:\t";
	std::vector <Capacity>::iterator iter;
	for (iter = capacity.begin(); iter!=capacity.end(); iter++)
	{
		std::cout << (*iter) << ",";
	}
	std::cout << "\n";
	std::cout << "Used power state:\t" << used_power_state_;

	std::cout << "\n";
}


int DcResource::command(int argc, const char*const* argv){


	if (argc == 2) {
		if (strcmp(argv[1], "print") == 0) {
			this->print();
			return (TCL_OK);
		}
	}
	return (DcResource::command(argc, argv));
}



