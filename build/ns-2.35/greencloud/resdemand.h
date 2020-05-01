/*
 * resdemand.h
 *
 *  @date Sep 5, 2013
 *  @author Guzek:Mateusz
 */

#ifndef RESDEMAND_H_
#define RESDEMAND_H_


#include <stdlib.h>
#include <vector>
#include "resource.h"

class DcResource; /**< Used to represent the status of execution. (Where, how fast, and how much left ) */

class ResDemand : public Resource {
public:

	std::vector <Capacity *> capacity_location; /**< Currently used capacities for "static" resources (other than CPU and Networking) */

	std::vector <double> current_performance; /**< Current allocation (i.e. processing rate) rate for each of the resources */

	/*Indexes of the used Computing resource*/
	ResDemand(Resource & res);
	ResDemand(Resource & res,DcResource* svr);
	DcResource* supported_virtual_resource ;
};

#endif /* RESDEMAND_H_ */
