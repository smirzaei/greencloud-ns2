/*
 * resourcespec.h
 *
 *  @date Feb 27, 2013
 *  @author Guzek:Mateusz
 */

#ifndef RESOURCESPEC_H_
#define RESOURCESPEC_H_

#include "resource.h"
#include "powermodel/powermodel.h"

#include <stdlib.h>
#include <vector>
#include <tclcl.h>

#include <iostream>
#include <string>




class ResourceSpec : public TclObject, public Resource {
public:
	friend class DcResource;
	ResourceSpec();
	virtual ~ResourceSpec();

	virtual int command(int argc, const char*const* argv);

public:
	/* Name of the component*/
	std::string name_;
	/*Power states, to be extended */
	std::vector <int> power_states;

	void print();

	int addCapacity(double cap);
	int addPowerState(int ps);
	int setName(const char* name);
	int setArch(const char* name);
	PowerModel* getPowerModel();

private:
	PowerModel * res_model_;
	void setPowerModel(PowerModel* model);
};

#endif /* RESOURCESPEC_H_ */
