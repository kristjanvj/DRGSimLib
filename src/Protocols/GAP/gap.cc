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

#include "gap.h"

//#define DEBUG
//#define NEIGHBORHOODTABLE_IN_STATUS_TEXT

Define_Module(gap);

gap::gap()
{
	initiateProtocolTimeout=NULL;
	sendUpdateTimeout=NULL;
	queryForwardEvent=NULL;
	cachedQuery=NULL;
}

gap::~gap()
{
	cancelAndDelete(initiateProtocolTimeout);
	cancelAndDelete(sendUpdateTimeout);
	cancelAndDelete(queryForwardEvent);

	if ( cachedQuery!=NULL )
		delete cachedQuery;
}

void gap::initialize(int stage)
{
    BaseProtocol::initialize(stage);

    if ( stage==1 )
    {

		#ifdef DEBUG
		ev << getFullPath() << ": Initializing GAP protocol for node " << nodeid << endl;
		#endif

		cachedQuery = NULL;
		msgcounter = 1;
		resetQueryInfo();
		sensorActive = false;

		recordingFileName = (const char *)par("recordingFileName");
		recordingEnabled = ( recordingFileName.size()!=0 );
		logFileName = (const char *)par("logFileName");
		loggingEnabled = ( logFileName.size()!=0 );

		updateDelay = par("updateDelay");

		zeroAggregateData(localData);

		rttEstimate = RTT_INITIAL_EST; // Conservative initial RTT estimate
		iamroot = ( nodeIdentifier==0 );

		initializeself(iamroot);
		diameterEstimate = INITIAL_DIAMETER_ESTIMATE;

		if (iamroot)
		{
			initiateProtocolTimeout = new mInitiateProtocolEvent();
			initiateProtocolTimeout->setKind(GAP_EVENT_QUERIER_INITIATE);
			scheduleAt(simTime()+600.0,initiateProtocolTimeout); // Schedule the first query message
		}
		else
		{
			queryForwardEvent = new mQueryForwardEvent();
			queryForwardEvent->setKind(GAP_EVENT_FORWARD_QUERY);
		}

		disableSensor();
		updateReachable();

		logMessage("Initialized");

		setStatusText();
    }
}

void gap::handleSelfMessage(cMessage *msg)
{
	if ( msg->getKind() == GAP_EVENT_QUERIER_INITIATE )
	{
		if ( iamroot )
			initiateQuery();
	}
	else if ( msg->getKind() == GAP_EVENT_SEND_UPDATE )
	{
		update();
	}
	else if ( msg->getKind() ==  GAP_EVENT_FORWARD_QUERY )
	{
		if ( cachedQuery != NULL )
			forwardQuery( cachedQuery );
	}
}

void gap::handleProtocolMessage(int senderId, cPacket *packet)
{
	simtime_t delay = simTime() - packet->getTimestamp();
	updateRttEstimate(delay.dbl());

	ev << getFullPath() << ": Handling a protocol message" << endl;

	if ( packet->getKind() == GAP_MESSAGE_QUERY )
		handleQueryMessage(senderId, packet);
	else if ( packet->getKind() == GAP_MESSAGE_UPDATE )
		handleUpdateMessage(senderId, packet);
	delete packet;
}

void gap::handleAuxMessage(cMessage *msg)
{
	ev << getFullPath() << ": Receiving an aux message" << endl;

	if ( cachedQuery==NULL )
		return; // Ignore all

	mSensorUpdate *update = check_and_cast<mSensorUpdate*>(msg);
	ev << getFullPath() << ": Received sensor update package. Value=" << update->getValue() << endl;
	setLocalAggregateData(update->getValue());
	// If protocol is active, then set update timeout
	setStatusText();
}

