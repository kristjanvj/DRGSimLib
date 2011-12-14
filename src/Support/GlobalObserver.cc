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

#include "GlobalObserver.h"

Define_Module(GlobalObserver);

GlobalObserver::GlobalObserver()
{
	updateEvent=NULL;
}

GlobalObserver::~GlobalObserver()
{
	cancelAndDelete(updateEvent);
}

void GlobalObserver::initialize()
{
    filename = (const char*)par("filename");
    epochLenght = par("epochLenght");
    updateEvent = new cMessage();
    curEpochStart=simTime().dbl();
}

void GlobalObserver::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() )
    {
    	epochUpdate();
    }
    else
    {
    	handleUpdateMessage(msg);
    }
}

void GlobalObserver::epochUpdate()
{
	cStdDev results;
	simtime_t updateTime = simTime();
	updateVectorType::iterator iter;
	for (iter=updateVector.begin(); iter!=updateVector.end(); iter++)
		results.collect((*iter).value);

	logEpochAggregate(updateTime,results);
	setDisplayString(updateTime,results);
	updateVector.clear();
}

void GlobalObserver::handleUpdateMessage(cMessage *msg)
{
	cPacket *pkt = check_and_cast<cPacket*>(msg);
	mSensorUpdate *sensorUpdate = check_and_cast<mSensorUpdate*>(pkt);
	if ( sensorUpdate->getCreationTime().dbl() >= curEpochStart )
	{
		SENSOR_UPDATE update;
		update.time = sensorUpdate->getCreationTime();
		update.senderid = sensorUpdate->getSenderId();
		update.value = sensorUpdate->getValue();
		updateVector.push_back(update);
	}
	delete msg;

	if ( !updateEvent->isScheduled() )
	{
		scheduleAt(simTime()+epochLenght, updateEvent);
		curEpochStart=simTime().dbl();
	}
}

void GlobalObserver::logEpochAggregate( simtime_t updateTime, cStdDev &data )
{
	static unsigned long epochcounter=0;

	epochcounter++;
	if ( data.getCount()==0 )
		return;

	if ( filename.size()==0 )
		return;
	FILE *f = fopen(filename.c_str(),"a");
	if ( f==NULL )
	{
		ev << getFullPath()
		   << ": ERROR: Cannot open file '" << filename << "' for appending" << endl;
		return;
	}

	double sem = data.getStddev()/sqrt(data.getCount());
	fprintf(f,"%ld\t%.6f\t%.6f\t%ld\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\r\n",
			epochcounter, updateTime.dbl(), curEpochStart, data.getCount(),
			data.getMean(), data.getStddev(), data.getMin(), data.getMax(), 1.96*sem );
	fclose(f);
}

void GlobalObserver::setDisplayString( simtime_t updateTime, cStdDev &data )
{
	if ( !ev.isGUI() )
		return;
	char szBuf[128];
	sprintf(szBuf,"t=%.2f: c=%ld,ave=%.2f", updateTime.dbl(), data.getCount(), data.getMean());
	this->getDisplayString().setTagArg("t",0,szBuf);
}
