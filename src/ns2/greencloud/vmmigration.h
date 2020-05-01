/*
 * vmmigration.h
 *
 *  @date Nov 13, 2013
 *  @author Guzek:Mateusz
 */

#ifndef VMMIGRATION_H_
#define VMMIGRATION_H_

#include <tclcl.h>

#include "tcp.h"
#include "vmmigrationsink.h"
#include "vm.h"

class VmMigration : public ResourceConsumer, public TclObject{
public:
	VmMigration();
	void initalizeMigration(VM* vm,ResourceProvider* target);
	virtual ~VmMigration();
	virtual int command(int argc, const char*const* argv);
	void finalizeMigration();
	void startMigration();

private:
	VM* migrated_vm_;
	ResourceProvider* target_;
	VmMigrationSink* vm_migration_sink_;
	TcpAgent* vm_migration_sender_;
	int id_;
};

#endif /* VMMIGRATION_H_ */