void gap::handleQueryMessage(int senderId, cPacket *packet)
{
	if ( iamroot )
		return;

	ev << getFullPath() << ": Received a query message" << endl;
	mGapQuery *q = check_and_cast<mGapQuery*>(packet);
	ev << getFullPath() << ": Received a query from peer " << q->getSenderId() << ". "
		                << "Querier ID=" << q->getQuerierId() << endl;
	updateEntry(q->getSenderId(), q->getSerial(), q->getLevel(), UNASSIGNED);
	if ( queryInfo.queryId != q->getQueryId() )
	{
		logMessage("New query received");
		setQueryInfo( q->getQuerierId(), q->getQueryId(), q->getRepeat(), q->getMaxLevelEstimate(), false );
		zeroAggregateData();
		neighborhoodTable[q->getSenderId()].queryId = q->getQueryId();

		if ( cachedQuery!=NULL )
			delete cachedQuery;
		cachedQuery = q->dup(); // cache a copy

		enableSensor();
	}
	else
	{
		queryInfo.timeStamp = simTime(); // Only modify the timestamp if this is a repeat query
	}
	restoreTableInvariant();

	// Forward query after a brief delay to help construct a shallow tree
	if ( queryForwardEvent->isScheduled() )
		cancelEvent(queryForwardEvent);
	scheduleAt(simTime()+0.5*rttEstimate,queryForwardEvent);
}

void gap::handleUpdateMessage(int senderId, cPacket *packet)
{
	ev << getFullPath() << ": Received a update message" << endl;
	mGapUpdate *update = check_and_cast<mGapUpdate*>(packet);
	ev << getFullPath() << ": Received a update message from " << update->getSenderId() << endl;
	updateEntry(update->getSenderId(), update->getSerial(), update->getLevel(), update->getParent());
	restoreTableInvariant();
	setAggregateData(update->getSenderId(), update->getAggregateData());
}

void gap::initiateQuery()
{
	ev << getFullPath() << ": Initiating query" << endl;

	static int queryId=0;
	mGapQuery *q = new mGapQuery("QUERY");
	q->setKind(GAP_MESSAGE_QUERY);
	q->setSenderId(mac);
	q->setSerial(msgcounter++);
	q->setQuerierId(mac);
	q->setQueryId(queryId++);
	q->setMaxLevelEstimate(2+diameterEstimate);
	q->setLevel(0);

	// Store the query state for the root.
	queryInfo.queryId = queryId;
	queryInfo.querierId = mac;
	queryInfo.maxLevelEstimate = 2+diameterEstimate;
	queryInfo.lastUpdate = simTime();

	encapsulateAndSend(q,BROADCAST);
}

void gap::sendUpdate(int peerId, AggregateData &data)
{
	ev << getFullPath() << ": Sending update to " << peerId << endl;

	mGapUpdate *update = new mGapUpdate("UPDATE");
	update->setKind(GAP_MESSAGE_UPDATE);
	update->setSenderId(mac);
	update->setSerial(msgcounter++);
	update->setLevel(level(mac));
	update->setAggregateData(data);
	update->setParent(parent(mac));
	update->setByteLength(UPDATE_BYTE_LENGTH);
	encapsulateAndSend(update,peerId);
	queryInfo.lastUpdate = simTime();
	queryInfo.counter++;
}

void gap::broadcastUpdate(AggregateData &data)
{
	ev << getFullPath() << ": Broadcasting update" << endl;

	mGapUpdate *update = new mGapUpdate("UPDATE");
	update->setKind(GAP_MESSAGE_UPDATE);
	update->setSenderId(mac);
	update->setSerial(msgcounter++);
	update->setLevel(level(mac));
	update->setAggregateData(data);
	update->setParent(parent(mac));
	update->setByteLength(UPDATE_BYTE_LENGTH);

	NeighborhoodTableType::iterator neighbor;
	for( neighbor=neighborhoodTable.begin(); neighbor!=neighborhoodTable.end(); neighbor++ )
	{
		if ( (*neighbor).first == mac )
			continue;
		encapsulateAndSend(update->dup(), (*neighbor).first );
	}
	delete update;

	queryInfo.lastUpdate = simTime();
	queryInfo.counter++;
}

