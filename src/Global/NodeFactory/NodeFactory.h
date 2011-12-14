// ****************************************************************************
//
// DRGSim -- Dynamic Random Graph Simulation Components
//
// ****************************************************************************
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License version 3
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not see <http://www.gnu.org/licenses/>.
//
// ***************************************************************************

#ifndef __NODE_FACTORY_INCLUDED__
#define __NODE_FACTORY_INCLUDED__

#include <omnetpp.h>
#include <string>
#include <map>
#include "factory_m.h"

#define GENERATE_NODE_EVENT			10
#define DESTROY_NODE_EVENT			12

using namespace std;

typedef unsigned int uint;
typedef map<uint,cModule*> NodeMapType;

/**
 *  @brief TopologyControlNIC module
 *
 *  @author Kristjan V. Jonsson (kristjanvj@gmail.com)
 *  @version 0.9
 *  @date december 2011
 *
 *  @todo document
 */
class NodeFactory : public cSimpleModule
{
private:
    string nodePrefix;           /** < @brief Default name prefix of generated nodes*/
    string nodeTypename;         /** < @brief Name of default node type*/
    cModuleType *nodeType;       /** < @brief ptr to default node creator*/
    NodeMapType nodeMap;
    cMessage *generateEvent;
    std::string logFileName;
    bool loggingEnabled;
    double minLifetime;
    int maxPopulation;
	unsigned long totalCreated;
	unsigned long totalDestroyed;

public:
    NodeFactory();
    virtual ~NodeFactory();

protected:
    /** @brief Overrides of virtual base class functions. */
    virtual void initialize(int stage);
    /** @brief Overrides of virtual base class functions. */
    virtual void finish();
    /** @brief Overrides of virtual base class functions. Handles create and destroy messages. */
    virtual void handleMessage(cMessage * msg);
    int numInitStages() const {return 2;}

    void handleDestroyEvent(cMessage *msg);
    void generateNode(bool initialize=true);

    cModule* createNode(bool initialize);
    void destroyNode(uint nodeId);

    void logMessage(std::string msg);
};

#endif /* __NODE_FACTORY_INCLUDED__ */
