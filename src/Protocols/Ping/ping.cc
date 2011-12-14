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

#include "ping.h"

Define_Module(Ping);

Ping::~Ping()
{
	cancelAndDelete(timeout);
}

void Ping::initialize(int stage)
{
	if ( stage==1 )
	{
		BaseProtocol::initialize(stage);
		timeout = new cMessage();
		double interval = par("pingInterval");
		if ( interval > 0.0 )
			scheduleAt(simTime()+interval, timeout);
	}
}

void Ping::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() )
    {
    	sendPingToPeer();
        double interval = par("pingInterval");
        scheduleAt(simTime()+interval, timeout);
    }
    else
    {
    	cPacket *pkt = check_and_cast<cPacket*>(msg);
    	cPacket *pingPkt = decapsulatePacket(pkt);
    	delete pkt; // delete the outer packet
    	if ( pingPkt!=NULL )
    	{
			mPing *ping = check_and_cast<mPing*>(pingPkt);
			ev << getFullName() << ": Received ping from a peer @ " << simTime() << endl;
			ev << "\tSender=" << ping->getSenderid() << endl;
			ev << "\tReceiver=" << ping->getReceiverid() << endl;
			ev << "\tCounter=" << ping->getCounter() << endl;
			ev << "\t\tCreation time:  " << ping->getCreationTime() << endl;
			ev << "\t\tArrival time:   " << simTime() << endl;
			ev << "\t\t\tDelay:        " << ( simTime() - ping->getCreationTime() ) << endl;
			delete pingPkt;
    	}
    }
}

void Ping::sendPingToPeer()
{
	static int counter=0;

	ev << getFullPath() << ": Sending ping to a peer" << endl;

	IntSet set;
	int peeridx,peerid;
	enumerateNeighbors(&set);
	int nNeighbors = set.size();
	if (nNeighbors<1)
	{
		ev << "\tNo peers found. Cannot send ping." << endl;
		return;
	}
	else
	{
		// Pick a random peer and send
		peeridx = uniform(0,nNeighbors-1);
		IntSet::iterator pos = set.begin();
		std::advance(pos,peeridx);
		peerid = *pos;
		ev << "\tsending ping to " << peerid << endl;
	}

	mPing *ping = new mPing();
	ping->setSenderid(mac);
	ping->setReceiverid(peerid);
	ping->setCounter(counter++);

	encapsulateAndSend(ping,peerid);
}

