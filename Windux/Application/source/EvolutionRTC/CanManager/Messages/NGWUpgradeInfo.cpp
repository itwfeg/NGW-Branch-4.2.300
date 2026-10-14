/**
 * @file NGWUpgradeInfo.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI to the state of the NGWUpgradeManager
 * Created on: April 4, 2016
 */
#include "NGWUpgradeInfo.h"
#include "Debug.h"
#include "Integer.h"

NGWUpgradeInfo::NGWUpgradeInfo( Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( NGWrapper, _UpgradeInfo, pHobThread )
{
	required = false;
}

NGWUpgradeInfo::~NGWUpgradeInfo()
{

}

NGWUpgradeInfo::NGWUpgradeInfo( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	size_t nodes;

	char *pChar = (char *) &nodes;
	for (unsigned int i = 0; i < sizeof(nodes); i++)
		*pChar++ = buffer[j++];

	while ( nodes-- )
	{
		nodeIdList::value_type nodeId;
		nodeVersionList::value_type nodeVersion;

		nodeVersionList::value_type firmwareVersion;

		pChar = (char *) &nodeId;
		for (unsigned int i = 0; i < sizeof(nodeId); i++)
			*pChar++ = buffer[j++];

		while ( buffer[j] )
			nodeVersion.push_back( buffer[j++] );
		j++;

		while ( buffer[j] )
			firmwareVersion.push_back( buffer[j++] );
		j++;

		nodeIds.push_back( nodeId );
		nodeVersions.push_back( nodeVersion );

		firmwareVersions.push_back( firmwareVersion );
	}

	pChar = (char *) &required;
	for (unsigned int i = 0; i < sizeof(required); i++)
		*pChar++ = buffer[j++];

}

const char *NGWUpgradeInfo::serialize()const
{
	EvolMessage::serialize();

	size_t nodes( 9999 );

	if ( nodeIds.size() < nodes )
		nodes = nodeIds.size();
	if ( nodeVersions.size() < nodes )
		nodes = nodeVersions.size();
	if ( firmwareVersions.size() < nodes )
		nodes = firmwareVersions.size();

	const char *pChar = (char *) &nodes;
	for (unsigned int i = 0; i < sizeof(nodes); i++)
		serializedData.append(1, pChar[i]);

	for ( size_t n( 0 ); n < nodes; ++n )
	{
		const nodeIdList::value_type nodeId( nodeIds[n] );
		const nodeVersionList::value_type nodeVersion( nodeVersions[n] );

		const nodeVersionList::value_type firmwareVersion( firmwareVersions[n] );

		pChar = (char *) &nodeId;
		for (unsigned int i = 0; i < sizeof(nodeId); i++)
			serializedData.append(1, pChar[i]);

		for (unsigned int i = 0; i < nodeVersion.size(); i++)
			serializedData.append(1, nodeVersion[i]);
		serializedData.append(1, NULL);

		for (unsigned int i = 0; i < firmwareVersion.size(); i++)
			serializedData.append(1, firmwareVersion[i]);
		serializedData.append(1, NULL);
	}

	pChar = (char *) &required;
	for (unsigned int i = 0; i < sizeof(required); i++)
		serializedData.append(1, pChar[i]);

	return serializedData.data();
}

void NGWUpgradeInfo::show()
{
	Debug::trace( "NGWUpgradeInfo", "nodeIds: " );
	for ( const auto& nodeId : nodeIds )
		Debug::trace( "NGWUpgradeInfo", "  " + Integer::toString(nodeId) );
	Debug::trace( "NGWUpgradeInfo", "nodeVersions: " );
	for ( const auto& nodeVersion : nodeVersions )
		Debug::trace( "NGWUpgradeInfo", "  " + nodeVersion );
	for ( const auto& firmwareVersion : firmwareVersions )
		Debug::trace( "NGWUpgradeInfo", "  " + firmwareVersion );
	Debug::trace( "NGWUpgradeInfo", "required: " + Integer::toString(required) );
}



