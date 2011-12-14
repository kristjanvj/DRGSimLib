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

#ifndef __SCALESIM_BASICGENERATOR_H_
#define __SCALESIM_BASICGENERATOR_H_

#include <omnetpp.h>
#include <queue>
#include <set>
#include <algorithm>
#include "Graph.h"

typedef std::set<int> ModuleSet;

/**
 *  @brief BasicGenerator module.
 *
 *  The base generator module.
 *
 *  @author Kristjan V. Jonsson (kristjanvj@gmail.com)
 *  @version 0.9
 *  @date december 2011
 *
 *  @see Topology
 *  @see TopologyControl
 *
 *  @todo document
 */
class BasicGenerator : public cSimpleModule
{
protected:
	double lastUpdateTime;
	ModuleSet modifiedSet;
	bool maintainConnectivity;
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
public:
	virtual void addNode(int vertexId, cGraph *g, bool update=true);
	virtual void removeNode(int vertexId, cGraph *g);
	virtual bool update(double t, cGraph *g);
	virtual void constructInitialTopology(cGraph *g);
public:
	int  getNextModified();
	void clearModifiedSet();
protected:
	void _addGraphEdge(int ida, int idb, cGraph *g, bool notifyChanges=true);
	void _removeGraphEdge(int edgeid, cGraph *g, bool notifyChanges=true);
	void _addGraphVertex(int id, cGraph *g, bool notifyChanges=true);
	void _removeGraphVertex(int id, cGraph *g, bool notifyChanges=true);
	void _markModified(int vertexId, cGraph *g);
protected:
	void _dumpNodeList(IntSet set);
};

#endif
