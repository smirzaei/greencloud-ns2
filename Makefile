# NOTE: This is a modified version (kind of a hack) which allows the parent
# GreenCloud folder to behave as a normal NS2 source directory.
# It contains entries for the customized source files; if any are changed,
# it defers the 'make' step to the modified NS2 directory

NS2_BUILD = build/ns-*
NS2_SRC = src/ns2
NAM_BUILD = build/nam-*
NAM_SRC = src/nam
DASHBOARD_BUILD = build/dashboard
DASHBOARD_SRC = src/dashboard

# If you create or remove any source files then run the following command to refresh this list:
#   find src/ -type f |grep -P '\.(cc|c|h)$'|sed 's/$/ \\/g'
NS2_SOURCE_FILES = \
	$(NS2_SRC)/greencloud/paretoclouduser.cc \
	$(NS2_SRC)/greencloud/expclouduser.cc \
	$(NS2_SRC)/greencloud/clouduser.cc \
	$(NS2_SRC)/greencloud/clouduser.h \
	$(NS2_SRC)/greencloud/tskcomsink.h \
	$(NS2_SRC)/greencloud/switchenergymodel.h \
	$(NS2_SRC)/greencloud/tskagent.h \
	$(NS2_SRC)/greencloud/cbrclouduser.cc \
	$(NS2_SRC)/greencloud/datacenter.cc \
	$(NS2_SRC)/greencloud/cloudtask.cc \
	$(NS2_SRC)/greencloud/dchost.cc \
	$(NS2_SRC)/greencloud/switchenergymodel.cc \
	$(NS2_SRC)/greencloud/datacenter.h \
	$(NS2_SRC)/greencloud/clouduser.h \
	$(NS2_SRC)/greencloud/tskcomsink.cc \
	$(NS2_SRC)/greencloud/dchost.h \
	$(NS2_SRC)/greencloud/tskagent.cc \
	$(NS2_SRC)/greencloud/cloudtask.h \
	$(NS2_SRC)/greencloud/resourcespec.h \
	$(NS2_SRC)/greencloud/resourcespec.cc \
	$(NS2_SRC)/greencloud/dcresource.h \
	$(NS2_SRC)/greencloud/dcresource.cc \
	$(NS2_SRC)/greencloud/cpu.h \
	$(NS2_SRC)/greencloud/cpu.cc \
	$(NS2_SRC)/greencloud/nic.h \
	$(NS2_SRC)/greencloud/nic.cc \
	$(NS2_SRC)/greencloud/resource.h \
	$(NS2_SRC)/greencloud/resource.cc \
	$(NS2_SRC)/greencloud/resdemand.h \
	$(NS2_SRC)/greencloud/resdemand.cc \
	$(NS2_SRC)/greencloud/resourceconsumer.h \
	$(NS2_SRC)/greencloud/resourceconsumer.cc \
	$(NS2_SRC)/greencloud/resourceprovider.h \
	$(NS2_SRC)/greencloud/resourceprovider.cc \
	$(NS2_SRC)/greencloud/vm.h \
	$(NS2_SRC)/greencloud/vm.cc \
	$(NS2_SRC)/greencloud/corescheduler.h \
	$(NS2_SRC)/greencloud/corescheduler.cc \
	$(NS2_SRC)/greencloud/taskalloc.h \
	$(NS2_SRC)/greencloud/taskalloc.cc \
	$(NS2_SRC)/greencloud/provideroutagent.h \
	$(NS2_SRC)/greencloud/provideroutagent.cc \
	$(NS2_SRC)/greencloud/vmmigration.h \
	$(NS2_SRC)/greencloud/vmmigration.cc \
	$(NS2_SRC)/greencloud/vmmigrationsink.h \
	$(NS2_SRC)/greencloud/vmmigrationsink.cc \
	$(NS2_SRC)/greencloud/bytecounter.h \
	$(NS2_SRC)/greencloud/bytecounter.cc \
	$(NS2_SRC)/greencloud/dcscheduler/providerscore.h \
	$(NS2_SRC)/greencloud/dcscheduler/providerscore.cc \
	$(NS2_SRC)/greencloud/dcscheduler/dcscheduler.h \
	$(NS2_SRC)/greencloud/dcscheduler/dcscheduler.cc \
	$(NS2_SRC)/greencloud/dcscheduler/randdens.h \
	$(NS2_SRC)/greencloud/dcscheduler/randdens.cc \
	$(NS2_SRC)/greencloud/dcscheduler/bestdens.h \
	$(NS2_SRC)/greencloud/dcscheduler/bestdens.cc \
	$(NS2_SRC)/greencloud/dcscheduler/greenscheduler.h \
	$(NS2_SRC)/greencloud/dcscheduler/greenscheduler.cc \
	$(NS2_SRC)/greencloud/dcscheduler/roundrobinscheduler.h \
	$(NS2_SRC)/greencloud/dcscheduler/roundrobinscheduler.cc \
	$(NS2_SRC)/greencloud/dcscheduler/probabilisticscheduler.h \
	$(NS2_SRC)/greencloud/dcscheduler/probabilisticscheduler.cc \
	$(NS2_SRC)/greencloud/dcscheduler/scorescheduler.h \
	$(NS2_SRC)/greencloud/dcscheduler/scorescheduler.cc \
	$(NS2_SRC)/greencloud/dcscheduler/bestscorescheduler.h \
	$(NS2_SRC)/greencloud/dcscheduler/bestscorescheduler.cc \
	$(NS2_SRC)/greencloud/dcscheduler/randomscheduler.h \
	$(NS2_SRC)/greencloud/dcscheduler/randomscheduler.cc \
	$(NS2_SRC)/greencloud/dcscheduler/herosscheduler.h \
	$(NS2_SRC)/greencloud/dcscheduler/herosscheduler.cc \
	$(NS2_SRC)/greencloud/powermodel/powermodel.h \
	$(NS2_SRC)/greencloud/powermodel/powermodel.cc \
	$(NS2_SRC)/greencloud/powermodel/linearpmodel.h \
	$(NS2_SRC)/greencloud/powermodel/linearpmodel.cc \
	$(NS2_SRC)/greencloud/powermodel/percomponentmodel.h \
	$(NS2_SRC)/greencloud/powermodel/percomponentmodel.cc \
	$(NS2_SRC)/greencloud/tskoutsink.h \
	$(NS2_SRC)/greencloud/tskoutsink.cc \
	$(NS2_SRC)/greencloud/taskinfo.h \
	$(NS2_SRC)/greencloud/taskinfo.cc \
	

