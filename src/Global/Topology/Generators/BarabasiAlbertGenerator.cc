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

#include "BarabasiAlbertGenerator.h"

Define_Module(BarabasiAlbertGenerator);

void BarabasiAlbertGenerator::initialize()
{
	BasicGenerator::initialize();
	m = par("m");
	m0 = par("m0");
	curDegreeSum=0;
}

void BarabasiAlbertGenerator::addNode(int vertexId, cGraph *g, bool update)
{
	Enter_Method_Silent();

	_addGraphVertex(vertexId, g);
	_connectVertex(vertexId,g);
}

void BarabasiAlbertGenerator::removeNode(int vertexId, cGraph *g)
{
	Enter_Method_Silent();
	//
	_removeVertexFromStatistic(vertexId,g);
	_removeGraphVertex(vertexId,g);
}

bool BarabasiAlbertGenerator:: update(double t, cGraph *g)
{
	Enter_Method_Silent();

	bool modified=false;

	double delta = t - lastUpdateTime;
	lastUpdateTime = t;

	// Get the per second probability of removing and adding a single link
	double linkChurnProbability = par("linkChurnProbability"); // per minute link churn probability

	// The probability of removing any particular edge. Divide by 60 since the churn probability is
	// per minute
	double premove = delta*linkChurnProbability/60.0;

	// The probability of adding a new edge. This is per vertex.
	double padd = (delta*linkChurnProbability/60.0) * (g->getEdgeCount()/g->getVertexCount());

	ulong edgeid;
	int expected = ceil(premove*g->getEdgeCount());
	if ( expected == 0 && bernoulli(premove) )
			expected=1; // Flip a coin to delete one additional. Helps for small graphs
	for( int i=0; i<expected; ++i )
	{
		edgeid = g->pickRandomEdge();
		if ( edgeid < 0 )
			continue;
		_removeEdgeFromStatistic(edgeid,g);
		_removeGraphEdge(edgeid,g);
		modified=true;
	}

	ulong nodeId;
	expected = ceil(padd*g->getVertexCount());
	if ( expected == 0 && bernoulli(premove) )
			expected=1; // Flip a coin to add one additional. Helps for small graphs
	for( int i=0; i<expected; ++i )
	{
		// Pick a vertex uniformly at random
		nodeId = g->pickRandomVertex();
		if ( nodeId<0 )
			continue;
		_connectVertex(nodeId,g);
		modified=true;
	}

	return modified;
}

void BarabasiAlbertGenerator::constructInitialTopology(cGraph *g)
{
	Enter_Method_Silent();
}

void BarabasiAlbertGenerator::_connectVertex(int vertexId, cGraph *g)
{
	if ( g->getVertexCount() <= 1 )
		return;

	int peerId;
	vertexMapType::iterator vertexiter;
	// Wire up initial m0 edges in a fully connected graph
	if ( g->vertices.size() <= m0 )
	{
		for( vertexiter=g->vertices.begin(); vertexiter!=g->vertices.end(); vertexiter++ )
		{
			peerId = (*vertexiter).first;
			if ( peerId==vertexId )
				continue;
			_addGraphEdge(vertexId,peerId,g);
			_addToStatistic(vertexId,peerId);
		}
	}
	else
	{
		for ( unsigned int i=0; i<m; ++i )
		{
			peerId = _pickRandomVertex(g,vertexId);
			if ( peerId==-1 )
			{
				continue;
			}
			_addGraphEdge(vertexId,peerId,g);
			_addToStatistic(vertexId,peerId);
		}
	}
}

int BarabasiAlbertGenerator::_pickRandomVertex( cGraph *g, int excludeId )
{
	double p = uniform(0.0,1.0);
	double cumsum=0;
	int nodeId=-1;

	NodeDegrees::iterator node;
	for(node=nodeDegrees.begin();node!=nodeDegrees.end();node++)
	{
		cumsum+=(double)(*node).second;
		if ( p <= (cumsum/curDegreeSum) )
		{
			nodeId=(*node).first;
			if ( excludeId==nodeId )
				continue;
			return nodeId;
		}
	}
	return (*nodeDegrees.begin()).first; // Return the first node just in case. TODO: This is an ugly hack.
}

unsigned long BarabasiAlbertGenerator::_sumDegrees( cGraph *g )
{
	unsigned long sum=0;
	vertexMapType::iterator vertex;
	for( vertex=g->vertices.begin(); vertex!=g->vertices.end(); vertex++ )
		sum+=g->getIncidentEdgeCount((*vertex).first);
	return sum;
}

void BarabasiAlbertGenerator::_addToStatistic(int a, int b)
{
	_incrementVertexStatistic(a);
	_incrementVertexStatistic(b);
}

void BarabasiAlbertGenerator::_removeVertexFromStatistic(int vertexId, cGraph *g)
{
	curDegreeSum -= g->getIncidentEdgeCount(vertexId);
	nodeDegrees.erase(vertexId);

	IntSet set;
	g->getIncidentVertices(vertexId,&set);
	IntSet::iterator v;
	for(v=set.begin(); v!=set.end(); v++)
		_decrementVertexStatistics((*v));
}

void BarabasiAlbertGenerator::_removeEdgeFromStatistic(int edgeId, cGraph *g)
{
	if ( g->edges.find(edgeId) == g->edges.end() )
		return;

	int a = g->edges[edgeId].a;
	int b = g->edges[edgeId].b;

	_decrementVertexStatistics(a);
	_decrementVertexStatistics(b);
}

void BarabasiAlbertGenerator::_incrementVertexStatistic(int vertexId)
{
	if ( nodeDegrees.find(vertexId)==nodeDegrees.end() )
		nodeDegrees[vertexId]=1;
	else
		nodeDegrees[vertexId]+=1;
	curDegreeSum++;
}

void BarabasiAlbertGenerator::_decrementVertexStatistics(int vertexId)
{
	if ( nodeDegrees.find(vertexId) != nodeDegrees.end() )
	{
		if ( nodeDegrees[vertexId]>0 )
			nodeDegrees[vertexId]-=1;
		else
			nodeDegrees.erase(vertexId);
		curDegreeSum--;
	}
}
