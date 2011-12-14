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

#ifndef __GAP_PROTOCOL_H_
#define __GAP_PROTOCOL_H_

#include <omnetpp.h>
#include <map>
#include "BaseProtocol.h"
#include "gap_m.h"
#include "events_m.h"
#include "sensor_m.h"

#define GAP_EVENT_QUERIER_INITIATE		20001
#define GAP_EVENT_SEND_UPDATE           20003
#define GAP_EVENT_FORWARD_QUERY         20007

#define GAP_MESSAGE_QUERY				20010
#define GAP_MESSAGE_UPDATE				20012

#define RTT_INITIAL_EST                   2.0

#define HELLO_INTERVAL                    1.0
#define FAILURE_DET_INTERVAL              3*HELLO_INTERVAL

#define HELLO_BYTE_LENGTH                  10
#define UPDATE_BYTE_LENGTH                 12

#define UNASSIGNED                         -1

//#define SHORT_UPDATE_DELAY                  1.0
#define INITIAL_DIAMETER_ESTIMATE          5

enum Status {unassigned,parentnode,parentpeer,self,child,peer};

struct NODE_ENTRY
{
	Status status;
	int level;
	int parentId;
	simtime_t lastUpdate;         // Last update time for the node
	unsigned long lastSerial;
	AggregateData data;
	int queryId;
	simtime_t lastAggregateUpdate; // Set when peer delivers aggregate, reset when data zeroed.
};

struct QUERY_INFO
{
	int querierId;
	int queryId;
	int counter;             //
	simtime_t lastUpdate;    // The most recent aggregate update for this query
	int maxLevelEstimate;    // The querier estimate of the network size which can potentially be useful
	simtime_t timeStamp;     // The time when the query was installed or refreshed
};

typedef std::map<int,NODE_ENTRY> NeighborhoodTableType;

const double rttAlpha = 0.5;
const double diameterAlpha = 0.5;

/**
 *  @brief GAP protocol.
 *
 *  GAP -- an implementation of the Generic Aggregation Protocol as
 * described by Dam and Stadler (2005) with minor modifications.
 * One modification is the query message which is used in this
 * implmentation to bootstrap the aggregation system.
 *
 *  @author Kristjan V. Jonsson (kristjanvj@gmail.com)
 *  @version 0.9
 *  @date december 2011
 *
 *  @todo document
 */
class gap : public BaseProtocol
{
public:
	gap();
	virtual ~gap();
protected:
	mInitiateProtocolEvent *initiateProtocolTimeout;
	mSendUpdateEvent *sendUpdateTimeout;
	mQueryForwardEvent *queryForwardEvent;
	NeighborhoodTableType neighborhoodTable;
	bool iamroot;
	double rttEstimate;
	unsigned long msgcounter;
	QUERY_INFO queryInfo;
	std::string recordingFileName;
	std::string logFileName;
	bool recordingEnabled;
	bool loggingEnabled;
	double updateDelay;
	int diameterEstimate;
	AggregateData localData;
	mGapQuery *cachedQuery;
	bool sensorActive;
protected:
    virtual void initialize(int stage);
private:
    void handleSelfMessage(cMessage *msg);
    void handleProtocolMessage(int senderId, cPacket *packet);
    void handleAuxMessage(cMessage *msg);
    void handleQueryMessage(int senderId, cPacket *packet);
    void handleUpdateMessage(int senderId, cPacket *packet);
private:
    void initiateQuery();
    void sendUpdate(int peerId, AggregateData &data);
    void broadcastUpdate(AggregateData &data);
    void forwardQuery( mGapQuery *query );
private:
    bool intable(int peerId);

    void initializeself(bool rootnode);
    void removeEntry(int peerId);
    void updateEntry(int peerId, unsigned long serial, int level=-1, int parentId=-1);

    void setQueryInfo(int querierId, int queryId, int repeat, int maxLevelEstimate, bool overwrite);
    void resetQueryInfo();
    void scheduleUpdate();
    void update();
    void update(AggregateData &data);
    void storeRootAggregate();
    void saveAggregate(AggregateData &data);

    int  level();
    int  level(int peerId);
    int  parent();
    int  parent(int peerId);
    bool iamleaf();
    AggregateData& getAggregateData(int peerId);
    void setAggregateData(int peerId, AggregateData &data);
    void setLocalAggregateData(double value);
    void zeroAggregateData();
    void zeroAggregateData(int peerId);
    void zeroAggregateData(AggregateData &data);
    void appendAggregateData(AggregateData &src, AggregateData &dest);
    bool computePartialAggregate(AggregateData &data);

    bool restoreTableInvariant();
    int  getCurrentMinLevel();
    bool setRelations(int parentLevel);
    bool setRootRelations();
    bool setPopulationRelations(int parentLevel);
    void timeoutScan();
    int  getUnknownCount();

    bool updateReachable();

    void enableSensor();
    void disableSensor();

    void updateRttEstimate(double delay);
    void updateDiameterEstimate(int curDiameter);

    void setStatusText();

    std::string peerStatusToStr(Status status);;

    void logMessage(std::string msg);
};

#endif /* __GAP_PROTOCOL_H_ */
