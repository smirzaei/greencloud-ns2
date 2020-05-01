/*
 * nic.cc
 *
 *  Created on: Mar 5, 2014
 *      Author: greencloud
 */

#include "nic.h"
#include "resourceprovider.h"

static class NicClass : public TclClass {
public:
	NicClass() : TclClass("NIC") {}
	TclObject* create(int argc, const char*const*argv) {
		return (new NIC());
	}
} class_nic;

NIC::NIC() {


}

NIC::~NIC() {

}

void NIC::setRp(ResourceProvider* rp){
	rp_ = rp;
}

double NIC::getUtilization(){
	return rp_->getResTypeUtil(Networking);
}

