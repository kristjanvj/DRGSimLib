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

#ifndef GRAPH_H_
#define GRAPH_H_

#include <stdlib.h>
#include <time.h>
#include <map>
#include <set>
#include <list>
#include <omnetpp.h>

#define E_VERTEX_DOES_NOT_EXIST                     1
#define	E_EDGE_EXISTS                               2
#define E_EDGE_LIST_INCONSISTENT                    3
#define E_VERTEX_EXISTS                             4
#define E_ADJACENCY_LIST_INCONSISTENT               5
#define E_VERTEX_EXISTS_IN_ADJACENCY_LIST           6
#define E_VERTEX_DOES_NOT_EXIST_IN_ADJACENCY_LIST   7
#define E_EDGE_DOES_NOT_EXIST                       8

typedef std::list<int> intlist;
typedef std::set<int> IntSet;

typedef std::map<int,int> adjacencyListType;

/**
 * vertexMapType
 *
 * This is the data structure used to store vertices and vertex relations (edges).
 * Mapping is as follows:
 * vertex[id] -> (single vertex id, adjacency list)
 *                                                 -> (neighbor id, edge id)
 */
typedef std::map<int,adjacencyListType> vertexMapType;

struct EDGE_DATA
{
	int a;
	int b;
	double weight;
};
typedef std::map<int,EDGE_DATA> edgeMapType;

/**
 *  @brief cGraph class
 *
 *  The graph datastructure used by TopologyControl.
 *
 *  @author Kristjan V. Jonsson (kristjanvj@gmail.com)
 *  @version 0.9
 *  @date december 2011
 *
 *  @todo document
 *  @todo rewrite with Boost graph classes?
 */
class cGraph
{
public:
	vertexMapType vertices;  // The vertices map
	edgeMapType edges;       // The edge map
public:
	cGraph();
	virtual ~cGraph();
public:
	void addGraphVertex(int a);
	void removeGraphVertex(int a);
    int  addGraphEdge(int vertexIdA, int vertexIdB, double weight=1.0);
    int  removeGraphEdge(int edgeId);
    int  removeGraphEdge(int vertexIdA, int vertexIdB);
public:
    void  getIncidentVertices(int nodeid, IntSet *set); // Returns a list of node IDs

    bool hasEdge(int nodeIdA, int nodeIdB);
    bool hasVertex(int vertexId);
    int  getVertexCount();
    int  getEdgeCount();
    int  getIncidentEdgeCount(int vertexId);
    void getEdgeIds(intlist *list);

    int  pickRandomVertex();
    int  pickRandomVertex(int excludeId);
    int  pickRandomNeighbor(int vertexId);
    int  pickRandomNeighbor(int nodeid, int maxhops);
    int  pickRandomEdge();

    void getConnectedComponent( int rootid, IntSet *set );
    void determineConnected( int id, IntSet *set, int k );

protected:
    int __addEdge(int vertexIdA, int vertedIxB, int edgeId, double weight);
    int __removeEdge(int vertexIdA, int vertexIdB);
    int __eraseIncidentEdges(int vertexid);
    void __getConnectedNodes( int nodeid, IntSet *set );
};


#endif /* GRAPH_H_ */
