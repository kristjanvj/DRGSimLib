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

#include "BaseProtocol.h"

Define_Module(BaseProtocol);

void BaseProtocol::initialize(int stage)
{
	if ( stage==0 )
	{
		nodeIdentifier = par("nodeid");

		cModule *nic = getParentModule()->getSubmodule("nic");
		if ( nic==NULL )
			error("NIC not found");
		tcnic = check_and_cast<TopologyControlNIC*>(nic);
		mac = tcnic->getMAC();
		if ( nodeIdentifier==-1 )
			nodeIdentifier = mac;

		outGate = gate("lowerOut");

		cGate *gate;
		gate = this->gate("lowerIn");
		if ( gate!=NULL )
			lowerInGateId=gate->getId();
		gate = this->gate("auxIn");
		if ( gate!=NULL )
			auxInGateId=gate->getId();
		gate = this->gate("lowerControlIn");
		if ( gate!=NULL )
			lowerControlInId=gate->getId();
	}
	else if ( stage==1 )
	{
		//
	}
}

void BaseProtocol::handleMessage(cMessage *msg)
{
	if ( msg->isSelfMessage() )
	{
		handleSelfMessage(msg);
	}
	else
	{
		if ( msg->arrivedOn(lowerInGateId) )
		{
			ev << getFullPath() << ": Handling a data message wrapped in a topology container" << endl;
			ev << "\tTITLE=" << msg->getDisplayString() << endl;
			cPacket *pkt = check_and_cast<cPacket*>(msg);
			mTopology *container = check_and_cast<mTopology*>(pkt);
			cPacket *encapsulatedPkt = decapsulatePacket(pkt);
			if ( encapsulatedPkt!=NULL )
				handleProtocolMessage(container->getSenderid(), encapsulatedPkt);
			delete msg;
		}
		else if ( msg->arrivedOn(auxInGateId) )
		{
			handleAuxMessage(msg);
			delete msg;
		}
		else if ( msg->arrivedOn(lowerControlInId) )
		{
			// only one type of control message expected at this time
			ev << getFullPath() << ": Handling a neighborhood change event" << endl;
			updateReachable();
			delete msg;
		}
	}
}

void BaseProtocol::handleSelfMessage(cMessage *msg)
{
	// Override in derived classes
}

void BaseProtocol::handleProtocolMessage(int senderId, cPacket *packet)
{
	// Override in derived classes
	// Note: Derived classes must delete the packet.
}

void BaseProtocol::handleAuxMessage(cMessage *msg)
{
	// Override in derived classes
}

bool BaseProtocol::updateReachable()
{
	// Override in derived classes
	return false;
}

void BaseProtocol::enumerateNeighbors(IntSet *set)
{
	tcnic->getNeighbors(set);
}

bool BaseProtocol::newNeighbors()
{
	return tcnic->newNeighbors();
}

void BaseProtocol::encapsulateAndSend(cPacket *packet, int receiverId)
{
	char szBuf[128];
	sprintf(szBuf,"%s %d->%d", packet->getName(), mac, receiverId);
	mTopology *m = new mTopology(szBuf);
	m->setSenderid(mac);
	m->setReceiverid(receiverId);
	packet->setTimestamp(simTime()); // Stamp both the payload and the transport packet with current time
	m->setTimestamp(simTime());
	m->setByteLength(HEADER_BYTE_LENGTH + packet->getByteLength()); // Encapsulate assuming TCP overhead
	m->encapsulate(packet);
	send(m,outGate);
}

cPacket* BaseProtocol::decapsulatePacket(cPacket *packet)
{
	return packet->decapsulate();
}
