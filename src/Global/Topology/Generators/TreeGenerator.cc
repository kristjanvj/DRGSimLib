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

#include "TreeGenerator.h"

Define_Module(TreeGenerator);

void TreeGenerator::initialize()
{
	n = par("n");
}

void TreeGenerator::addNode(int vertexId, cGraph *g, bool update)
{
	Enter_Method_Silent();
	_addGraphVertex(vertexId, g);
	if ( update )
	{
		// TODO: Dynamic update
	}
}

void TreeGenerator::removeNode(int vertexId, cGraph *g)
{
	Enter_Method_Silent();
	_removeGraphVertex(vertexId,g);
}

bool TreeGenerator:: update(simtime_t t, cGraph *g)
{
	Enter_Method_Silent();
	return false;
}

void TreeGenerator::constructInitialTopology(cGraph *g)
{
	Enter_Method_Silent();
	//
	std::vector<int> nodes;
	vertexMapType::iterator node = g->vertices.begin();
	for ( ; node!=g->vertices.end(); node++ )
		nodes.push_back((*node).first);

	int parentidx, parentid, nodeid;
	for ( unsigned int nodeidx=1; nodeidx<nodes.size(); ++nodeidx )
	{
		parentidx = (nodeidx-1)/n;
		parentid = nodes[parentidx];
		nodeid = nodes[nodeidx];
		_addGraphEdge(parentid,nodeid,g,false);
	}
}

