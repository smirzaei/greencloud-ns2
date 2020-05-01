/*
 * PowerModel.h
 *
 *  @date Sep 12, 2013
 *  @author Guzek:Mateusz
 */

#ifndef POWERMODEL_H_
#define POWERMODEL_H_
class DcResource;

#include <stdlib.h>
#include <iostream>
#include <tclcl.h>

/*An abstract class for power models.*/
class PowerModel : public TclObject {
public:
	PowerModel();
	virtual ~PowerModel();
	virtual void print();
	virtual int command(int argc, const char*const* argv);
	virtual double estimate(int size, double* predictors)  = 0;
	virtual double getMaxPower() = 0;
	virtual void addComponent(DcResource* component)  = 0;
	void setName(const char* name);
	std::string name_;
};

#endif /* POWERMODEL_H_ */