void gap::forwardQuery( mGapQuery *query )
{
	ev << getFullPath() << ": Forwarding query with id=" << query->getQueryId() << ". Querier=" << query->getQuerierId() << endl;
	query->setSenderId(mac);
	query->setSerial(msgcounter++);
	query->setLevel(level(mac));
	NeighborhoodTableType::iterator iter;

	for ( iter=neighborhoodTable.begin(); iter!=neighborhoodTable.end(); iter++ )
	{
		if ( (*iter).second.queryId!=query->getQueryId() )
		{
			encapsulateAndSend(query->dup(),(*iter).first);
			(*iter).second.queryId = query->getQueryId();
		}
	}
}

void gap::initializeself(bool rootnode)
{
	if (rootnode)
	{
		neighborhoodTable[mac].level=0;
		neighborhoodTable[mac].parentId=0;
	}
	else
	{
		neighborhoodTable[mac].level=-1;
		neighborhoodTable[mac].parentId=-1;
	}
	neighborhoodTable[mac].status=self;
	neighborhoodTable[mac].lastUpdate=simTime(); // The time of initialization for the local node. Never updated.
	zeroAggregateData(mac);
}

bool gap::intable(int peerId)
{
	return ( neighborhoodTable.find(peerId) != neighborhoodTable.end() );
}

void gap::removeEntry(int peerId)
{
	if ( neighborhoodTable.find(peerId) == neighborhoodTable.end() )
		return;
	neighborhoodTable.erase(peerId);
}

void gap::updateEntry(int peerId, unsigned long serial, int level, int parentId)
{
	if ( neighborhoodTable.find(peerId) == neighborhoodTable.end() )
	{
		neighborhoodTable[peerId].level = -1;
		neighborhoodTable[peerId].parentId = -1;
		neighborhoodTable[peerId].lastUpdate = 0.0;
		neighborhoodTable[peerId].lastSerial = -1;
		neighborhoodTable[peerId].status = unassigned;
		neighborhoodTable[peerId].queryId = -1;
		zeroAggregateData(peerId);
	}
	else
	{
		neighborhoodTable[peerId].level = level;
		neighborhoodTable[peerId].parentId = parentId;
		neighborhoodTable[peerId].lastUpdate = simTime();
		neighborhoodTable[peerId].lastSerial = serial;
	}
}

void gap::setQueryInfo(int querierId, int queryId, int repeat, int maxLevelEstimate, bool overwrite)
{
	queryInfo.timeStamp = simTime();
	if ( querierId==queryInfo.querierId && queryId==queryInfo.queryId && !overwrite )
		return;
	// Overwrite the current data
	queryInfo.querierId = querierId;
	queryInfo.queryId = queryId;
	queryInfo.counter = 0;
	queryInfo.lastUpdate = 0.0;
	queryInfo.maxLevelEstimate = maxLevelEstimate;
	diameterEstimate = maxLevelEstimate;
}

void gap::resetQueryInfo()
{
	queryInfo.querierId = -1;
	queryInfo.queryId = -1;
	queryInfo.counter = 0;
	queryInfo.lastUpdate = 0.0;
	queryInfo.maxLevelEstimate = 0;
	queryInfo.timeStamp = 0.0;
	disableSensor();
}

void gap::scheduleUpdate()
{
	if ( queryInfo.querierId == -1 || queryInfo.queryId == -1 )
	{
		return; // No query set up
	}

	AggregateData data;
	bool ready = computePartialAggregate(data);
	if ( ready )
	{
		update(data);
	}
	else
	{
		if ( sendUpdateTimeout==NULL )
		{
			sendUpdateTimeout = new mSendUpdateEvent();
			sendUpdateTimeout->setKind(GAP_EVENT_SEND_UPDATE);
		}
		if ( !sendUpdateTimeout->isScheduled() )
		{
			double delay = updateDelay;
			scheduleAt(simTime()+delay,sendUpdateTimeout);
		}
	}
}

