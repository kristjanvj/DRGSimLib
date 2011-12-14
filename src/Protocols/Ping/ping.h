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

#ifndef __SCALESIM_PING_H_
#define __SCALESIM_PING_H_

#include <omnetpp.h>
#include "ping_m.h"
#include "BaseProtocol.h"

/**
 *  @brief Ping protocol. A simple protocol which inherits from the BaseProtocol class.
 *
 *  @author Kristjan V. Jonsson (kristjanvj@gmail.com)
 *  @version 0.9
 *  @date december 2011
 *
 *  @todo document
 */
class Ping : public BaseProtocol
{
protected:
	cMessage *timeout;
protected:
	virtual ~Ping();
protected:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
protected:
    void sendPingToPeer();
};

#endif
