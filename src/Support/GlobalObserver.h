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

#ifndef __SCALESIM_GLOBALOBSERVER_H_
#define __SCALESIM_GLOBALOBSERVER_H_

#include <omnetpp.h>
#include <string>
#include <vector>
#include <cstddev.h>
#include "sensor_m.h"

struct SENSOR_UPDATE
{
	int senderid;
	simtime_t time;
	double value;
};

typedef std::vector<SENSOR_UPDATE> updateVectorType;

/**
 *  @brief Global observer module.
 *
 *  Used to record observations produced by Sensr modules.
 *  A single global observer should be instantiated at scenario level.
 *
 *  @author Kristjan V. Jonsson (kristjanvj@gmail.com)
 *  @version 0.9
 *  @date december 2011
 *
 *  @todo document
 */
class GlobalObserver : public cSimpleModule
{
private:
	std::string filename;
	double epochLenght;
	double curEpochStart;
	cMessage *updateEvent;
	updateVectorType updateVector;
public:
	GlobalObserver();
	virtual ~GlobalObserver();
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
protected:
    void epochUpdate();
    void handleUpdateMessage(cMessage *msg);
    void logEpochAggregate( simtime_t updateTime, cStdDev &data );
    void setDisplayString( simtime_t updateTime, cStdDev &data );
};

#endif