void gap::update()
{
	AggregateData data;
	bool ready = computePartialAggregate(data);
	if ( !ready )
	{
		NeighborhoodTableType::iterator iter;
		for ( iter=neighborhoodTable.begin(); iter!=neighborhoodTable.end(); iter++)
		{
			if ((*iter).first==mac)
				continue;
			if ((*iter).second.status == unassigned )
				(*iter).second.status = peer; // Probably peer if not assigned at this point.
		}
	}
	update(data);
}

void gap::update(AggregateData &data)
{
	if ( sendUpdateTimeout!=NULL && sendUpdateTimeout->isScheduled() )
		cancelEvent(sendUpdateTimeout);

	broadcastUpdate(data);

	updateDiameterEstimate(data.maxLevel);
	if ( sendUpdateTimeout !=NULL && sendUpdateTimeout->isScheduled() )
		cancelEvent(sendUpdateTimeout);
}

void gap::storeRootAggregate()
{
	queryInfo.lastUpdate = simTime();
	AggregateData data;
	computePartialAggregate(data);
	updateDiameterEstimate(data.maxLevel);
	saveAggregate(data);
	setStatusText();
}

void gap::saveAggregate(AggregateData &data)
{
	if ( !recordingEnabled )
		return;

	static unsigned long updatecounter=0;

	//
	// Save a local snapshot
	//
	localData = data;

	//
	// Save to file
	//
	FILE *f = fopen(recordingFileName.c_str(),"a");
	if ( f==NULL )
	{
		ev << getFullPath()
		   << ": ERROR: Cannot open file '" << recordingFileName << "' for appending" << endl;
		return;
	}

	fprintf( f, "%ld\t%.6f\t%d\t%.6f\t%.6f\t%.6f\t%.6f\t%d\r\n",
			 updatecounter++, queryInfo.lastUpdate.dbl(),
			 data.count, data.sum, (data.sum/data.count),
			 data.min, data.max, data.maxLevel );
	fclose(f);
}

int gap::level()
{
	return level(mac);
}

int gap::level(int peerId)
{
	if ( neighborhoodTable.find(peerId) == neighborhoodTable.end() )
		return -1;
	else
		return neighborhoodTable[peerId].level;
}

int gap::parent()
{
	return parent(mac);
}

int gap::parent(int peerId)
{
	if ( neighborhoodTable.find(peerId) == neighborhoodTable.end() )
		return -1;
	else
		return neighborhoodTable[peerId].parentId;
}

bool gap::iamleaf()
{
	int childcount=0;
	int unknowncount=0;
	NeighborhoodTableType::iterator iter;
	for ( iter=neighborhoodTable.begin(); iter!=neighborhoodTable.end(); iter++)
	{
		if ( (*iter).first == mac )
			continue;
		if ( (*iter).second.parentId==-1 || (*iter).second.level==-1 )
			unknowncount++;
		if ( (*iter).second.status == child )
			childcount++;
	}
	return ( unknowncount==0 && childcount==0 );
}

AggregateData& gap::getAggregateData(int peerId)
{
	return neighborhoodTable[peerId].data;
}

void gap::setAggregateData(int peerId, AggregateData &data)
{
	neighborhoodTable[peerId].data.sum      = data.sum;
	neighborhoodTable[peerId].data.count    = data.count;
	neighborhoodTable[peerId].data.max      = data.max;
	neighborhoodTable[peerId].data.min      = data.min;
	neighborhoodTable[peerId].data.maxLevel = data.maxLevel;
	neighborhoodTable[peerId].lastAggregateUpdate = simTime();

	if ( iamroot )
	{
		storeRootAggregate(); //scheduleFinalizeAggregate();
		AggregateData data;
		zeroAggregateData(data);
		sendUpdate(peerId,data); // Echo empty update to peer
	}
	else
	{
		if ( neighborhoodTable[peerId].status == child )
			scheduleUpdate();
	}
}

