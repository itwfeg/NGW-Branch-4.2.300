/**
 * @file PowerUpMsg.h
 * @brief Power up message for gateway communication.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef EDGECOMMPOWERUPMSG_H
#define EDGECOMMPOWERUPMSG_H

#include "Header.h"

#include <cstring>

/// Edge Comms namespace.
namespace edgecomm
{

/// Peripheral status flags.
class powerUpMsg
{
public:

	/// Message flags.
	enum peripheralStatusFlags
	{
		wrapperOnline = 0x01,
		weigherOnline = 0x02,
		printerOneOnline = 0x04,
		printerTwoOnline = 0x08,
		printerThreeOnline = 0x10
	};

	/// Firmware types.
	enum firmwareType
	{
		APPLICATION_VERSION = 0,
		OS_VERSION,
		LOADER_VERSION,
		SM_BACKEND_VERSION,
		SYSTEM_CONTROLLER_VERSION,
		WRAPPER_NODE1_APP_VERSION,
		WRAPPER_NODE2_APP_VERSION,
		WRAPPER_NODE3_APP_VERSION,
		WRAPPER_NODE4_APP_VERSION,
		WRAPPER_NODE5_APP_VERSION,
		WRAPPER_NODE1_DATA_VERSION,
		WRAPPER_NODE2_DATA_VERSION,
		WRAPPER_NODE3_DATA_VERSION,
		WRAPPER_NODE4_DATA_VERSION,
		WRAPPER_NODE5_DATA_VERSION,
		WRAPPER_NODE1_BOOT_VERSION,
		WRAPPER_NODE2_BOOT_VERSION,
		WRAPPER_NODE3_BOOT_VERSION,
		WRAPPER_NODE4_BOOT_VERSION,
		WRAPPER_NODE5_BOOT_VERSION,
		WRAPPER_NODE1_CHECKSUM,
		WRAPPER_NODE2_CHECKSUM,
		WRAPPER_NODE3_CHECKSUM,
		WRAPPER_NODE4_CHECKSUM,
		WRAPPER_NODE5_CHECKSUM,
		WEIGHER_SOFTWARE_VERSION,
		WEIGHER_HARDWARE_VERSION,
		WEIGHER_CHECKSUM,
		PRINTER1_SOFTWARE_VERSION,
		PRINTER1_HARDWARE_VERSION,
		PRINTER1_CHECKSUM,
		PRINTER2_SOFTWARE_VERSION,
		PRINTER2_HARDWARE_VERSION,
		PRINTER2_CHECKSUM,
		PRINTER3_SOFTWARE_VERSION,
		PRINTER3_HARDWARE_VERSION,
		PRINTER3_CHECKSUM,
		FPC_VERSION
	};
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Power up message.
struct powerUpMsg_v1 : public header
{
	typedef powerUpMsg_v1 _Myt;
	typedef header _Mybase;

	uint32_t manufactureDate_;
	uint32_t installDate_;

	uint16_t firmwareInfoOffset_;
	uint16_t firmwareInfoCount_;

	char machineAddress_[16];
	char machineHostname_[16];

	uint16_t healthInfoOffset_;
	uint16_t healthInfoCount_;

	uint32_t cycleCount_;
	uint32_t powerOnOffCount_;
	uint32_t networkOfflineCount_;

	uint16_t peripheralStatusFlags_;

	uint16_t wrapperInfoOffset_;
	uint16_t wrapperInfoLength_;

	uint32_t powerUpTime_;

	// ========================================================================
	// CTOR
	// ========================================================================

	/// Constructor.
	powerUpMsg_v1() :
		_Mybase( POWER_UP_MESSAGE, 1 ) {initialize();}

	/// Constructor.
	/**
	 * @param[in] serialNumber  serial number
	 * @param[in] modelNumber  model number
	 */
	powerUpMsg_v1( const std::string &serialNumber, const std::string &modelNumber ) :
		_Mybase( POWER_UP_MESSAGE, 1, serialNumber, modelNumber ) {initialize();}

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve message offsets.
	/**
	 * @param[in,out] list  list of offsets
	 */
	void offsets( offsetList &list ) const {
		_Mybase::offsets( list );
		list.push_back( offsetList::value_type( &manufactureDate_, sizeof(manufactureDate_) ) );
		list.push_back( offsetList::value_type( &installDate_, sizeof(installDate_) ) );
		list.push_back( offsetList::value_type( &firmwareInfoOffset_, sizeof(firmwareInfoOffset_) ) );
		list.push_back( offsetList::value_type( &firmwareInfoCount_, sizeof(firmwareInfoCount_) ) );
		list.push_back( offsetList::value_type( &machineAddress_, sizeof(machineAddress_) ) );
		list.push_back( offsetList::value_type( &machineHostname_, sizeof(machineHostname_) ) );
		list.push_back( offsetList::value_type( &healthInfoOffset_, sizeof(healthInfoOffset_) ) );
		list.push_back( offsetList::value_type( &healthInfoCount_, sizeof(healthInfoCount_) ) );
		list.push_back( offsetList::value_type( &cycleCount_, sizeof(cycleCount_) ) );
		list.push_back( offsetList::value_type( &powerOnOffCount_, sizeof(powerOnOffCount_) ) );
		list.push_back( offsetList::value_type( &networkOfflineCount_, sizeof(networkOfflineCount_) ) );
		list.push_back( offsetList::value_type( &peripheralStatusFlags_, sizeof(peripheralStatusFlags_) ) );
		list.push_back( offsetList::value_type( &wrapperInfoOffset_, sizeof(wrapperInfoOffset_) ) );
		list.push_back( offsetList::value_type( &wrapperInfoLength_, sizeof(wrapperInfoLength_) ) );
		list.push_back( offsetList::value_type( &powerUpTime_, sizeof(powerUpTime_) ) );
	}

