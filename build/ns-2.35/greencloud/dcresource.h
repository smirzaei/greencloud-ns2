/*
 * dcresource.h
 *
 *  @date Feb 27, 2013
 *  @author Guzek:Mateusz
 */

#ifndef DCRESOURCE_H_
#define DCRESOURCE_H_

#include <stdlib.h>
#include <vector>
#include <string>

#include "resourcespec.h"
#include "powermodel/powermodel.h"


/*The Resource fields represent the free capacities of DcResource.*/
class DcResource : public TclObject, public Resource {
public:
	DcResource();
	virtual ~DcResource();

	virtual int command(int argc, const char*const* argv);

	virtual int setSpecification(ResourceSpec* resspec);

	virtual double getUtilization();
	virtual double getPower();
	virtual double getMaxPower();

	virtual void print();
	ResourceSpec* specification;
private:
	int used_power_state_; /**< Used power state */
	double total_cap;


};


#endif /* DCRESOURCE_H_ */
