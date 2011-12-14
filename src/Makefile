#
# OMNeT++/OMNEST Makefile for DRGSimLib
#
# This file was generated with the command:
#  opp_makemake -f --deep
#

# Name of target to be created (-o option)
TARGET = DRGSimLib$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I. \
    -IGlobal \
    -IGlobal/NICs \
    -IGlobal/NodeFactory \
    -IGlobal/Topology \
    -IGlobal/Topology/Generators \
    -IGraph \
    -INodes \
    -INodes/Sensors \
    -IProtocols \
    -IProtocols/GAP \
    -IProtocols/Ping \
    -ISupport

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = /src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/Global/NICs/TopologyControlNIC.o \
    $O/Global/NodeFactory/NodeFactory.o \
    $O/Global/Topology/topologyControl.o \
    $O/Global/Topology/Generators/BarabasiAlbertGenerator.o \
    $O/Global/Topology/Generators/BasicGenerator.o \
    $O/Global/Topology/Generators/FullyConnectedGenerator.o \
    $O/Global/Topology/Generators/gnpRandomGenerator.o \
    $O/Global/Topology/Generators/randomNeighborGenerator.o \
    $O/Global/Topology/Generators/TreeGenerator.o \
    $O/Graph/Graph.o \
    $O/Nodes/Sensors/sensor.o \
    $O/Protocols/BaseProtocol.o \
    $O/Protocols/GAP/gap.o \
    $O/Protocols/Ping/ping.o \
    $O/Support/GlobalObserver.o \
    $O/Global/NodeFactory/factory_m.o \
    $O/Global/Topology/topology_m.o \
    $O/Nodes/Sensors/sensor_m.o \
    $O/Protocols/events_m.o \
    $O/Protocols/GAP/gap_m.o \
    $O/Protocols/Ping/ping_m.o

# Message files
MSGFILES = \
    Global/NodeFactory/factory.msg \
    Global/Topology/topology.msg \
    Nodes/Sensors/sensor.msg \
    Protocols/events.msg \
    Protocols/GAP/gap.msg \
    Protocols/Ping/ping.msg

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $(TARGET)

$(TARGET) : $O/$(TARGET)
	$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY:

.SUFFIXES: .cc

