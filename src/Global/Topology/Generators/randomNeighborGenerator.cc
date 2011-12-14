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

#include "randomNeighborGenerator.h"

Define_Module(RandomNeighborGenerator);

void RandomNeighborGenerator::initialize()
{
	BasicGenerator::initialize();
	alpha = par("alpha");
	beta  = par("beta");
	srand(time(NULL));
}

void RandomNeighborGenerator::addNode(int vertexId, cGraph *g, bool update)
{
	Enter_Method_Silent();

	_addGraphVertex(vertexId, g);

	if ( g->vertices.size() <= 1 )
		return;

	int peerId =  g->pickRandomVertex();
	_addGraphEdge(vertexId,peerId,g);

	__linkToNeighborhood(vertexId, peerId, g);
}

void RandomNeighborGenerator::removeNode(int vertexId, cGraph *g)
{
	Enter_Method_Silent();
	_removeGraphVertex(vertexId,g);
}

bool RandomNeighborGenerator:: update(double t, cGraph *g)
{
	Enter_Method_Silent();

	double delta = t - lastUpdateTime;
	lastUpdateTime = t;
	bool modified;

	// Get the per second probability of removing and adding a single link
	double linkChurnProbability = par("linkChurnProbability"); // per minute link churn probability

	// The probability of removing any particular edge. Divide by 60 since the churn probability is
	// per minute
	double premove = delta*linkChurnProbability/60.0;

	// The probability of adding a new edge. This is per vertex.
	double padd = (delta*linkChurnProbability/60.0) * (g->getEdgeCount()/g->getVertexCount());

	// TODO: Optimize -- dont iterate through the entire set of nodes
	edgeMapType::iterator edgeiter;
	for(edgeiter=g->edges.begin();edgeiter!=g->edges.end();edgeiter++)
	{
		if ( bernoulli(premove) )
		{
			_removeGraphEdge((*edgeiter).first,g);
			modified=true;
		}
	}

	// TODO: Optimize -- dont iterate through the entire set of nodes
	vertexMapType::iterator vertexiter;
	int peerId,peerpeerId;
	for( vertexiter=g->vertices.begin(); vertexiter!=g->vertices.end(); vertexiter++ )
	{
		if ( bernoulli(padd) )
		{
			if ( g->getIncidentEdgeCount((*vertexiter).first) == 0 )
			{
				peerId = g->pickRandomVertex();
				if ( peerId != -1 )
				{
					_addGraphEdge((*vertexiter).first,peerId,g);
					modified=true;
				}
			}
			else
			{
				peerId = g->pickRandomNeighbor((*vertexiter).first);
				if ( peerId == -1 ) continue;
				peerpeerId = g->pickRandomNeighbor(peerId);
				if ( peerpeerId == -1 ) continue;
				_addGraphEdge((*vertexiter).first,peerpeerId,g);
				modified=true;
			}
		}
	}
	return modified;
}

void RandomNeighborGenerator::constructInitialTopology(cGraph *g)
{
	Enter_Method_Silent();
}

void RandomNeighborGenerator::__linkToNeighborhood(int vertexId, int peerId, cGraph *g)
{
	IntSet set;
	g->getIncidentVertices(peerId,&set);
	IntSet::iterator neighbor;
	int added=1;
	double p;
	for( neighbor=set.begin(); neighbor!=set.end(); neighbor++ )
	{
		p = alpha/pow(added,beta);
		if ( bernoulli(p) )
		{
			added++;
			_addGraphEdge(vertexId,(*neighbor),g);
		}
	}
}

/*
long RandomNeighborGenerator::__pickRandomVertex(vertexMapType *vertices)
{
	if ( vertices.size()==0 )
		return -1;
	if ( vertices.size()==1 )
		return (*vertices.begin()).first;
	//srand(time(NULL));
	int idx = rand() % ( vertices.size() - 1 );
	vertexMapType::iterator iter = vertices.begin();
	for(;idx>0;idx--,iter++);
	return (*iter).first;
}

long RandomNeighborGenerator::__pickRandomVertex(intvector &vertices)
{
	if ( vertices.size()==0 )
		return -1;
	if ( vertices.size()==1 )
		return vertices[0];
	int peerIdx = rand() % ( vertices.size() - 1 );
	return vertices[peerIdx];
}
*/

/*
ulong GnmRandomGenerator::pickRandomEdge(edgeMapType &edges)
{
	if ( edges.size()==0 )
		return 0;
	if ( edges.size()==1 )
		return (*edges.begin()).first;
	//srand(time(NULL));
	int idx = (rand() % edges.size()) - 1;
	edgeMapType::iterator iter = edges.begin();
	for(;idx>0;idx--,iter++);
	return (*iter).first;
}
*/
