/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#include "dchost.h"
#include <lib/builtin.h>

static class DcHostClass : public TclClass {
public:
	DcHostClass() : TclClass("DcHost") {}
	TclObject* create(int argc, const char*const*argv) {
		return (new DcHost());
	}
} class_dchost;

DcHost::DcHost(): eConsumed_(0.0), eNominalrate_ (0.0),  eCurrentConsumption_ (0.0),   eDNS_enabled_(0.0) , eLastUpdateTime_(0.0)
{
	bind("id_", &id_);
	bind("ntasks_", &ntasks_);
	bind("currentLoad_", &currentLoad_);
	bind("currentLoadMem_", &currentLoadMem_);
	bind("currentLoadStor_", &currentLoadStor_);
	bind("tskFailed_", &tskFailed_);

	bind("eConsumed_", &eConsumed_);				/* total W of energy consumed */
	bind("eNominalrate_", &eNominalrate_);
	bind("eCurrentConsumption_", &eCurrentConsumption_);		/* current consumption rate */

	bind("eDVFS_enabled_", &eDVFS_enabled_);			/* ON when DVFS is enabled */
	bind("eDNS_enabled_", &eDNS_enabled_);			/* ON when DNS is enabled */

}

DcHost::~DcHost()
{
	delete powerModel;
}

void DcHost::setPowerModel(PowerModel* pModel){
	powerModel = pModel;
}


int DcHost::command(int argc, const char*const* argv)
{
	Tcl& tcl = Tcl::instance();

	if (argc == 2) {
		if (strcmp(argv[1], "start") == 0) {

			/* start counting energy consumed */
			setCurrentConsumption();
			eLastUpdateTime_ = Scheduler::instance().clock();
			started_ = true;
			return (TCL_OK);
		} else if (strcmp(argv[1], "stop") == 0) {
			/* update total energy consumed */
			updateEnergyAndConsumption();
			return (TCL_OK);
		} else if (strcmp(argv[1], "print") == 0) {
			/* print general info */
			print();
			return (TCL_OK);
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "set-power-model") == 0) {
			PowerModel* pModel = (PowerModel*) TclObject::lookup(argv[2]);
			if (pModel == NULL) {
				tcl.resultf("no such power model %s", argv[2]);
				return(TCL_ERROR);
			}
			setPowerModel(pModel);
			return(TCL_OK);
		}
	}
	return (ResourceProvider::command(argc, argv));
}

void DcHost::print(){
	std::cout << "DcHost:\t";
	std::cout << id_;
	std::cout << "\n";
	std::cout << "Resources:\n";
	std::vector <std::vector<DcResource*> >::iterator iter_out;
	for(iter_out = resource_list.begin(); iter_out!=resource_list.end() ;iter_out++){
		std::vector <DcResource*>::iterator iter;
		for (iter = iter_out->begin(); iter!=iter_out->end(); iter++)
		{
			(*iter)->print();
		}
	}
	std::cout << "\n";

}

void DcHost::printTasklist(){
	std::vector<CloudTask *>::iterator iter;
	std::cout <<"Host " <<this->id_  << "\n";
	ResourceProvider::printTasklist();
}

void DcHost::setCurrentConsumption()
{
	double * predictors = new double[LastResType+1];
	bool idle = true;
	for(int i = Computing; i <= LastResType; i++){
		predictors[i]=updateResTypeUtil(static_cast<res_type>(i));
		if(predictors[i]!=0){
			idle=false;
		}
	}
	if((eDNS_enabled_) && (idle)){
		eCurrentConsumption_ = 0;
	} else {
		eCurrentConsumption_ = powerModel->estimate(4,predictors);
	}
	delete[] predictors;
}

void DcHost::eUpdate()
{
	/* Get time spent since last update */
	double etime = (Scheduler::instance().clock() - eLastUpdateTime_)/3600;	/* time in hours */

	eConsumed_ += etime * eCurrentConsumption_;
	eLastUpdateTime_ = Scheduler::instance().clock();

}

void DcHost::addResource(DcResource* res){
	ResourceProvider::addResource(res);
	if(res->specification->getPowerModel()!=NULL){
		powerModel->addComponent(res);
	}
}


void DcHost::updateEnergyAndConsumption(){
	setCurrentConsumption();
	eUpdate();
}
