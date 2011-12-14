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

typedef std::map<int,int> NodeDegrees;

/**
 *  @brief BarabasiAlbertGenerator module.
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
class BarabasiAlbertGenerator : public BasicGenerator
{
private:
	unsigned int m,m0;
	unsigned long curDegreeSum;
	NodeDegrees nodeDegrees;
protected:
	virtual void initialize();
public:
	virtual void addNode(int vertexId, cGraph *g, bool update=true);
	virtual void removeNode(int vertexId, cGraph *g);
	virtual bool update(double t, cGraph *g);
	virtual void constructInitialTopology(cGraph *g);
private:
	void _connectVertex(int vertexId, cGraph *g);
	int _pickRandomVertex( cGraph *g, int excludeId=-1 );
	unsigned long _sumDegrees( cGraph *g );
	void _addToStatistic(int a, int b);
	void _removeVertexFromStatistic(int vertexId, cGraph *g);
	void _removeEdgeFromStatistic(int edgeId, cGraph *g);
	void _incrementVertexStatistic(int vertexId);
	void _decrementVertexStatistics(int vertexId);
};

#endif