$O/%.o: %.cc
	@$(MKPATH) $(dir $@)
	$(CXX) -c $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	-rm -rf $O
	-rm -f DRGSimLib DRGSimLib.exe libDRGSimLib.so libDRGSimLib.a libDRGSimLib.dll libDRGSimLib.dylib
	-rm -f ./*_m.cc ./*_m.h
	-rm -f Global/*_m.cc Global/*_m.h
	-rm -f Global/NICs/*_m.cc Global/NICs/*_m.h
	-rm -f Global/NodeFactory/*_m.cc Global/NodeFactory/*_m.h
	-rm -f Global/Topology/*_m.cc Global/Topology/*_m.h
	-rm -f Global/Topology/Generators/*_m.cc Global/Topology/Generators/*_m.h
	-rm -f Graph/*_m.cc Graph/*_m.h
	-rm -f Nodes/*_m.cc Nodes/*_m.h
	-rm -f Nodes/Sensors/*_m.cc Nodes/Sensors/*_m.h
	-rm -f Protocols/*_m.cc Protocols/*_m.h
	-rm -f Protocols/GAP/*_m.cc Protocols/GAP/*_m.h
	-rm -f Protocols/Ping/*_m.cc Protocols/Ping/*_m.h
	-rm -f Support/*_m.cc Support/*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc Global/*.cc Global/NICs/*.cc Global/NodeFactory/*.cc Global/Topology/*.cc Global/Topology/Generators/*.cc Graph/*.cc Nodes/*.cc Nodes/Sensors/*.cc Protocols/*.cc Protocols/GAP/*.cc Protocols/Ping/*.cc Support/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/Global/NICs/TopologyControlNIC.o: Global/NICs/TopologyControlNIC.cc \
  Global/Topology/topologyControl.h \
  Graph/Graph.h \
  Global/NICs/TopologyControlNIC.h \
  Global/Topology/Generators/BasicGenerator.h \
  Global/Topology/topology_m.h
$O/Global/NodeFactory/NodeFactory.o: Global/NodeFactory/NodeFactory.cc \
  Global/NodeFactory/NodeFactory.h \
  Global/NodeFactory/factory_m.h
$O/Global/NodeFactory/factory_m.o: Global/NodeFactory/factory_m.cc \
  Global/NodeFactory/factory_m.h
$O/Global/Topology/topologyControl.o: Global/Topology/topologyControl.cc \
  Global/Topology/topologyControl.h \
  Graph/Graph.h \
  Global/NICs/TopologyControlNIC.h \
  Global/Topology/Generators/BasicGenerator.h \
  Global/Topology/topology_m.h
$O/Global/Topology/topology_m.o: Global/Topology/topology_m.cc \
  Global/Topology/topology_m.h
$O/Global/Topology/Generators/BarabasiAlbertGenerator.o: Global/Topology/Generators/BarabasiAlbertGenerator.cc \
  Graph/Graph.h \
  Global/Topology/Generators/BarabasiAlbertGenerator.h \
  Global/Topology/Generators/BasicGenerator.h
$O/Global/Topology/Generators/BasicGenerator.o: Global/Topology/Generators/BasicGenerator.cc \
  Graph/Graph.h \
  Global/Topology/Generators/BasicGenerator.h
$O/Global/Topology/Generators/FullyConnectedGenerator.o: Global/Topology/Generators/FullyConnectedGenerator.cc \
  Graph/Graph.h \
  Global/Topology/Generators/FullyConnectedGenerator.h \
  Global/Topology/Generators/BasicGenerator.h
$O/Global/Topology/Generators/TreeGenerator.o: Global/Topology/Generators/TreeGenerator.cc \
  Graph/Graph.h \
  Global/Topology/Generators/BasicGenerator.h \
  Global/Topology/Generators/TreeGenerator.h
$O/Global/Topology/Generators/gnpRandomGenerator.o: Global/Topology/Generators/gnpRandomGenerator.cc \
  Graph/Graph.h \
  Global/Topology/Generators/BasicGenerator.h \
  Global/Topology/Generators/gnpRandomGenerator.h
$O/Global/Topology/Generators/randomNeighborGenerator.o: Global/Topology/Generators/randomNeighborGenerator.cc \
  Graph/Graph.h \
  Global/Topology/Generators/BasicGenerator.h \
  Global/Topology/Generators/randomNeighborGenerator.h
$O/Graph/Graph.o: Graph/Graph.cc \
  Graph/Graph.h
$O/Nodes/Sensors/sensor.o: Nodes/Sensors/sensor.cc \
  Nodes/Sensors/sensor_m.h \
  Nodes/Sensors/sensor.h \
  Support/GlobalObserver.h
$O/Nodes/Sensors/sensor_m.o: Nodes/Sensors/sensor_m.cc \
  Nodes/Sensors/sensor_m.h
$O/Protocols/BaseProtocol.o: Protocols/BaseProtocol.cc \
  Global/Topology/topologyControl.h \
  Global/NICs/TopologyControlNIC.h \
  Graph/Graph.h \
  Global/Topology/Generators/BasicGenerator.h \
  Protocols/BaseProtocol.h \
  Global/Topology/topology_m.h
$O/Protocols/events_m.o: Protocols/events_m.cc \
  Protocols/events_m.h
$O/Protocols/GAP/gap.o: Protocols/GAP/gap.cc \
  Nodes/Sensors/sensor_m.h \
  Graph/Graph.h \
  Global/Topology/Generators/BasicGenerator.h \
  Global/Topology/topologyControl.h \
  Global/NICs/TopologyControlNIC.h \
  Protocols/GAP/gap.h \
  Protocols/GAP/gap_m.h \
  Protocols/BaseProtocol.h \
  Protocols/events_m.h \
  Global/Topology/topology_m.h
$O/Protocols/GAP/gap_m.o: Protocols/GAP/gap_m.cc \
  Protocols/GAP/gap_m.h
$O/Protocols/Ping/ping.o: Protocols/Ping/ping.cc \
  Graph/Graph.h \
  Global/Topology/Generators/BasicGenerator.h \
  Global/Topology/topologyControl.h \
  Global/NICs/TopologyControlNIC.h \
  Protocols/Ping/ping_m.h \
  Protocols/BaseProtocol.h \
  Global/Topology/topology_m.h \
  Protocols/Ping/ping.h
$O/Protocols/Ping/ping_m.o: Protocols/Ping/ping_m.cc \
  Protocols/Ping/ping_m.h
$O/Support/GlobalObserver.o: Support/GlobalObserver.cc \
  Nodes/Sensors/sensor_m.h \
  Support/GlobalObserver.h
