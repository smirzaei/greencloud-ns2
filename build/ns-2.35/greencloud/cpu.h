/*
 * cpu.h
 *
 *  @date Sep 3, 2013
 *  @author Guzek:Mateusz
 */

#ifndef CPU_H_
#define CPU_H_

#include "corescheduler.h"
#include "dcresource.h"

class ResourceConsumer;
class ResourceProvider;

/*The CPU class that tracks the status of the */
class CPU : public DcResource{
public:
	CPU();
	virtual ~CPU();

	double getCurrentMIPS();
	double getNominalMIPS();
	/**
	 * Sets the resource specification of the resource.
	 */
	virtual int setSpecification(ResourceSpec* resspec);
	/**
	 * Sets the resource provider of the resource.
	 */
	void setProvider(ResourceProvider* provider);
	void setDVFS(int eDVFS_enabled_);
	virtual double getUtilization();
	virtual void print();
	virtual int command(int argc, const char*const* argv);
	std::vector<CoreScheduler *> cores_schedulers_; /**<  List of cores schedulers of the CPU*/
private:
	void getMIPS();

	double nominal_mips_;



};
#endif /* CPU_H_ */
