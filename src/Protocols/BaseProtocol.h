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

#ifndef __SCALESIM_BASEPROTOCOL_H_
#define __SCALESIM_BASEPROTOCOL_H_

#include <omnetpp.h>
#include "topology_m.h"
#include "TopologyControlNIC.h"

#define BROADCAST -1

#define IP_HEADER_BYTE_LENGTH      20
#define TCP_HEADER_BYTE_LENGTH     20
#define HEADER_BYTE_LENGTH         IP_HEADER_BYTE_LENGTH+TCP_HEADER_BYTE_LENGTH

/**
 *  @brief The base protocol module
 *
 *  @author Kristjan V. Jonsson (kristjanvj@gmail.com)
 *  @version 0.9
 *  @date december 2011
 *
 *  @todo document
 */
class BaseProtocol : public cSimpleModule
{
protected:
	int nodeIdentifier;
	int mac;
	TopologyControlNIC *tcnic;
	cGate *outGate;
	int lowerInGateId;
	int auxInGateId;
	int lowerControlInId;
protected:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    int numInitStages() const {return 2;}
protected:
    virtual void handleSelfMessage(cMessage *msg);
    virtual void handleProtocolMessage(int senderId, cPacket *packet);
    virtual void handleAuxMessage(cMessage *msg);
    virtual bool updateReachable();
protected:
    void enumerateNeighbors(IntSet *set);
    bool newNeighbors();
    void encapsulateAndSend(cPacket *packet, int receiverId);
    cPacket* decapsulatePacket(cPacket *packet);
};

#endif
