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

#include "FullyConnectedGenerator.h"

Define_Module(FullyConnectedGenerator);

void FullyConnectedGenerator::addNode(int vertexId, cGraph *g, bool update)
{
	Enter_Method_Silent();
	_addGraphVertex(vertexId, g, update);
	vertexMapType::iterator iter;
	for( iter=g->vertices.begin(); iter!=g->vertices.end(); iter++ )
	{
		if ( (*iter).first == vertexId )
			continue;
		_addGraphEdge( vertexId, (*iter).first, g, update );
	}
}

void FullyConnectedGenerator::removeNode(int vertexId, cGraph *g)
{
	Enter_Method_Silent();
	//
	_removeGraphVertex(vertexId,g);
}

void FullyConnectedGenerator:: update(simtime_t t, cGraph *g)
{
	Enter_Method_Silent();
}

void FullyConnectedGenerator::constructInitialTopology(cGraph *g)
{
	Enter_Method_Silent();
}
