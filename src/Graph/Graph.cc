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

#include "Graph.h"

cGraph::cGraph()
{
	//
}

cGraph::~cGraph()
{
	//
}

void cGraph::addGraphVertex(int vertexId)
{
	vertices[vertexId].clear();
}

void cGraph::removeGraphVertex(int vertexId)
{
	if ( vertices.size()==0 )
		return; // NULL; // Nothing to do
	if ( vertices.find(vertexId) == vertices.end() )
		return; // NULL; // No such vertex

	__eraseIncidentEdges(vertexId);
	vertices.erase(vertices.find(vertexId));
}

int cGraph::addGraphEdge(int vertexIdA, int vertexIdB, double weight )
{
	static int edgecounter=0;
	return __addEdge(vertexIdA, vertexIdB, ++edgecounter, weight );
}

int  cGraph::removeGraphEdge(int edgeId)
{
	if ( edges.find(edgeId) == edges.end() )
		return E_EDGE_DOES_NOT_EXIST;
	int a = edges[edgeId].a;
	int b = edges[edgeId].b;
	removeGraphEdge(a,b);
	return 0;
}

int cGraph::removeGraphEdge(int vertexIdA, int vertexIdB)
{
	return __removeEdge(vertexIdA,vertexIdB);
}

void cGraph::getIncidentVertices(int nodeid, IntSet *set)
{
	set->clear();
	if ( vertices.find(nodeid) == vertices.end() )
		return;

	adjacencyListType::iterator iter;
	for(iter=vertices[nodeid].begin();iter!=vertices[nodeid].end();iter++)
		set->insert((*iter).first);
}

bool cGraph::hasVertex(int vertexId)
{
	return ( vertices.find(vertexId) != vertices.end() );
}

bool cGraph::hasEdge(int nodeIdA, int nodeIdB)
{
	if ( vertices.find(nodeIdA) == vertices.end() ||
	     vertices.find(nodeIdB) == vertices.end() )
		return false;

	return ( vertices[nodeIdA].find(nodeIdB) != vertices[nodeIdA].end() );
}

int cGraph::getVertexCount()
{
	return vertices.size();
}

int cGraph::getEdgeCount()
{
	return edges.size();
}

int cGraph::getIncidentEdgeCount(int vertexId)
{
	if ( vertices.find(vertexId) == vertices.end() )
		return 0;
	else
		return vertices[vertexId].size();
}

void cGraph::getEdgeIds(intlist *list)
{
	list->clear();
	edgeMapType::iterator iter;
	for ( iter=edges.begin(); iter!=edges.end(); iter++)
		list->push_back((*iter).first);
}

int cGraph::pickRandomVertex()
{
	if ( vertices.size()<1 )
		return -1;
	if ( vertices.size()==1 )
		return (*vertices.begin()).first;
	int idx = uniform(0,vertices.size());
	vertexMapType::iterator pos = vertices.begin();
	std::advance(pos,idx);
	return (*pos).first;
}

int cGraph::pickRandomVertex(int excludeId)
{
	if ( vertices.size()<2 )
		return -1;
	int id;
	do
	{
		id = pickRandomVertex();
	}
	while(id==excludeId);
	return id;
}

int cGraph::pickRandomNeighbor(int vertexId)
{
	if ( vertexId==-1 )
		return -1;
	if ( vertices.size()==0 )
		return -1;
	if ( vertices.find(vertexId)==vertices.end() )
		return -1;
	int idx = uniform(0,vertices[vertexId].size());
	adjacencyListType::iterator pos = vertices[vertexId].begin();
	std::advance(pos,idx);
	return (*pos).first;
}

int cGraph::pickRandomNeighbor(int nodeid, int maxhops)
{
	int neighbor=nodeid;
	int hops = uniform(1,maxhops+1);
	for ( int i=0; i<hops; ++i )
		neighbor = pickRandomNeighbor(neighbor);
	return neighbor;
}

int cGraph::pickRandomEdge()
{
	if ( edges.size()==0 )
		return -1;
	if ( edges.size()==1 )
		return (*edges.begin()).first;
	int idx = uniform(0,edges.size());
	edgeMapType::iterator pos = edges.begin();
	std::advance(pos,idx);
	return (*pos).first;
}

void cGraph::getConnectedComponent( int rootid, IntSet *set )
{
	__getConnectedNodes(rootid, set);
}

void cGraph::determineConnected( int id, IntSet *set, int k )
{
	if ( k==0 ) return;
	if ( set->size()==0 ) return;
	set->erase(id);
	adjacencyListType::iterator neighbor;
	for( neighbor=vertices[id].begin(); neighbor!=vertices[id].end(); neighbor++ )
	{
		determineConnected((*neighbor).first,set,k-1);
		if ( set->size()==0 ) return;
	}
}

int cGraph::__eraseIncidentEdges(int vertexid)
{
	int neighborid;
	adjacencyListType::iterator iter;
	int retval;
	for(iter=vertices[vertexid].begin();iter!=vertices[vertexid].end();iter++)
	{
		neighborid = (*iter).first;
		retval = __removeEdge(vertexid,neighborid);
		if ( retval != 0 )
			return retval;
	}
	vertices[vertexid].clear();
	return 0;
}

int cGraph::__addEdge(int vertexIdA, int vertexIdB, int edgeId, double weight)
{
	if ( vertices.find(vertexIdA) == vertices.end() )
		return E_VERTEX_DOES_NOT_EXIST;
	else if ( vertices[vertexIdA].find(vertexIdB) != vertices[vertexIdA].end() )
		return E_EDGE_EXISTS;

	if ( vertices.find(vertexIdB) == vertices.end() )
		return E_VERTEX_DOES_NOT_EXIST;
	else if ( vertices[vertexIdB].find(vertexIdA) != vertices[vertexIdB].end() )
		return E_EDGE_EXISTS;

	if ( edges.find(edgeId) != edges.end() )
		return E_EDGE_LIST_INCONSISTENT;

	vertices[vertexIdA][vertexIdB] = edgeId;
	vertices[vertexIdB][vertexIdA] = edgeId;

	edges[edgeId].a = vertexIdA;
	edges[edgeId].b = vertexIdB;
	edges[edgeId].weight = weight;

	return 0;
}

int cGraph::__removeEdge(int vertexIdA, int vertexIdB)
{
	if ( vertices.find(vertexIdA) == vertices.end() )
		return E_VERTEX_DOES_NOT_EXIST;
	if ( vertices[vertexIdA].find(vertexIdB) == vertices[vertexIdA].end() )
		return E_ADJACENCY_LIST_INCONSISTENT;

	int edgeId = vertices[vertexIdA][vertexIdB];
	if ( edges.find(edgeId) == edges.end() )
		return E_EDGE_LIST_INCONSISTENT;

	vertices[vertexIdA].erase(vertexIdB);
	vertices[vertexIdB].erase(vertexIdA);

	edges.erase(edgeId);

	return 0;
}

void cGraph::__getConnectedNodes( int nodeid, IntSet *set )
{
	adjacencyListType::iterator neighbor;
	for ( neighbor=vertices[nodeid].begin(); neighbor!=vertices[nodeid].end(); neighbor++ )
	{
		if ( set->find((*neighbor).first) == set->end() )
		{
			set->insert((*neighbor).first);
			__getConnectedNodes((*neighbor).first,set);
		}
	}
}