NAM_SOURCE_FILES=


# 'make all' updates the build directory with any dev changes, 
# and defers all other decisions to the underlying NS2 Makefile.
# no need to 'make' the graphite stuff - we can run the GreenCloud Graphite scripts directly
all: $(NS2_SOURCE_FILES) $(NAM_SOURCE_FILES)
	. build/environment.sh; \
	rsync -av $(DASHBOARD_SRC)/ $(DASHBOARD_BUILD)/ ; \
	rsync -av $(NS2_SRC)/ $(NS2_BUILD)/ ; \
	mkdir -p $(NAM_SRC); \
	rsync -av $(NAM_SRC)/ $(NAM_BUILD)/ ; \
	cd $(NS2_BUILD); \
	make; \
	cd -; \
	cd $(NAM_BUILD); \
	make; \
	cd -;

install: all
	cd $(NS2_BUILD); \
	make install; \
	cd -; \
	cd $(NAM_BUILD); \
	make install

clean:
	cd $(NS2_BUILD); \
	make clean; \
	cd -; \
	cd $(NAM_BUILD); \
	make clean

distclean:
	cd $(NS2_BUILD); \
	make distclean; \
	cd -; \
	cd $(NAM_BUILD); \
	make distclean
	
greencloudclean:
	cd $(NS2_BUILD)/greencloud; \
	rm -f *.o ; \
	rm -f powermodel/*.o \
	rm -f dcscheduler/*.o

