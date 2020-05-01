/*
 * percomponentmodel.h
 *
 *  @date Feb 25, 2014
 *  @author Guzek:Mateusz
 *
 */

#ifndef PERCOMPONENTMODEL_H_
#define PERCOMPONENTMODEL_H_
class DcResource;

#include "powermodel.h"
#include <stdlib.h>
#include <vector>

class PerComponentModel : public PowerModel {
public:
	PerComponentModel();
	virtual ~PerComponentModel();
	virtual void print();
	virtual int command(int argc, const char*const* argv);
	virtual double estimate(int size, double* predictors);
	virtual double getMaxPower();
	virtual void addComponent(DcResource* component);
	std::vector<DcResource*> modeled_components_;
};

#endif /* PERCOMPONENTMODEL_H_ */
