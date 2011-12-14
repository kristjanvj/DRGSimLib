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

#include "sensor.h"

Define_Module(Sensor);

Sensor::Sensor()
{
	updateEvent=NULL;
}

Sensor::~Sensor()
{
	if (updateEvent!=NULL)
		cancelAndDelete(updateEvent);
}

void Sensor::initialize()
{
	enabled = par("enabled");
	baseReading = par("valueInitial");        // The initial value of the sensor (minus noise)
    valueMultiplier = par("valueMultiplier");  // The slope of the sensor trend
    activationTime = simTime();
    double updateTimeDistribution = par("updateTimeDistribution");
    if ( updateTimeDistribution<=0.0 )
    	enabled=false;

    globalObserver = simulation.getSystemModule()->getSubmodule("globalObserver");

    updateEvent = new cMessage();
    if ( enabled )
    {
    	double nextUpdateTime = par("updateTimeDistribution"); // This is a volatile parameter
    	scheduleAt(simTime()+nextUpdateTime,updateEvent);
    }
}

void Sensor::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() )
    {
    	sendUpdate();
    	double nextUpdateTime = par("updateTimeDistribution");
    	scheduleAt(simTime()+nextUpdateTime,updateEvent);
    }
    else
    {
    	mSensorControl *ctrl = check_and_cast<mSensorControl*>(msg);
    	if ( ctrl->getCode() == enable )
    	{
    		if ( updateEvent->isScheduled() )
    			cancelEvent(updateEvent);
    		enabled = true;
        	double nextUpdateTime = par("updateTimeDistribution"); // This is a volatile parameter
        	if ( nextUpdateTime > 0.0 )
        		scheduleAt(simTime()+nextUpdateTime,updateEvent);
    	}
    	else if ( ctrl->getCode() == disable )
    	{
    		if ( updateEvent->isScheduled() )
    			cancelEvent(updateEvent);
    		enabled = false;
    	}
    	else if ( ctrl->getCode() == poll )
    	{
    		sendUpdate();
    	}
    	delete msg;
    }
}

void Sensor::sendUpdate()
{
	mSensorUpdate *update = new mSensorUpdate("SENSOR");
	update->setValue(getNewValue());

	// Send a copy directly to global observer if it is defined
	if ( globalObserver!=NULL )
		sendDirect(update->dup(),globalObserver,"in");

	// Send to the host
	send(update,"out");
}

double Sensor::getNewValue()
{
	double noisevalue = par("valueNoiseDistribution");
    int stepMultiplier = ((int)simTime().dbl()) / 3600;
	return baseReading*(stepMultiplier+1) + valueMultiplier*simTime().dbl() + noisevalue;
}
