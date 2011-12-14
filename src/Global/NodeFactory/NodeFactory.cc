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

#include "NodeFactory.h"
#include <fstream>
#include <sstream>
#include <vector>

//#define __NODE_FACTORY_DEBUG__

// The module class needs to be registered with OMNeT++
Define_Module(NodeFactory);

//
// Constructor
//
// Initialize event messages
//
NodeFactory::NodeFactory()
{
	generateEvent = NULL;
}

NodeFactory::~NodeFactory()
{
	cancelAndDelete(generateEvent);
}

//
// initialize
//
// Override of virtual base class method.
// Initialize simulation at startup.
//
void NodeFactory::initialize(int stage)
{
	if ( stage==0 )
	{
		totalCreated=0;
		totalDestroyed=0;

		nodePrefix = string((const char *) par("nodePrefix"));
		nodeTypename = string((const char *) par("nodeTypename"));
		minLifetime = par("minLifetime");
		maxPopulation = par("maxPopulation");

	    logFileName = (const char *)par("logFileName");
	    loggingEnabled = (logFileName.size()!=0);

		if ( nodeTypename == "")
			error("Unspecified node typename");
		nodeType = cModuleType::get(nodeTypename.c_str());
		if ( nodeType == NULL )
			error("Unknown node type");

		int initial = par("initialPopulation");
		for ( int i=0; i<initial; ++i )
			generateNode(false);
	}
	else if ( stage==1 )
	{
		double generateInterval = par("generateInterval");
		generateEvent = new cMessage();
		generateEvent->setKind(GENERATE_NODE_EVENT);
		if ( generateInterval > 0.0 )
			scheduleAt(simTime()+generateInterval, generateEvent);
	}
}

void NodeFactory::finish()
{
	recordScalar("totalCreated", totalCreated);
	recordScalar("totalDestroyed", totalDestroyed);
}

void NodeFactory::handleMessage(cMessage * msg)
{
	if ( msg->isSelfMessage() )
	{
		if ( msg->getKind() == GENERATE_NODE_EVENT )
		{
			generateNode();
			if ( maxPopulation>0 && (int)nodeMap.size() >= maxPopulation )
				return; // Temporarily suspend genertion
			double generateInterval = par("generateInterval");
			scheduleAt(simTime()+generateInterval, generateEvent);
		}
		else if ( msg->getKind() == DESTROY_NODE_EVENT )
		{
			handleDestroyEvent(msg);
			delete msg;
			if ( !generateEvent->isScheduled() && (int)nodeMap.size() < maxPopulation )
			{
				// Restart generation if we have less than the maximum number of active nodes
				double generateInterval = par("generateInterval");
				scheduleAt(simTime()+generateInterval, generateEvent);
			}
		}
	}
	else
	{
		delete msg;
	}
}

void NodeFactory::handleDestroyEvent(cMessage *msg)
{
	mFactoryDestroyNode *destroyEvent = check_and_cast<mFactoryDestroyNode*>(msg);
	uint nodeid = destroyEvent->getNodeId();
	ev << getFullPath() << ": Destroying node " << nodeid << " @ " << simTime().dbl() << endl;
	destroyNode(nodeid);
}

void NodeFactory::generateNode(bool initialize)
{
	cModule *node = createNode(initialize);
	uint nodeid = (uint)node->getId();
	totalCreated++;

	logMessage("CREATE");

	nodeMap[nodeid] = node;

	double lifetime = par("lifetime");
	if ( lifetime == 0 )
		return;

	if ( !initialize )
	{
		// This means that the node is generated at time zero.
		// Hence, we want to uniformly distribute the destroy events for the initial node population
		// rather than having them bunch up at the expected value of the lifetime distribution.
		lifetime = uniform(0,lifetime);
	}
	else
	{
		if ( lifetime < minLifetime )
			lifetime=minLifetime;
	}

	mFactoryDestroyNode *destroyEvent = new mFactoryDestroyNode();
	destroyEvent->setKind(DESTROY_NODE_EVENT);
	destroyEvent->setNodeId(nodeid);
	scheduleAt(simTime()+lifetime, destroyEvent);
}

cModule* NodeFactory::createNode(bool initialize)
{
	static uint nodecounter=0;

	// Create the module name
	char szModuleName[32];
  	sprintf( szModuleName, "%s%.4d", nodePrefix.c_str(), nodecounter++ );

	cModule *module = nodeType->create( szModuleName, this->getParentModule() );
  	module->finalizeParameters();

	char szDisplayString[100];
	sprintf( szDisplayString, "i=%s;i2=%s", "device/palm2", "status/green" );
	module->setDisplayString( szDisplayString );

	module->buildInside();

	// create activation message
	module->scheduleStart( simTime() );
	if (initialize)
		module->callInitialize();
	return module;
}

void NodeFactory::destroyNode(uint nodeId)
{
	if ( nodeMap.find(nodeId) == nodeMap.end() )
		return;

	cModule *node = nodeMap[nodeId];
	node->callFinish();
	node->deleteModule();
	nodeMap.erase(nodeId);

	totalDestroyed++;
	logMessage("DESTROY");
}

void NodeFactory::logMessage(std::string msg)
{
	if ( !loggingEnabled )
		return;

	FILE *f = fopen(logFileName.c_str(),"a");
	if ( f==NULL )
	{
		ev << getFullPath()
		   << ": ERROR: Cannot open file '" << logFileName << "' for appending" << endl;
		return;
	}

	fprintf( f, "%.6f\t%s\t%ld\t%ld\t%ld\n", simTime().dbl(), msg.c_str(),
			 totalCreated, totalDestroyed, totalCreated-totalDestroyed );
	fclose(f);
}
