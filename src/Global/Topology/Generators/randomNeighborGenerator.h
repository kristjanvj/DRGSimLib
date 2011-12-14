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

#ifndef __SCALESIM_GNP_RANDOM_GRAPH_H_
#define __SCALESIM_GNP_RANDOM_GRAPH_H_

#include <omnetpp.h>
#include "BasicGenerator.h"

/**
 *  @brief RandomNeighborGenerator module.
 *
 *  Generator module used with the Topology compound module.
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
class RandomNeighborGenerator : public BasicGenerator
{
private:
	double alpha;
	double beta;
protected:
	virtual void initialize();
public:
	virtual void addNode(int vertexId, cGraph *g, bool update=true);
	virtual void removeNode(int vertexId, cGraph *g);
	virtual bool update(double t, cGraph *g);
	virtual void constructInitialTopology(cGraph *g);
private:
	void __linkToNeighborhood(int vertexId, int peerId, cGraph *g);
//	int  __pickRandomVertex(vertexMapType &vertices);
//	int  __pickRandomVertex(intvector &vertices);
};

#endif
