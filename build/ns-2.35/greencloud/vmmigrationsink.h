/*
 * vmmigrationsink.h
 *
 *  @date Nov 13, 2013
 *  @author Guzek:Mateusz
 */

#ifndef VMMIGRATIONSINK_H_
#define VMMIGRATIONSINK_H_

#include "tcp-sink.h"
#include "bytecounter.h"

class VmMigration;

class VmMigrationSink: public TcpSink, public ByteCounter {
public:
	VmMigrationSink(Acker *);
	virtual ~VmMigrationSink();
	virtual void recv(Packet*, Handler*);
	int seq_expected_;
	void setVmMigration(VmMigration* vm_migration);
private:
	VmMigration* vm_migration_;
	bool migration_finished_;
};

#endif /* VMMIGRATIONSINK_H_ */
