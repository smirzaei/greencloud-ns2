/*
 * vmmigrationsink.cc
 *
 *  @date Nov 13, 2013
 *  @author Guzek:Mateusz
 */

#include "vmmigrationsink.h"
#include "vmmigration.h"

static class VmMigrationSinkClass : public TclClass {
public:
	VmMigrationSinkClass() : TclClass("Agent/TCPSink/VmMigrationSink") {}
	TclObject* create(int, const char*const*) {
		return (new VmMigrationSink(new Acker()));
	}
} class_vm_migrationsink;

VmMigrationSink::VmMigrationSink(Acker* a) : TcpSink(a), seq_expected_(-1),migration_finished_(false){

}

VmMigrationSink::~VmMigrationSink() {

}

void VmMigrationSink::setVmMigration(VmMigration* vm_migration){
	vm_migration_ = vm_migration;
}

void VmMigrationSink::recv(Packet* pkt, Handler*)
{
	bytes_since_ += hdr_cmn::access(pkt)->size();

	if(seq_expected_ == hdr_tcp::access(pkt)->seqno()){
		//		Migration complete.
		migration_finished_ = true;
		vm_migration_->finalizeMigration();
	}
	else if(migration_finished_){
		std::cerr << "ERROR! Something went wrong. Packets received after migration is finished!\n";
	}
	TcpSink::recv(pkt,this);
}
