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

#include "BasicGenerator.h"

Define_Module(BasicGenerator);

void BasicGenerator::initialize()
{
	lastUpdateTime = 0.0;
	maintainConnectivity = par("maintainConnectivity");
}

void BasicGenerator::handleMessage(cMessage *msg)
{
    //
}

void BasicGenerator::addNode(int vertexId, cGraph *g, bool update)
{
	Enter_Method_Silent();
	error("The basic generator function should not be called directly. Override in a derived generator");
}

void BasicGenerator::removeNode(int vertexId, cGraph *g)
{
	Enter_Method_Silent();
	error("The basic generator function should not be called directly. Override in a derived generator");
}

bool BasicGenerator::update(double t, cGraph *g)
{
	Enter_Method_Silent();
	error("The basic generator function should not be called directly. Override in a derived generator");
	return false;
}

void BasicGenerator::constructInitialTopology(cGraph *g)
{
	Enter_Method_Silent();
	error("The basic generator function should not be called directly. Override in a derived generator");
}

int BasicGenerator::getNextModified()
{
	Enter_Method_Silent();
	if ( modifiedSet.empty() )
	{
		return -1;
	}
	else
	{
		int id = *modifiedSet.begin();
		modifiedSet.erase(modifiedSet.begin());
		return id;
	}
}

void BasicGenerator::clearModifiedSet()
{
	Enter_Method_Silent();
	modifiedSet.clear();
}

void BasicGenerator::_addGraphEdge(int ida, int idb, cGraph *g, bool notifyChanges)
{
	if ( g->addGraphEdge(ida,idb)==0 && notifyChanges )
	{
		_markModified(ida,g);
		_markModified(idb,g);
	}
}

void BasicGenerator::_removeGraphEdge(int edgeid, cGraph *g, bool notifyChanges)
{
	int a = g->edges[edgeid].a;
	int b = g->edges[edgeid].b;
	if ( g->removeGraphEdge(edgeid)==0 && notifyChanges )
	{
		_markModified(a,g);
		_markModified(b,g);

		// Determine whether the graph remains a single connected
		// component after the edge deletion. If not, or rather probably not,
		// then create an edge which maintains connectivity.
		if ( maintainConnectivity )
		{
			IntSet nodes;
			nodes.insert(a);
			nodes.insert(b);
			g->determineConnected(a,&nodes,3);
			if ( nodes.size()!=0 )
			{
				int aa = g->pickRandomNeighbor(a,2);
				int bb = g->pickRandomNeighbor(b,2);
				g->addGraphEdge(aa,bb);
			}
		}
	}
}

void BasicGenerator::_addGraphVertex(int id, cGraph *g, bool notifyChanges)
{
	g->addGraphVertex(id);
}

void BasicGenerator::_removeGraphVertex(int id, cGraph *g, bool notifyChanges)
{
	IntSet nodes;
	int adjacentid;
	int prevVertex=-1;

	if ( notifyChanges )
	{
		adjacencyListType::iterator adjacent = g->vertices[id].begin();
		for( ; adjacent!=g->vertices[id].end(); adjacent++ )
		{
			adjacentid = adjacent->first;
			_markModified(adjacentid,g);
			nodes.insert(adjacentid);
		}
	}
	g->removeGraphVertex(id);

	// The following code checks if the graph remains (probably) a single connected
	// component after the vertex deletion. If not, or rather probably not, we create
	// sufficiently many edges to maintain graph connectivity.
	// TODO: Review code and refactor.
	if ( maintainConnectivity )
	{
		IntSet connectedNodes;
		IntSet allNodes = nodes; // Copy
		g->determineConnected(adjacentid,&nodes,3);
		if ( nodes.size()==0 )
			return;

		std::set_difference( allNodes.begin(), allNodes.end(), nodes.begin(), nodes.end(),
							 std::inserter(connectedNodes, connectedNodes.end()) );

		IntSet::iterator iter;
		int peeridx,nodeid,peerid;
		IntSet::iterator peer;
		for(iter=nodes.begin(); iter!=nodes.end(); iter++)
		{
			if ( connectedNodes.size()>1 )
				peeridx = uniform(0,connectedNodes.size());
			else
				peeridx = 0;
			peer=connectedNodes.begin();
			std::advance(peer,peeridx);
			nodeid=(*iter);
			peerid=(*peer);
			g->addGraphEdge(nodeid, peerid);
		}
	}
}

void BasicGenerator::_markModified(int vertexId, cGraph *g)
{
	if ( modifiedSet.find(vertexId) != modifiedSet.end() )
		return;
	modifiedSet.insert(vertexId);
}

void BasicGenerator::_dumpNodeList(IntSet set)
{
	int curNodeId;
	IntSet::iterator iter;
	for( iter=set.begin(); iter!=set.end(); iter++ )
	{
		curNodeId = (*iter);
		ev << "Node: " << curNodeId << endl;
	}
}
