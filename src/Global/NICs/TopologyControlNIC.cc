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

#include "TopologyControlNIC.h"

Define_Module(TopologyControlNIC);

void TopologyControlNIC::initialize(int stage)
{
	if ( stage==0 )
	{
		transmittedCount = 0;
		errorDropCount = 0;
		unreachableDropCount = 0;

		neighborChangeFlag=true;

		dataRate = par("dataRate");
		dataRate *= 1024;
		double bitErrorRate = par("bitErrorRate");
		lossless = (bitErrorRate==0.0);

		topology = simulation.getSystemModule()->getSubmodule("topology");
		if ( topology == NULL )
			error("Topology control not found");
		tcontrol = dynamic_cast<TopologyControl*>(topology->getSubmodule("topologyControl"));
		if ( tcontrol==NULL )
			error("Topology control module not found");

		registerNode();
	}
	else if ( stage==1 )
	{
		tcontrol->enumerateNeighbors(getId(),&neighbors);
	}
}

void TopologyControlNIC::finish()
{
	unregisterNode();
	recordScalar("transmittedCount", transmittedCount);
	recordScalar("errorDropCount", errorDropCount);
	recordScalar("unreachableDropCount", unreachableDropCount);
}

void TopologyControlNIC::handleMessage(cMessage *msg)
{
	if ( msg->isSelfMessage() )
	{
		// No self messages expected at this time
	}
	else
	{
		cPacket *packet = check_and_cast<cPacket*>(msg);
		if ( packet->arrivedOn("upperIn") )
		{
			if ( !lossless && dropMessage(packet->getBitLength()) )
			{
				// Drop the message based on the BER.
				delete packet;
				errorDropCount++;
				return;
			}
			sendToNeighbors(packet);
		}
		else if ( packet->arrivedOn("networkIn") )
		{
			ev << getFullPath() << ": Received a message @time " << simTime()
			   << ". Transmitted at T=" << packet->getSendingTime() << ", arrived at T=" << packet->getArrivalTime() << endl;
			send(packet,"upperOut");
		}
		else if ( msg->arrivedOn("controlIn") )
		{
			handleControlMessage(msg);
			delete msg;
		}
	}
}

void TopologyControlNIC::handleControlMessage(cMessage *msg)
{
	// Only one control message expected at this time -- the topology change notification.
	neighbors.clear();
	tcontrol->enumerateNeighbors(getId(),&neighbors);
	neighborChangeFlag=true;
	cMessage *notify = new cMessage();
	sendDelayed(notify, 30.0+truncnormal(0,30), "upperControlOut"); // TODO: Make the notification delay a NED parameter
}

void TopologyControlNIC::getNeighbors(IntSet *set)
{
	Enter_Method_Silent();
	ModuleMap::iterator iter;
	(*set) = neighbors;
	neighborChangeFlag = false;
}

bool TopologyControlNIC::newNeighbors()
{
	Enter_Method_Silent();
	return neighborChangeFlag;
}

int TopologyControlNIC::getMAC()
{
	Enter_Method_Silent();
	return getId();
}

void TopologyControlNIC::registerNode()
{
	tcontrol->registerNode(getId());
}

void TopologyControlNIC::unregisterNode()
{
	tcontrol->unregisterNode(getId());
}

void TopologyControlNIC::sendToNeighbors(cPacket *packet)
{
	if ( neighbors.size()==0 )
		tcontrol->enumerateNeighbors(getId(),&neighbors); // Make sure the neighbors list is current
	mTopology *tmsg = check_and_cast<mTopology*>(packet);
	int receiverid = tmsg->getReceiverid();
	if ( receiverid==-1 )
	{
		broadcastToNeighbors(packet);
		delete packet;  // Delete the original -- duplicates sent to peers
	}
	else
	{
		sendToNeighbor(receiverid,packet);
	}
}

void TopologyControlNIC::sendToNeighbor( int neighborId, cPacket *packet )
{
	cModule *module = simulation.getModule(neighborId);
	if ( neighbors.find(neighborId) == neighbors.end() || module==NULL )
	{
		delete packet; // No such receiver in the current neighborhood
		unreachableDropCount++;
	}
	else
	{
		double propagationDelay = par("propagationDelay");
		double duration = par("processingDelay");
		duration += (double)packet->getBitLength()/dataRate;
		sendDirect( packet, propagationDelay, duration, module, "networkIn" );
		transmittedCount++;
	}
}

void TopologyControlNIC::broadcastToNeighbors(cPacket *packet)
{
	IntSet::iterator iter;
	cModule *module;
	double propagationDelay;
	double duration;
	for ( iter=neighbors.begin(); iter!=neighbors.end(); iter++ )
	{
		module = simulation.getModule((*iter));
		if ( module != NULL )
		{
			propagationDelay = par("propagationDelay");
			duration = par("processingDelay");
			duration += (double)packet->getBitLength()/dataRate;
			sendDirect( packet->dup(), propagationDelay, duration, module, "networkIn" );
			transmittedCount++;
		}
	}
}

bool TopologyControlNIC::dropMessage( int messageBitSize )
{
	if ( lossless )
		return false;
	// Simple BER failure modeling. A single bit failure invalidates the packet.
	double ber = par("bitErrorRate");
	double p = messageBitSize*ber;
	return bernoulli(p);
}