void gap::setLocalAggregateData(double value)
{
	// TODO: The policy used here counts nodes. The latest value is stored.
	// Alternative policy is to increment the counter for each sensor value received and sum up contributions
	// in case sensor fires more than once per local epoch.
	neighborhoodTable[mac].data.sum = value;
	neighborhoodTable[mac].data.count = 1;
	if ( neighborhoodTable[mac].data.min == -1 || neighborhoodTable[mac].data.min > value )
		neighborhoodTable[mac].data.min = value;
	if ( neighborhoodTable[mac].data.max == -1 || neighborhoodTable[mac].data.max < value )
		neighborhoodTable[mac].data.max = value;
	neighborhoodTable[mac].data.maxLevel = neighborhoodTable[mac].level;
	neighborhoodTable[mac].lastAggregateUpdate = simTime();

	scheduleUpdate();
}

void gap::zeroAggregateData()
{
	NeighborhoodTableType::iterator iter;
	for(iter=neighborhoodTable.begin(); iter!=neighborhoodTable.end(); iter++)
	{
		zeroAggregateData((*iter).first);
	}
	neighborhoodTable[mac].lastAggregateUpdate = 0.0;
	neighborhoodTable[mac].queryId = -1;
}

void gap::zeroAggregateData(int peerId)
{
	zeroAggregateData(neighborhoodTable[peerId].data);
	neighborhoodTable[peerId].lastAggregateUpdate = 0.0;
	neighborhoodTable[peerId].queryId = -1;
}

void gap::zeroAggregateData(AggregateData &data)
{
	data.sum      = 0.0;
	data.count    = 0;
	data.max      = -1;
	data.min      = -1;
	data.maxLevel = -1;
}

void gap::appendAggregateData(AggregateData &src, AggregateData &dest)
{
	dest.sum += src.sum;
	dest.count += src.count;
	if ( src.max != -1 && ( dest.max==-1 || dest.max < src.max ) )
		dest.max = src.max;
	if ( src.max != -1 && ( dest.min==-1 || dest.min > src.min ) )
		dest.min = src.min;
	if ( src.maxLevel != -1 && ( dest.maxLevel==-1 || dest.maxLevel < src.maxLevel ) )
		dest.maxLevel = src.maxLevel;
}

bool gap::computePartialAggregate(AggregateData &data)
{
	int contributorCount=0;
	int currentContributorCount=0;

	NeighborhoodTableType::iterator iter;
	for( iter=neighborhoodTable.begin(); iter!=neighborhoodTable.end(); iter++)
	{
		if ( (*iter).second.status == self || (*iter).second.status == child )
		{
			appendAggregateData((*iter).second.data, data);
			contributorCount++;
			if ( (*iter).second.lastAggregateUpdate > queryInfo.lastUpdate )
				currentContributorCount++;
		}
	}

	int k=0; // TODO: use parameter here
	return ( (currentContributorCount+k) >= contributorCount );
}

bool gap::restoreTableInvariant()
{
	bool statusModified = false;

	int minLevel=getCurrentMinLevel();
	if ( minLevel != -1 )
		statusModified = setRelations(minLevel);

	setStatusText();

	return statusModified;
}

int gap::getCurrentMinLevel()
{
	if ( iamroot )
		return 0; // Root always has 0 as level

	// TODO: The min level should always be the current parents level
	// Return -1 if no parent is currently found.
	int minlevel=-1;
	NeighborhoodTableType::iterator iter;
	for ( iter=neighborhoodTable.begin(); iter!=neighborhoodTable.end(); iter++ )
	{
		if ( (*iter).first == mac )
			continue; // Skip myself
		if ( (*iter).second.level>=0 && ( minlevel == -1 || (*iter).second.level < minlevel ) )
			minlevel = (*iter).second.level;
	}
	return minlevel;
}

bool gap::setRelations(int parentLevel)
{
	if ( iamroot )
		return setRootRelations();
	else
		return setPopulationRelations(parentLevel);
}

