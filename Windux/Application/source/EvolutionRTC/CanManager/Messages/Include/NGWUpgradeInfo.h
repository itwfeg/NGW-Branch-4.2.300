/**
 * @file NGWUpgradeInfo.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief NGW Wrapper Events
 * Created on: April 4, 2016
 *
 **/
#ifndef NGWUPGRADEINFO_H_
#define NGWUPGRADEINFO_H_
#include "EvolMessage.h"
#include "NGWEvent.h"
#include "Integer.h"

class NGWUpgradeInfo : public EvolMessage
{
public:

	typedef std::vector<UINT8> nodeIdList;
	typedef std::vector<std::string> nodeVersionList;

	NGWUpgradeInfo( Hobart::CEvolMsgReceiver *pHobThread = NULL );
	NGWUpgradeInfo( const char *buffer );
	~NGWUpgradeInfo();

	nodeIdList getNodeIds() const								{ return nodeIds; }
	void setNodeIds( const nodeIdList &ids )					{ nodeIds = ids; }
	
	nodeVersionList getNodeVersions() const						{ return nodeVersions; }
	void setNodeVersions( const nodeVersionList& vers ) 		{ nodeVersions = vers; }

	nodeVersionList getFirmwareVersions() const					{ return firmwareVersions; }
	void setFirmwareVersions( const nodeVersionList& vers ) 	{ firmwareVersions = vers; }
	
	bool isRequired() const										{ return required; }
	void setRequired( bool req )								{ required = req; }
	
	const char *serialize()const;

	void show();

protected:

	nodeIdList nodeIds;
	nodeVersionList nodeVersions;

	nodeVersionList firmwareVersions;

	bool required;

};

#endif 
