#ifndef ns_dchost_h
#define ns_dchost_h

#include "object.h"
#include "cloudtask.h"
#include "config.h"
#include "scheduler.h"

#include <stdlib.h>
#include <limits.h>
#include <tclcl.h>
#include <vector>
#include <cfloat>
#include <float.h>
#include <math.h> 
#include <iostream> 
#include <numeric>

#include "dcresource.h"
#include "resourceprovider.h"
#include "powermodel/linearpmodel.h"
#include "powermodel/powermodel.h"

class DcRack;


class DcHost : public TclObject, public ResourceProvider {
public:
	DcHost();
	virtual ~DcHost();

public:
	DcRack* rack_;
	PowerModel* powerModel;
	virtual void print();
	virtual void printTasklist();

	/* Energy model */
	double eConsumed_;			/**< total W of energy consumed */
	double eNominalrate_;			/**< nominal consumption rate at full load at max CPU frequency */
	double eCurrentConsumption_;		/**< current consumption rate */


	int eDNS_enabled_;	/**< ON when dynamic shutdown is enabled */

	virtual int command(int argc, const char*const* argv);

	virtual void updateEnergyAndConsumption();
protected:

	void setCurrentConsumption();
	void eUpdate();
	virtual void addResource(DcResource* res);
	void setPowerModel(PowerModel* pModel);


	double eLastUpdateTime_;


};

#endif
