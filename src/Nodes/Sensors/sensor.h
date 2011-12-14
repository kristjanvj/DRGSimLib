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

#ifndef __SCALESIM_SENSOR_H_
#define __SCALESIM_SENSOR_H_

#include <omnetpp.h>
#include "sensor_m.h"
#include "GlobalObserver.h"

/**
 *  @brief Sensor module
 *
 *  @author Kristjan V. Jonsson (kristjanvj@gmail.com)
 *  @version 0.9
 *  @date december 2011
 *
 *  @todo document
 */
class Sensor : public cSimpleModule
{
protected:
	bool enabled;
	cMessage *updateEvent;
	double baseReading;
	double valueMultiplier;
	simtime_t activationTime;
	cModule *globalObserver;
public:
	Sensor();
	virtual ~Sensor();
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
private:
    void sendUpdate();
    double getNewValue();
};

#endif
