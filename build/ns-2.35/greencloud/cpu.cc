/*
 * cpu.cc
 *
 *  @date Sep 3, 2013
 *  @author Guzek:Mateusz
 */

#include "cpu.h"

static class CpuClass : public TclClass {
public:
	CpuClass() : TclClass("CPU") {}
	TclObject* create(int argc, const char*const*argv) {
		return (new CPU());
	}
} class_cpu;

CPU::CPU(){

}

CPU::~CPU(){

}

void CPU::print(){
	std::cerr << "CPU: ";
	DcResource::print();
}

void CPU::getMIPS(){
	double result = 0;
	std::vector <Capacity>::iterator iter;
	for (iter = this->specification->capacity.begin(); iter!=this->specification->capacity.end(); iter++)
	{
		result += (*iter);
	}
	char out[100];
	sprintf(out,"set tmp_cpu_mips %.0f",result);
	Tcl& tcl = Tcl::instance();
	tcl.eval(out);

}

int CPU::command(int argc, const char*const* argv){

	if (argc == 2) {
		if (strcmp(argv[1], "print") == 0) {
			this->print();
			return (TCL_OK);
		} else if (strcmp(argv[1], "getMIPS") == 0) {
			this->getMIPS();
			return (TCL_OK);
		}
	}
	return (CPU::command(argc, argv));
}

/*Set specification, initialize usage as vector of 0.*/

int CPU::setSpecification(ResourceSpec* resspec){
	if(resspec==NULL){
		std::cerr << "ERROR: Null pointer passed as ResourceSpec.";
		return 1;
	}
	this->DcResource::setSpecification(resspec);
	std::vector<Capacity>::iterator iter_nominal;
	std::vector<Capacity>::iterator iter_reserved;
	for(iter_nominal= resspec->capacity.begin(),iter_reserved=this->capacity.begin();
			iter_nominal != resspec->capacity.end();
			iter_nominal++,iter_reserved++){
		cores_schedulers_.push_back(new CoreScheduler(&(*iter_nominal),&(*iter_reserved)));
	}
	std::vector<CoreScheduler*>::iterator iter;
	nominal_mips_ = 0;
	for(iter= cores_schedulers_.begin();
			iter != cores_schedulers_.end();
			iter++){
		nominal_mips_ += (*iter)->getNominalMIPS();
	}

	return 0;
}

void CPU::setProvider(ResourceProvider* provider){
	if(cores_schedulers_.empty()){
		std::cerr << "No core schedulers!\n";
	}
	std::vector<CoreScheduler*>::iterator iter;
	for(iter= cores_schedulers_.begin();
			iter != cores_schedulers_.end();
			iter++){
		(*iter)->setProvider(provider);
	}

}

void CPU::setDVFS(int eDVFS_enabled_){
	if(cores_schedulers_.empty()){
		std::cerr << "No core schedulers!\n";
	}
	std::vector<CoreScheduler*>::iterator iter;
	for(iter= cores_schedulers_.begin();
			iter != cores_schedulers_.end();
			iter++){
		(*iter)->setDVFS(eDVFS_enabled_);
	}
}

double CPU::getCurrentMIPS(){
	std::vector<CoreScheduler*>::iterator iter;
	double result = 0;
	for(iter= cores_schedulers_.begin();
			iter != cores_schedulers_.end();
			iter++){

		result += (*iter)->getCurrentMIPSRecursive();
	}
	return result;
}
double CPU::getNominalMIPS(){
	return nominal_mips_;
}

double CPU::getUtilization(){
	return getCurrentMIPS()/getNominalMIPS();
}
