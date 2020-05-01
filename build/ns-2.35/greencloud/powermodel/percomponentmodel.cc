/*
 * percomponentmodel.cc
 *
 *  @date Feb 25, 2014
 *  @author Guzek:Mateusz
 */

#include "percomponentmodel.h"
#include "../dcresource.h"

static class PerComponentModelClass : public TclClass {
public:
	PerComponentModelClass() : TclClass("PerComponentModel") {}
	TclObject* create(int argc, const char*const*argv) {
		return (new PerComponentModel());
	}
} class_powermodel;

PerComponentModel::PerComponentModel() {


}

PerComponentModel::~PerComponentModel() {

}

void PerComponentModel::addComponent(DcResource* component){
	modeled_components_.push_back(component);
}

int PerComponentModel::command(int argc, const char*const* argv){
	if (argc == 2) {
		if (strcmp(argv[1], "print") == 0) {
			/* print general info */
			print();
			return (TCL_OK);
		}
	} else if (argc==3){
		if (strcmp(argv[1], "set-name") == 0) {
			this->setName(argv[2]);
			return(TCL_OK);
		} else {
			return(TCL_ERROR);
		}
	}
	return(TCL_ERROR);

}
double PerComponentModel::estimate(int size, double* predictors){
	std::vector<DcResource*>::iterator iter;
	double result = 0;
	for(iter = modeled_components_.begin(); iter != modeled_components_.end(); iter++){
		result+=(*iter)->getPower();
	}
	return result;
}

double PerComponentModel::getMaxPower(){
	std::vector<DcResource*>::iterator iter;
	double result = 0;
	for(iter = modeled_components_.begin(); iter != modeled_components_.end(); iter++){
		result+=(*iter)->getMaxPower();
	}
	return result;
}


void PerComponentModel::print(){
	std::cout << "Per component power model.\nModeled components:\n";
	std::vector<DcResource*>::iterator iter;
	for(iter = modeled_components_.begin(); iter != modeled_components_.end(); iter++){
		std::cout << (*iter)->specification->getPowerModel()->name_ << "\n";
	}

}

