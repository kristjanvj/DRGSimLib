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

#include "topologyControl.h"
#include "TopologyControlNIC.h"

Define_Module(TopologyControl);

TopologyControl::TopologyControl()
{
	graph = new cGraph();
	curState = none;
	rootnode=-1;
}

TopologyControl::~TopologyControl()
{
	cancelAndDelete(topologyUpdate);
	delete graph;
}

void TopologyControl::initialize(int stage)
{
	ev << getFullPath() << ": Initializing, stage=" << stage << endl;
    if ( stage==0 )
    {
    	curState = initializing;

    	ev << getFullPath() << ": Initializing stage 0\n";

    	cModule *module = this->getParentModule()->getSubmodule("generator");
    	if ( module==NULL )
    		error("Topology generator module not found");
    	topogen = (BasicGenerator *)module;

    	updateInterval = par("updateInterval");
    	saveSnapshot = par("saveSnapshot");
    	recordDuringInitialization = par("recordDuringInitialization");
    	snapshotFile = (const char *)par("snapshotFile");
    	saveSnapshot = (snapshotFile.size()!=0);
    	logFileName = (const char *)par("logFileName");
    	loggingEnabled = (logFileName.size()!=0);

        topologyUpdate = new cMessage("topologyUpdate");
        topologyUpdate->setKind(MSG_TOPOLOGY_UPDATE);
        dynamicUpdates = ( updateInterval > 0.0 );
        if ( dynamicUpdates )
        {
        	ev << "\tScheduling periodic updates -- interval = " << updateInterval << " seconds." << endl;
        	scheduleAt(simTime()+updateInterval,topologyUpdate);
        }
    }
    else if ( stage==1 )
    {
    	curState = running;
    	constructInitialTopology();
    }
}

void TopologyControl::finish()
{
	ev << getFullPath() << ": Finalizing @ " << simTime() << endl;
	curState=finalizing;
}

void TopologyControl::handleMessage(cMessage *msg)
{
	if ( msg->isSelfMessage() )
	{
		//
		// Topology update timeout
		//

		if ( msg->getKind()==MSG_TOPOLOGY_UPDATE )
		{
			updateTopology();
            if ( updateInterval > 0.0 )
            	scheduleAt( simTime()+updateInterval, topologyUpdate );
		}
	}
	else
	{
		// No other messages expected
		delete msg;
	}
}

void TopologyControl::registerNode(int nodeid)
{
	Enter_Method_Silent();
	#ifdef DEBUG
	static unsigned int vertexcount=0;
	ev << getFullPath() << ": Registering vertex #" << (vertexcount++) << ", id=" << nodeid << endl;
	#endif
	topogen->addNode(nodeid, graph, isRunning());
	if ( saveSnapshot && ( isRunning() || ( isInitializing() && recordDuringInitialization ) ) )
		saveTopologySnapshot();
	if ( isRunning() )
		notifyModified();

	// The following code is used to compute the size of the cnnected component on updates
	// which is useful to keep track of graph partitioning.
	// TODO: Make this optional, use switches in NED file.
	if (rootnode==-1)
		rootnode=nodeid; // Grab first one to determine the size of the connected component
	if ( isRunning() )
	{
		IntSet connectedComponent;
		graph->getConnectedComponent(rootnode,&connectedComponent);
		int sizeTotal = graph->vertices.size();
		int sizeConnected = connectedComponent.size();
		logMessage("ADD",nodeid,sizeTotal,sizeConnected);
	}
}

void TopologyControl::unregisterNode(int nodeid)
{
	Enter_Method_Silent();
	#ifdef DEBUG
	ev << getFullPath() << ": Unregistering vertex " << nodeid << endl;
	#endif
	if ( isRunning() )
		topogen->removeNode(nodeid,graph);
	if ( saveSnapshot && isRunning() )
		saveTopologySnapshot();
	if ( isRunning() )
		notifyModified();

	// The following code is used to compute the size of the cnnected component on updates
	// which is useful to keep track of graph partitioning.
	// TODO: Make this optional, use switches in NED file.
	if ( isRunning() )
	{
		IntSet connectedComponent;
		graph->getConnectedComponent(rootnode,&connectedComponent);
		int sizeTotal = graph->vertices.size();
		int sizeConnected = connectedComponent.size();
		logMessage("DELETE",nodeid,sizeTotal,sizeConnected);
	}
}

