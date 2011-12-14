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

packet mGapBase 
{
    int senderId;
    unsigned long serial;
    int level;
}

packet mGapQuery extends mGapBase 
{
    int querierId;
    int queryId;
    //int senderLevel; // The level of the sender
    int repeat = 1;  // One shot is default
    int maxLevelEstimate = 20;
}

//
// Delivered locally upon detection of a failure
//
packet mGapFail
{
    int failedId;
}

//
// Delivered locally upon detection of a new node
//  
//packet mGapNew
//{
// 	int newId;       
//}

packet mGapHello extends mGapBase
{
    int level;
    int parent;
}

struct AggregateData
{
    int count;
	double sum;
	double min;
	double max;
	int maxLevel;
};

//
// The main update message
//
packet mGapUpdate extends mGapHello
{
	//int weight;    
	//int level;
	//int parentId;
	AggregateData aggregateData;	
}

//
// The local weight update message
//
//packet mGapWeight
//{
// 	int weight;   
//}

//
// The local timeout message
//
//packet mGapTimeout
//{
//}