bool gap::setRootRelations()
{
	bool updatePeers = false;
	int level;

	if ( neighborhoodTable[mac].level != 0 )
		error("Inconsistent neighborhood table for root. Level error.");
	if ( neighborhoodTable[mac].status != self )
		error("Inconsistent neighborhood table for root. SELF error.");

	// Set relations
	NeighborhoodTableType::iterator iter;
	for ( iter=neighborhoodTable.begin(); iter!=neighborhoodTable.end(); iter++ )
	{
		if ( (*iter).first==mac )
			continue;

		level = (*iter).second.level;

		if ( level != 1 )
		{
			(*iter).second.level = 1;
			updatePeers=true;
		}

		if ( (*iter).second.parentId == mac && (*iter).second.level==1 )
			(*iter).second.status = child;
		else
			(*iter).second.status = unassigned;
	}
	return false;
}

bool gap::setPopulationRelations(int parentLevel)
{
	int level;
	int selflevel = parentLevel+1;
	int childlevel = selflevel+1;
	bool parentFound=false;
	bool levelModified=false;
	bool parentModified=false;
	int curParent;
	NeighborhoodTableType::iterator iter;

	// Check self status
	if ( neighborhoodTable[mac].status != self )
		error("Inconsistent neighborhood table for root. SELF error.");

	curParent = parent();
	if ( curParent == -1 ) // Only assign new parent if the old one fails.
	{
		// Scan for new parent
		for ( iter=neighborhoodTable.begin(); iter!=neighborhoodTable.end(); iter++ )
		{
			if ( (*iter).first == mac )
				continue;

			level = (*iter).second.level;
			if ( level == parentLevel && !parentFound ) // Grab the first node with the lowest level
			{
				parentFound=true;
				(*iter).second.status = parentnode;
				neighborhoodTable[mac].parentId = (*iter).first;
				parentModified=true;
			}
		}
	}

	// Set my level according to the current minimum level
	if ( neighborhoodTable[mac].level != selflevel )
	{
		neighborhoodTable[mac].level = selflevel;
		levelModified = true;
	}

	// Scan table and set levels
	for ( iter=neighborhoodTable.begin(); iter!=neighborhoodTable.end(); iter++ )
	{
		if ( (*iter).first == mac )
			continue;
		if ( (*iter).second.status == parentnode )
			continue;

		(*iter).second.status = unassigned; // The default

		if ( (*iter).second.level == parentLevel )
		{
			(*iter).second.status = parentpeer;
		}
		else if ( (*iter).second.level == selflevel )
		{
			(*iter).second.level = peer;
		}
		else if ( (*iter).second.level > selflevel )
		{
			if ( (*iter).second.parentId == mac )
				(*iter).second.status = child;
			(*iter).second.level = childlevel;
		}

		if ( parentModified && !levelModified )
		{
			// Send update to the parent
			AggregateData data;
			computePartialAggregate(data);
			sendUpdate((*iter).first,data);
		}
		else if ( levelModified )
		{
			AggregateData data;
			computePartialAggregate(data);
			scheduleUpdate();
		}
	}

	return ( levelModified || parentModified );
}

int gap::getUnknownCount()
{
	int unknownNodes=0;
	NeighborhoodTableType::iterator iter;
	for ( iter=neighborhoodTable.begin(); iter!=neighborhoodTable.end(); iter++ )
	{
		if ( (*iter).first == mac )
			continue;
		if ( (*iter).second.status==unassigned)
			unknownNodes++;
	}
	return unknownNodes;
}

