/*
 * vm.h
 *
 *  @date Jun 12, 2013
 *  @author Guzek:Mateusz
 */

#ifndef VM_H_
#define VM_H_

#include "object.h"
#include "config.h"
#include "scheduler.h"

#include "resourceprovider.h"
#include "resourceconsumer.h"

/*Update the last element if new position to enum is added*/
enum vm_state {
	Ready,
	Running,
	Suspended,
	Stopped,
	Dead,
	FirstVmState = Ready,
	LastVmState = Dead
};

class VM: public TclObject, public ResourceProvider, public ResourceConsumer {
public:
	VM();
	virtual ~VM();

	virtual void print();
	virtual void printTasklist();

	virtual int command(int argc, const char*const* argv);
	virtual  void updateMIPS();
	virtual void addResource(DcResource* res);

	vm_state getVmState();

	void setHost(ResourceProvider * newHost);


protected:
	virtual void updateEnergyAndConsumption();
	vm_state state;
};

#endif /* VM_H_ */
