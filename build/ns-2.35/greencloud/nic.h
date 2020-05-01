/*
 * nic.h
 *
 *  Created on: Mar 5, 2014
 *      Author: greencloud
 */

#ifndef NIC_H_
#define NIC_H_

class ResourceProvider;
#include "dcresource.h"

class NIC  : public DcResource{
public:
	NIC();
	virtual ~NIC();
	void setRp(ResourceProvider* rp);
	virtual double getUtilization();
private:
	ResourceProvider* rp_;
};

#endif /* NIC_H_ */
