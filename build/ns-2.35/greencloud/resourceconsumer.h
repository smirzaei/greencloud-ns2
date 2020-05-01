/*
 * resourceconsumer.h
 *
 *  @date Mar 11, 2013
 *  @author Guzek:Mateusz
 */

#ifndef RESOURCECONSUMER_H_
#define RESOURCECONSUMER_H_

#include "resdemand.h"
#include "resource.h"

#include <stdlib.h>
#include <vector>
#include <string.h>
#include "dcresource.h"


class ResourceConsumer {

public:
	ResourceConsumer();
	ResourceConsumer(unsigned int size, std::vector<Resource *> dem, bool isTask, bool isVM);


	bool isTask;
	bool isVM;
	virtual ~ResourceConsumer();

	double size_;	/**< amount of bytes transferred to servers for task execution */
	unsigned int getSize();
	void setSize(unsigned int size);
	double currProcRate_;	/**< current processing rate of the task (determined by the server) */


	std::vector<ResDemand *> res_demands; /**< Initial demand for resources */

	void setCurrentPerformance(std::vector<double> newPerf);

	void addUsedCapacity(double * cap);

};

#endif /* RESOURCECONSUMER_H_ */