void TopologyControl::enumerateNeighbors(int caller, IntSet *set)
{
	Enter_Method_Silent();
	graph->getIncidentVertices(caller,set);
}

void TopologyControl::constructInitialTopology()
{
	ev << getFullPath() << ": Constructing initial topology" << endl;
	topogen->constructInitialTopology(graph);
	if ( saveSnapshot )
		saveTopologySnapshot();
	topogen->clearModifiedSet();

	// The following code is used to compute the size of the cnnected component on updates
	// which is useful to keep track of graph partitioning.
	// TODO: Make this optional, use switches in NED file.
	IntSet connectedComponent;
	graph->getConnectedComponent(rootnode,&connectedComponent);
	int sizeTotal = graph->vertices.size();
	int sizeConnected = connectedComponent.size();
	logMessage("INITIAL",0,sizeTotal,sizeConnected);
}

void TopologyControl::updateTopology()
{
	ev << getFullPath() << ": Updating topology" << endl;
	double curtime=simTime().dbl();
	if ( graph->getVertexCount() < 2 )
		return;
	bool modified = topogen->update(curtime,graph);
	if ( modified && saveSnapshot && isRunning() )
		saveTopologySnapshot();
	if ( modified && dynamicUpdates && isRunning() )
		notifyModified();

	IntSet connectedComponent;
	graph->getConnectedComponent(rootnode,&connectedComponent);
	int sizeTotal = graph->vertices.size();
	int sizeConnected = connectedComponent.size();
	logMessage("UPDATE",0,sizeTotal,sizeConnected);
}

void TopologyControl::notifyModified()
{
	// Notify the NICs of changed modules.
	// TODO: There may be a more sensible or efficient way of doing this!
	cModule* nicModule;
	int moduleid = topogen->getNextModified();
	mTopologyUpdateEvent *event = new mTopologyUpdateEvent();
	event->setKind(NIC_CONTROL_NEIGHBORHOOD_CHANGE);
	while( moduleid != -1 )
	{
		if ( graph->vertices.find(moduleid) != graph->vertices.end() )
		{
			nicModule = simulation.getModule(moduleid);
			if ( nicModule!=NULL )
				sendDirect(event->dup(),nicModule,"controlIn");
		}
		moduleid = topogen->getNextModified();
	}
	delete event;
}

void TopologyControl::saveTopologySnapshot()
{
	static unsigned long snapshotCounter=0;

	char szFilename[256];
	char mode[1];
	int pos = snapshotFile.find("#");
	if ( pos!=-1)
	{
		std::string snapshotFileFirst = snapshotFile.substr(0,pos);
		std::string snapshotFileExt = snapshotFile.substr(pos+1,snapshotFile.size()-pos);
		sprintf(szFilename,"%s%.4ld%s", snapshotFileFirst.c_str(),snapshotCounter,snapshotFileExt.c_str());
		mode[0]='w';
	}
	else
	{
		sprintf(szFilename,"%s",snapshotFile.c_str());
		mode[0]='a';
	}
	FILE *f = fopen(szFilename,mode);
	if ( f==NULL )
	{
		ev << getFullPath()
		   << ": ERROR: Cannot opne topology file '" << szFilename << "' for appending" << endl;
		return;
	}

	edgeMapType::iterator iedges;
	fprintf( f, "#\r\n#%ld\t%.6f\r\n", snapshotCounter++, simTime().dbl() );
	fprintf( f, "#edgeid\tvertexa\tvertexb\tweight\r\n#\r\n" );
	for( iedges=graph->edges.begin(); iedges!=graph->edges.end(); iedges++ )
	{
		fprintf( f, "%d\t%d\t%d\t%.6f\r\n",
				    (*iedges).first, (*iedges).second.a, (*iedges).second.b, (*iedges).second.weight );
	}

	fprintf(f,"\r\n");
	fclose(f);
}

void TopologyControl::logMessage(std::string event, int nodeid, int totalNodes, int connectedComponentSize)
{
	if ( !loggingEnabled )
		return;

	int orphans = totalNodes-connectedComponentSize;

	FILE *f = fopen(logFileName.c_str(),"a");
	if ( f==NULL )
	{
		ev << getFullPath()
		   << ": ERROR: Cannot open file '" << logFileName << "' for appending" << endl;
		return;
	}

	fprintf( f, "%.6f\t%s\t%d\t%d\t%d\t%d\r\n",
			 simTime().dbl(), event.c_str(), nodeid, totalNodes, connectedComponentSize, orphans );
	fclose(f);
}
