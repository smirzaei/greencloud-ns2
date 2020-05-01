/*
 * resourcespec.cc
 *
 *  @date Feb 27, 2013
 *  @author Guzek:Mateusz
 */

#include "resourcespec.h"


static class ResourceSpecClass : public TclClass {
public:
	ResourceSpecClass() : TclClass("ResourceSpec") {}
	TclObject* create(int argc, const char*const*argv) {
		return (new ResourceSpec());
	}
} class_resourcespec;

ResourceSpec::ResourceSpec() :  name_("NA"), res_model_(NULL){
	//	std::cerr << ("Resource Spec Constructor.\n");
	capacity.clear();
	power_states.clear();
}

ResourceSpec::~ResourceSpec() {
	name_.clear();
}

int ResourceSpec::addCapacity(double cap){
	capacity.push_back(cap);
	return 0;
}
int ResourceSpec::addPowerState(int ps){
	power_states.push_back(ps);
	return 0;
}


int ResourceSpec::setName(const char* name){
	name_=name;
	return 0;
}

int ResourceSpec::setArch(const char* name){
	arch = atof(name);
	return 0;
}

void ResourceSpec::setPowerModel(PowerModel* model){
	res_model_  = model;
}

PowerModel* ResourceSpec::getPowerModel(){
	return res_model_;
}

void ResourceSpec::print(){
	std::cerr << "ResourceSpec:\t";
	std::cerr << name_;
	std::cerr << "\n";
	Resource::print();
	std::vector <int>::iterator iter;
	std::cerr << "Power states:\t";
	for (iter = power_states.begin(); iter!=power_states.end(); iter++)
	{
		std::cerr << (*iter) << ",";
	}
	std::cerr << "\n";

}



int ResourceSpec::command(int argc, const char*const* argv){


	if (argc == 2) {
		if (strcmp(argv[1], "print") == 0) {
			print();
			return (TCL_OK);
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "add-capacity") == 0) {
			addCapacity(atof(argv[2]));
			return (TCL_OK);
		} else if(strcmp(argv[1], "add-power-state") == 0){
			addPowerState(atoi(argv[2]));
			return (TCL_OK);
		} else if(strcmp(argv[1], "set-type") == 0){
			if(Resource::setType(argv[2])==0){
				return (TCL_OK);
			} else {
				return (TCL_ERROR);
			}
		} else if(strcmp(argv[1], "set-name") == 0){
			setName(argv[2]);
			return (TCL_OK);
		} else if(strcmp(argv[1], "set-arch") == 0){
			setArch(argv[2]);
			return (TCL_OK);
		} else if (strcmp(argv[1], "set-power-model") == 0) {
			PowerModel* pm = (PowerModel*) TclObject::lookup(argv[2]);
			this->setPowerModel(pm);
			return (TCL_OK);
		}
	}
	return (ResourceSpec::command(argc, argv));
}