bool gap::updateReachable()
{
	if ( !newNeighbors() )
		return false;

	IntSet set;
	enumerateNeighbors(&set);
	bool modified=false;

	if ( set.size()==0 )
		logMessage("No neighbors");

	// Add new reachable nodes (potential peers) to the state table
	IntSet::iterator iter;
	for(iter=set.begin(); iter!=set.end(); iter++)
	{
		if( !intable((*iter)) )
		{
			logMessage("New reachable");
			updateEntry((*iter),0);
			modified=true;
			if ( cachedQuery!=NULL )
			{
				forwardQuery(cachedQuery);
				logMessage("Sending cached query");
			}
		}
	}

	// Handle unreachable peers
	int curid;
	std::vector<int> failedvector;
	NeighborhoodTableType::iterator tableiter;
	for(tableiter=neighborhoodTable.begin(); tableiter!=neighborhoodTable.end(); tableiter++)
	{
		curid = (*tableiter).first;

		if ( curid == mac )
			continue; // Leave self alone!
		if ( set.find(curid) == set.end() )
		{
			if ( (*tableiter).second.status == parentnode )  // Lost the parent
			{
				neighborhoodTable[mac].level = -1;
				neighborhoodTable[mac].parentId = -1;
			}
			failedvector.push_back(curid); // Schedule for delete
		}
	}

	std::vector<int>::iterator failed;
	for(failed=failedvector.begin(); failed!=failedvector.end(); failed++)
		removeEntry((*failed));
	failedvector.clear();

	return modified;
}

void gap::enableSensor()
{
	if ( gate("auxOut")->isConnected() )
	{
		logMessage("Enabling sensor");
		mSensorControl *ctrl = new mSensorControl();
		ctrl->setCode(enable);
		send(ctrl, "auxOut");
		sensorActive=true;
	}
}

void gap::disableSensor()
{
	sensorActive=false;
	logMessage("Disabling sensor");
	if ( gate("auxOut")->isConnected() )
	{
		mSensorControl *ctrl = new mSensorControl();
		ctrl->setCode(disable);
		send(ctrl, "auxOut");
	}
}

void gap::updateRttEstimate(double delay)
{
	rttEstimate = (1-rttAlpha)*rttEstimate + rttAlpha*2*delay;
}

void gap::updateDiameterEstimate(int curDiameter)
{
	if ( curDiameter <= 0 )
		return;
	if ( diameterEstimate > curDiameter )
		diameterEstimate = (1-diameterAlpha)*diameterEstimate + diameterAlpha*curDiameter;
	else
		diameterEstimate = curDiameter;
}

void gap::setStatusText()
{
	if ( !ev.isGUI() )
		return;

	static unsigned long updateCounter=0;
	std::stringstream str;
	str << nodeIdentifier << " (" << mac << "), l=" << level(mac) << ", p=" << parent(mac);
	if ( iamroot )
	{
		str << " -- ROOT" << endl;
		str << queryInfo.lastUpdate << updateCounter++ << ":" << localData.count << ", "
			<< localData.sum/localData.count << ", "
		    << localData.min << ", " << localData.max;
	}
	str << endl;

	#ifdef NEIGHBORHOODTABLE_IN_STATUS_TEXT
	NeighborhoodTableType::iterator iter;
	for ( iter=neighborhoodTable.begin(); iter!=neighborhoodTable.end(); iter++ )
	{
		str << (*iter).first << "," << (*iter).second.level << ","
			<< (*iter).second.parentId << "," << peerStatusToStr((*iter).second.status);
		if ( (*iter).first == nodeid )
			str << "*";
		if ( (*iter).second.status==parentnode)
			str << "#";
		str << endl;
	}
	#endif

	getParentModule()->getDisplayString().setTagArg("t",0,str.str().c_str());
}

std::string gap::peerStatusToStr(Status status)
{
	switch(status)
	{
	case unassigned: return "UNASSIGNED";
	case parentnode: return "PARENT";
	case parentpeer: return "PARENTPEER";
	case self:       return "SELF";
	case child:      return "CHILD";
	case peer:       return "PEER";
	default:         return "UNKNOWN";
	}
}

void gap::logMessage(std::string msg)
{
	if ( !loggingEnabled )
		return;

	FILE *f = fopen(logFileName.c_str(),"a");
	if ( f==NULL )
	{
		ev << getFullPath()
		   << ": ERROR: Cannot open file '" << logFileName << "' for appending" << endl;
		return;
	}

	fprintf( f, "%.6f\t%d\t%s\r\n", simTime().dbl(), mac, msg.c_str() );
	fclose(f);
}