private:

	void initialize() {
		memset( &((char*) this)[sizeof(_Mybase)], 0, sizeof(_Myt) - sizeof(_Mybase) );
	}

};

/// Firmware version.
struct powerUpMsg_v1_firmware
{
	typedef powerUpMsg_v1_firmware _Myt;

	uint16_t firmwareType_;
	char firmwareVersion_[14];

	// ========================================================================
	// CTOR
	// ========================================================================

	/// Constructor.
	powerUpMsg_v1_firmware() {
		memset( this, 0, sizeof(_Myt) );
	}

	/// Constructor.
	/**
	 * @param[in] type  firmware type
	 * @param[in] version  version
	 */
	powerUpMsg_v1_firmware( uint16_t type, const std::string &version ) {
		util::NetworkVal::setValueInt( firmwareType_, type );
		util::NetworkVal::setValueString( firmwareVersion_, sizeof(firmwareVersion_), version );
	}

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve message offsets.
	/**
	 * @param[in,out] list  list of offsets
	 */
	void offsets( offsetList &list ) const {
		list.push_back( offsetList::value_type( &firmwareType_, sizeof(firmwareType_) ) );
		list.push_back( offsetList::value_type( &firmwareVersion_, sizeof(firmwareVersion_) ) );
	}

};

/// Health info.
struct powerUpMsg_v1_healthInfo
{
	typedef powerUpMsg_v1_healthInfo _Myt;

	uint32_t errorCode_;
	uint32_t date_;
	uint16_t numberOccurrences_;
	uint32_t lastOccurrenceDate_;
	uint16_t severity_;

	// ========================================================================
	// CTOR
	// ========================================================================

	/// Constructor.
	powerUpMsg_v1_healthInfo() {
		memset( this, 0, sizeof(_Myt) );
	}

