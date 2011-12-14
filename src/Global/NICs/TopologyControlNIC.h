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

#ifndef __SCALESIM_TOPOLOGYCONTROLNIC_H_
#define __SCALESIM_TOPOLOGYCONTROLNIC_H_

#include <omnetpp.h>
#include <queue>
#include <set>
#include "topologyControl.h"

#define NIC_CONTROL_NEIGHBORHOOD_CHANGE 40001

typedef std::queue<cMessage*> MessageQueue;

/**
 *  @brief TopologyControlNIC module
 *
 *  @author Kristjan V. Jonsson (kristjanvj@gmail.com)
 *  @version 0.9
 *  @date december 2011
 *
 *  @todo document
 */
class TopologyControlNIC : public cSimpleModule
{
protected:
	cModule *topology;
	TopologyControl *tcontrol;
	IntSet neighbors;
	bool neighborChangeFlag;    // Flag to protocol of pending changes
	MessageQueue messageQueue;
	int dataRate;
	bool lossless;
	unsigned long transmittedCount;
	unsigned long errorDropCount;
	unsigned long unreachableDropCount;
protected:
    virtual void initialize(int stage);
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    int numInitStages() const {return 2;}
protected:
    void handleControlMessage(cMessage *msg);
public:
    void getNeighbors(IntSet *set);
    bool newNeighbors();
    int  getMAC();
protected:
    void registerNode();
    void unregisterNode();
    void sendToNeighbors(cPacket *packet);
    void sendToNeighbor( int neighborId, cPacket *packet );
    void broadcastToNeighbors(cPacket *packet);
    void refreshNeighbors();
    bool dropMessage( int messageBitSize );
};

#endif
