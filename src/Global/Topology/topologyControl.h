// ****************************************************************************
//
// DRGSim -- Dynamic Random Graph Simulation Components
//
// ****************************************************************************
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __TSENSESIM_TOPOLOGYCONTROL_H_
#define __TSENSESIM_TOPOLOGYCONTROL_H_

#include <omnetpp.h>
#include <string>
#include "Graph.h"
#include "BasicGenerator.h"
#include "topology_m.h"

#define MSG_TOPOLOGY_UPDATE      10001

typedef std::map<ulong,cModule*> ModuleMap;
typedef std::set<int> IntSet;

enum States {none,initializing,running,finalizing};

/**
 *  @brief TopologyControl module
 *
 *  @author Kristjan V. Jonsson (kristjanvj@gmail.com)
 *  @version 0.9
 *  @date december 2011
 *
 *  @todo document
 */
class TopologyControl : public cSimpleModule
{
  private:
	double updateInterval;
	cMessage *topologyUpdate;
	cGraph *graph;
	bool saveSnapshot;
	std::string snapshotFile;
	std::string logFileName;
	bool loggingEnabled;
	BasicGenerator *topogen;
	States curState;
	bool recordDuringInitialization;
	bool dynamicUpdates;
	int rootnode;
  public:
	TopologyControl();
	virtual ~TopologyControl();
  protected:
    virtual void initialize(int stage);
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    int numInitStages() const {return 2;}
  public:
    void registerNode(int nodeid /*, cModule *node*/ );
    void unregisterNode(int nodeid);
    void enumerateNeighbors(int caller, IntSet *set);
  protected:
    void constructInitialTopology();
    void updateTopology();
    void saveTopologySnapshot();
    void notifyModified();
  protected:
    bool isRunning() {return curState==running;}
    bool isInitializing() {return curState==initializing;}
  protected:
    void logMessage(std::string event, int nodeid, int totalNodes, int connectedComponentSize);
};

#endif /* __TSENSESIM_TOPOLOGYCONTROL_H_ */