	/// Constructor.
	/**
	 * @param[in] errorCode  error code
	 * @param[in] date  date
	 * @param[in] numberOccurrences  number occurrences
	 * @param[in] lastOccurrenceDate  last occurrence date
	 * @param[in] severity  severity
	 */
	powerUpMsg_v1_healthInfo( uint32_t errorCode, const util::Timestamp &date, uint16_t numberOccurrences, const util::Timestamp &lastOccurrenceDate, uint16_t severity ) {
		util::NetworkVal::setValueInt( errorCode_, errorCode );
		util::NetworkVal::setValueInt( date_, date.toUnixEpochTime() );
		util::NetworkVal::setValueInt( numberOccurrences_, numberOccurrences );
		util::NetworkVal::setValueInt( lastOccurrenceDate_, lastOccurrenceDate.toUnixEpochTime() );
		util::NetworkVal::setValueInt( severity_, severity );
	}

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve message offsets.
	/**
	 * @param[in,out] list  list of offsets
	 */
	void offsets( offsetList &list ) const {
		list.push_back( offsetList::value_type( &errorCode_, sizeof(errorCode_) ) );
		list.push_back( offsetList::value_type( &date_, sizeof(date_) ) );
		list.push_back( offsetList::value_type( &numberOccurrences_, sizeof(numberOccurrences_) ) );
		list.push_back( offsetList::value_type( &lastOccurrenceDate_, sizeof(lastOccurrenceDate_) ) );
		list.push_back( offsetList::value_type( &severity_, sizeof(severity_) ) );
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Wrapper Info.
struct powerUpMsgWrapperInfo_v1
{
	typedef powerUpMsgWrapperInfo_v1 _Myt;

	uint16_t blockVersion_;

	char wrapperSerialNumber_[16];
	char wrapperModelNumber_[16];

	uint16_t upperFilmWidth_;
	uint16_t lowerFilmWidth_;
	uint16_t upperFilmUsage_;
	uint16_t lowerFilmUsage_;

	uint16_t sealerBeltState_;
	uint16_t sealerBeltTempExpected_;
	uint16_t sealerBeltTempActual_;
	uint8_t sealerBeltOnOff_[7][4];

	uint16_t compressorLine_;
	uint16_t compressorRegulated_;
	uint32_t compressorRunTime_;

	uint16_t vacuumPumpLine_;
	uint32_t vacuumPumpRunTime_;

	// ========================================================================
	// CTOR
	// ========================================================================

	/// Constructor.
	powerUpMsgWrapperInfo_v1() {
		memset( this, 0, sizeof(_Myt) );
		util::NetworkVal::setValueInt( blockVersion_, 1 );
	}

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve message offsets.
	/**
	 * @param[in,out] list  list of offsets
	 */
	void offsets( offsetList &list ) const {
		list.push_back( offsetList::value_type( &blockVersion_, sizeof(blockVersion_) ) );
		list.push_back( offsetList::value_type( &wrapperSerialNumber_, sizeof(wrapperSerialNumber_) ) );
		list.push_back( offsetList::value_type( &wrapperModelNumber_, sizeof(wrapperModelNumber_) ) );
		list.push_back( offsetList::value_type( &upperFilmWidth_, sizeof(upperFilmWidth_) ) );
		list.push_back( offsetList::value_type( &lowerFilmWidth_, sizeof(lowerFilmWidth_) ) );
		list.push_back( offsetList::value_type( &upperFilmUsage_, sizeof(upperFilmUsage_) ) );
		list.push_back( offsetList::value_type( &lowerFilmUsage_, sizeof(lowerFilmUsage_) ) );
		list.push_back( offsetList::value_type( &sealerBeltState_, sizeof(sealerBeltState_) ) );
		list.push_back( offsetList::value_type( &sealerBeltTempExpected_, sizeof(sealerBeltTempExpected_) ) );
		list.push_back( offsetList::value_type( &sealerBeltTempActual_, sizeof(sealerBeltTempActual_) ) );
		list.push_back( offsetList::value_type( &sealerBeltOnOff_, sizeof(sealerBeltOnOff_) ) );
		list.push_back( offsetList::value_type( &compressorLine_, sizeof(compressorLine_) ) );
		list.push_back( offsetList::value_type( &compressorRegulated_, sizeof(compressorRegulated_) ) );
		list.push_back( offsetList::value_type( &compressorRunTime_, sizeof(compressorRunTime_) ) );
		list.push_back( offsetList::value_type( &vacuumPumpLine_, sizeof(vacuumPumpLine_) ) );
		list.push_back( offsetList::value_type( &vacuumPumpRunTime_, sizeof(vacuumPumpRunTime_) ) );
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // EDGECOMMPOWERUPMSG_H
