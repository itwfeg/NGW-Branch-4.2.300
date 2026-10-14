/**
 * @file Header.h
 * @brief Gateway comm structure header.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef EDGECOMMHEADER_H
#define EDGECOMMHEADER_H

#include <Util/NetworkVal.h>
#include <Util/Timestamp.h>

#include <stdint.h>

#include <list>
#include <map>
#include <string>

/// Edge Comms namespace.
namespace edgecomm
{

/// Field offset definition.
typedef std::pair<const void*, size_t> fieldOffsetDef;

/// Field offset list.
typedef std::list<fieldOffsetDef> offsetList;

/// Message types.
enum msgType
{
	POWER_UP_MESSAGE = 1,							///< Machine power up.
	STATUS_CHECK_MESSAGE = 2,						///< Status check.
	EVENT_MESSAGE = 3,								///< Event.
	HEALTH_INFO_MESSAGE = 4,						///< Health information.
	DIAGNOSTICS_MESSAGE = 5							///< Diagnostics.
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Gateway comm structure header.
struct header
{
	typedef header _Myt;

	uint8_t messageType_;							///< Message type.
	uint8_t messageVersion_;						///< Message version.
	uint32_t messageStamp_;							///< Message time stamp.

	char serialNumber_[16];							///< Machine serial number.
	char modelNumber_[16];							///< Machine model number.

	// ========================================================================
	// CTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] type  message type
	 * @param[in] version  message version
	 */
	header( uint8_t type, uint8_t version ) {
		const util::Timestamp stamp( util::Timestamp::now() );

		util::NetworkVal::setValueInt( messageType_, type );
		util::NetworkVal::setValueInt( messageVersion_, version );
		util::NetworkVal::setValueInt( messageStamp_, stamp.toUnixEpochTime() );

		util::NetworkVal::setValueString( serialNumber_, sizeof(serialNumber_), std::string() );
		util::NetworkVal::setValueString( modelNumber_, sizeof(modelNumber_),  std::string() );
	}

	/// Constructor.
	/**
	 * @param[in] type  message type
	 * @param[in] version  message version
	 * @param[in] serialNumber  serial number
	 * @param[in] modelNumber  model number
	 */
	header( uint8_t type, uint8_t version, const std::string &serialNumber, const std::string &modelNumber ) {
		const util::Timestamp stamp( util::Timestamp::now() );

		util::NetworkVal::setValueInt( messageType_, type );
		util::NetworkVal::setValueInt( messageVersion_, version );
		util::NetworkVal::setValueInt( messageStamp_, stamp.toUnixEpochTime() );

		util::NetworkVal::setValueString( serialNumber_, sizeof(serialNumber_), serialNumber );
		util::NetworkVal::setValueString( modelNumber_, sizeof(modelNumber_), modelNumber );
	}

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve message offsets.
	/**
	 * @param[in,out] list  list of offsets
	 */
	void offsets( offsetList &list ) const {
		list.push_back( offsetList::value_type( &messageType_, sizeof(messageType_) ) );
		list.push_back( offsetList::value_type( &messageVersion_, sizeof(messageVersion_) ) );
		list.push_back( offsetList::value_type( &messageStamp_, sizeof(messageStamp_) ) );
		list.push_back( offsetList::value_type( &serialNumber_, sizeof(serialNumber_) ) );
		list.push_back( offsetList::value_type( &modelNumber_, sizeof(modelNumber_) ) );
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // EDGECOMMHEADER_H
