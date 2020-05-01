/*
 * resource.cc
 *
 *  @date Mar 8, 2013
 *  @author Guzek:Mateusz
 */

#include "resource.h"


	Capacity::Capacity() : value(0.0) {

	}

	Capacity::Capacity(int i) : value(i){

	}

	Capacity::Capacity(double d) : value(d){

	}

Capacity&  Capacity::operator=(const double& val){
	this->value = val;
	return *this;    // Return ref for multiple assignment
}

Capacity&  Capacity::operator=(const Capacity& c){
	if (this != &c) {  // make sure it is not the same object
	this->value = c.value;
	this->virtual_capacities.clear();
	this->virtual_capacities = c.virtual_capacities;
	}
	return *this;    // Return ref for multiple assignment
}

Capacity::operator double(){
	return value;
}

Capacity& Capacity::operator+=(const Capacity& rhs){
	this->value += rhs.value;
	return *this;
}
Capacity& Capacity::operator-=(const Capacity& rhs){
	this->value -= rhs.value;
	return *this;
}

Capacity& Capacity::operator+=(const double& rhs){
	this->value += rhs;
	return *this;
}
Capacity& Capacity::operator-=(const double& rhs){
	this->value -= rhs;
	return *this;
}

double Capacity::getValueRecursive(){
	double result = value;
	std::vector<Capacity*>::iterator iter;
	if(!virtual_capacities.empty()){
		for(iter = virtual_capacities.begin(); iter!=virtual_capacities.end(); iter ++){
			result += (*iter)->getValueRecursive();
		}
	}
	return result;
}

Resource::Resource() {

}

Resource::Resource(res_type t,double a ,std::vector <Capacity> cap){
	type = t;
	arch = a;
	this->setCapacity(cap);
}

Resource::~Resource() {

}

/*Computing, Memory, Storage, Networking, extend according to needs.*/
res_type Resource::translateType(const char* t){
	res_type type;
	if(strcmp(t, "Computing") == 0){
		type=Computing;
	} else if(strcmp(t, "Memory") == 0){
		type=Memory;
	} else if(strcmp(t, "Storage") == 0){
		type=Storage;
	} else if(strcmp(t, "Networking") == 0){
		type=Networking;
	} else {
		std::cerr << "Unknown resource type" << t;
		abort();

	}
	return type;
}

int Resource::setType(const char* t){
	type = translateType(t);
	return 0;
}



int Resource::setCapacity(std::vector <Capacity> cap){
	capacity = cap;
	return 0;
}

void Resource::sortCapacity(){
	std::sort(capacity.begin(),capacity.end());
}

Resource& Resource::operator=(const Resource& r) {
	if (this != &r) {  // make sure it is not the same object
		capacity.clear();
		capacity = r.capacity;
		arch = r.arch;
		type = r.type;
	}
	return *this;    // Return ref for multiple assignment
}

void Resource::print(){
	std::cerr << "Type:\t"<< type;
	std::cerr << "\n";
	std::cerr << "Architecture:\t"<< arch;
	std::cerr << "\n";
	std::cerr << "Capacities:\t";
	std::vector <Capacity>::iterator iter;
	for (iter = capacity.begin(); iter!=capacity.end(); iter++)
	{
		std::cerr << (*iter) << ",";
	}

}
