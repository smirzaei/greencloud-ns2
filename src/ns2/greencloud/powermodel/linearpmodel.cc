/*
 * linearpmodel.cc
 *
 *  @date Feb 25, 2014
 *  @author Guzek:Mateusz
 */

#include "linearpmodel.h"
#include "../resource.h"

static class LinearPModelClass : public TclClass {
public:
	LinearPModelClass() : TclClass("LinearPModel") {}
	TclObject* create(int argc, const char*const*argv) {
		return (new LinearPModel());
	}
} class_powermodel;


LinearPModel::LinearPModel() {

	/*LastResType+2 = +1 for elements number, +1 for the intercept:*/
	coefficients = new double[LastResType+2];
	initialized = new bool[LastResType+2];
	setCoefNumber(LastResType+2);
	for(int i = 0; i < LastResType+2; i++){
		coefficients[i]= 0;
		initialized[i]=false;
	}

}

LinearPModel::~LinearPModel() {

	delete[] coefficients;
	delete[] initialized;
	name_.clear();
}

void LinearPModel::setCoefNumber(int number){

	coef_number = number;
	if(coefficients != NULL){
	delete[] coefficients;
	delete[] initialized;
	}
	coefficients = new double[coef_number];
	initialized = new bool[coef_number];
}


void LinearPModel::addComponent(DcResource* component){
	/* Linear power model does not accept components*/
	return;
}

double LinearPModel::estimate(int size, double* predictors){
	if(ready){
		if(size!=  coef_number - 1){
			std::cerr <<"Incorrect size of predictors array!\n";
		}
		double result = coefficients[size];
		for(int i = 0; i < size; i++){
			result += predictors[i] * coefficients[i];
		}
		return result;
	} else {
		std::cerr << "The model is not correctly initalized.\n" ;
		print();
		std::cerr << "Aborting simulation";
		abort();
	}
}

double LinearPModel::getMaxPower(){
	double * load = new double[coef_number];
	for(int i = 0; i < coef_number; i++){
		load[i] = 1;
	}
	return estimate(coef_number,load);
}



void LinearPModel::print(){
	std::cout << "Linear model: "<< name_ << "\n";
	if(ready){
		std::cout << "Coefficients:\n";
		for(int i = 0; i < coef_number ; i++){
			std::cout << i << ": " << coefficients[i] << "\n";
		}
	} else {
		std::cout << "Model not initalized properly\n";
	}
}

void LinearPModel::setCoefficient(const char* coef,double value){
	if(strcmp(coef, "Intercept") != 0){
		res_type type = Resource::translateType(coef);
		coefficients[type]=value;
		initialized[type]=true;
	} else {
		coefficients[coef_number-1]= value;
		initialized[coef_number-1]=true;
	}
	updateInit();
}

void LinearPModel::setCoefficientNumeric(const char* coef,double value){
	if(strcmp(coef, "Intercept") != 0){
		int i = atoi(coef);
		coefficients[i]=value;
		initialized[i]=true;
	} else {
		coefficients[coef_number-1]= value;
		initialized[coef_number-1]=true;
	}
	updateInit();
}
void LinearPModel::updateInit(){
	bool result = true;
	for(int i = 0; i < coef_number-1; i++){
		result = result && initialized[i];
	}
	ready = result;
}

int LinearPModel::command(int argc, const char*const* argv)
{

	if (argc == 2) {
		if (strcmp(argv[1], "print") == 0) {
			/* print general info */
			print();
			return (TCL_OK);
		}
	} else if (argc==3){
		if (strcmp(argv[1], "set-name") == 0) {
			setName(argv[2]);
			return(TCL_OK);
		} else if (strcmp(argv[1], "set-coef-number") == 0) {
			setCoefNumber(atoi(argv[2]));
			return(TCL_OK);
		} else {
			return(TCL_ERROR);
		}
	}
	else if (argc == 4) {
		if (strcmp(argv[1], "set-coefficient") == 0) {
			setCoefficient(argv[2],atof(argv[3]));
			return(TCL_OK);
		} else if (strcmp(argv[1], "set-coefficient-numeric") == 0) {
			setCoefficientNumeric(argv[2],atof(argv[3]));
			return(TCL_OK);
		} else {
			return(TCL_ERROR);
		}
	}
	return (TCL_ERROR);
}

