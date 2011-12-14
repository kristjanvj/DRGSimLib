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

#include "gnpRandomGenerator.h"

Define_Module(GnpRandomGenerator);

void GnpRandomGenerator::initialize()
{
	BasicGenerator::initialize();
	checkIfConnected = false;
}

void GnpRandomGenerator::addNode(int vertexId, cGraph *g, bool update)
{
	Enter_Method_Silent();

	_addGraphVertex(vertexId, g);

	if ( !update )
		return;

	if ( g->getVertexCount() <= 1 )
		return;

	// Pick one vertex at random with probability 1 to ensure connectivity
	int peerId = g->pickRandomVertex(vertexId);
	_addGraphEdge(vertexId,peerId,g,update);

	double p = par("p"); // p is a volatile parameter
	vertexMapType::iterator iter;
	int expected = ceil(p*g->getVertexCount());
	if ( expected == 0 && bernoulli(p) )
		expected=1; // Flip a coin to delete one additional. Helps for small graphs
	for ( int i=0; i<expected; ++i )
	{
		peerId = g->pickRandomVertex(vertexId);
		_addGraphEdge(vertexId,peerId,g,update);
	}
}

void GnpRandomGenerator::removeNode(int vertexId, cGraph *g)
{
	Enter_Method_Silent();
	_removeGraphVertex(vertexId,g);
}

bool GnpRandomGenerator:: update(double t, cGraph *g)
{
	Enter_Method_Silent();

	double delta = t - lastUpdateTime;
	lastUpdateTime = t;
	bool modified = false;

	// Get the per second probability of removing and adding a single link
	double linkChurnProbability = par("linkChurnProbability"); // per minute link churn probability
	if ( linkChurnProbability==0.0 )
		return modified;

	// The probability of removing any particular edge. Divide by 60 since the churn probability is per minute
	double premove = delta*linkChurnProbability/60.0;

	// The probability of adding a new edge per minute. This is per vertex.
	double padd = (delta*linkChurnProbability/60.0) * (g->getEdgeCount()/g->getVertexCount());

	ulong edgeid;
	int expected = ceil(premove*g->getEdgeCount());
	if ( expected == 0 && bernoulli(premove) )
			expected=1; // Flip a coin to delete one additional. Helps for small graphs
	for( int i=0; i<expected; ++i )
	{
		edgeid = g->pickRandomEdge();
		if ( edgeid<0 )
			continue;
		_removeGraphEdge(edgeid,g);
		modified=true;
	}

	ulong nodeId,peerId;
	expected = ceil(padd*g->getVertexCount());
	if ( expected == 0 && bernoulli(premove) )
			expected=1; // Flip a coin to add one additional. Helps for small graphs
	for( int i=0; i<expected; ++i )
	{
		nodeId = g->pickRandomVertex();
		if ( nodeId<0 )
			continue;
		peerId = g->pickRandomVertex(nodeId);
		if ( peerId<0 )
			continue;
		_addGraphEdge(nodeId,peerId,g);
		modified=true;
	}
	return modified;
}

void GnpRandomGenerator::constructInitialTopology(cGraph *g)
{
	Enter_Method_Silent();

	double p;
	int peerId=-1;
	int vertexId;
	vertexMapType vertices = g->vertices;
	vertexMapType::iterator outer;
	vertexMapType::iterator inner;
	int expected;
	for ( outer=vertices.begin(); outer!=vertices.end(); outer++ )
	{
		vertexId=(*outer).first;
		peerId=g->pickRandomVertex(vertexId);
		if ( peerId < 0 )
			continue;
		_addGraphEdge(vertexId,peerId,g,false);
		p = par("p"); // Draw a new random value
		expected = ceil(p*g->getVertexCount());;
		for( int i=0; i<expected; ++i )
		{
			peerId = g->pickRandomVertex(vertexId);
			if ( peerId < 0 )
				continue;
			_addGraphEdge(vertexId,peerId,g,false);
		}
	}
}
