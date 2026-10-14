/**
 * @file DiagnosticsEdgeDevice.h
 * @brief Interface to gateway edge device.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSEDGEDEVICE_H
#define DIAGNOSTICSEDGEDEVICE_H

#include "Util/BasicSocket.h"
#include "Util/Dict.h"

#include <util/thread/mutex.h>

#include <list>
#include <string>
#include <vector>

namespace edgecomm {
	struct powerUpMsg_v1_firmware;
	struct powerUpMsg_v1_healthInfo;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Interface to gateway edge device.
class DiagnosticsEdgeDevice
{
	typedef DiagnosticsEdgeDevice _Myt;

public:

	/// Dictionary type.
	typedef util::DictionaryList dict_type;

	/// Diagnostics test result codes.
	enum diagResult
	{
		Pass = 0,									///< Test passed.
		Fail,										///< Test failed.
		FailTimeout									///< Failed due to timeout.
	};

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	DiagnosticsEdgeDevice();

	/// Destructor.
	virtual ~DiagnosticsEdgeDevice();

	// ========================================================================
	// Properties
	// ========================================================================

	/// Check if edge device enabled.
	/**
	 * @return  @c true if enabled, @c false otherwise
	 */
	virtual bool enabled() const;

	/// Retrieve offline message spool size.
	/**
	 * @return  count of messages in offline spool
	 */
	virtual unsigned int offlineMessages() const;

	/// Check if edge device online.
	/**
	 * @param[out] gateway  gateway device is connected to
	 * @param[out] discovered  @c true if gateway was discovered, @c false otherwise
	 * @return  @c true if enabled, @c false otherwise
	 */
	virtual bool online( std::string &gateway, bool &discovered ) const;

	/// Set if edge device enabled.
	/**
	 * @param[in] value  @c true for enabled, @c false otherwise
	 */
	virtual void setEnabled( bool value ) {enabled_ = value;}

	// GENERAL MACHINE

	/// Set health information.
	/**
	 * @param[in] value  health values
	 */
	virtual void setHealthInfo( const dict_type &value );

	/// Set model.
	/**
	 * @param[in] value  model
	 */
	virtual void setModel( const std::string &value ) {model_ = value;}

	/// Set serial.
	/**
	 * @param[in] value  serial
	 */
	virtual void setSerial( const std::string &value ) {serial_ = value;}

	// PRINTER

	/// Set printer hardware version.
	/**
	 * @param[in] index  hardware type
	 * @param[in] value  version
	 */
	virtual void setPrinterHardwareVersion( size_t index, const std::string &value ) {printerHardwareVersion_[index] = value;}

	/// Set printer software version.
	/**
	 * @param[in] index  software type
	 * @param[in] value  version
	 */
	virtual void setPrinterSoftwareVersion( size_t index, const std::string &value ) {printerSoftwareVersion_[index] = value;}

	/// Set printer checksum.
	/**
	 * @param[in] index  checksum type
	 * @param[in] value  checksum
	 */
	virtual void setPrinterChecksum( size_t index, const std::string &value ) {printerChecksum_[index] = value;}

	/// Set printer counts.
	/**
	 * @param[in] index  checksum type
	 * @param[in] value  count
	 */
	virtual void setPrinterCounts( size_t index, unsigned int value ) {printerCounts_[index] = value;}

	// WEIGHER

	/// Set weigher hardware version.
	/**
	 * @param[in] index  hardware type
	 * @param[in] value  version
	 */
	virtual void setWeigherHardwareVersion( const std::string &value ) {weigherHardwareVersion_ = value;}

	/// Set weigher software version.
	/**
	 * @param[in] index  software type
	 * @param[in] value  version
	 */
	virtual void setWeigherSoftwareVersion( const std::string &value ) {weigherSoftwareVersion_ = value;}

	/// Set weigher checksum.
	/**
	 * @param[in] index  checksum type
	 * @param[in] value  checksum
	 */
	virtual void setWeigherChecksum( const std::string &value ) {weigherChecksum_ = value;}

	// WRAPPER

	/// Set wrapper belt state.
    /**
     * @param[in] enabled  @c true if belt powered, @c false otherwise
     * @param[in] tempSetting  temp setting
     * @param[in] tempActual  actual temp
     */
	virtual void setWrapperBeltState( bool enabled, int tempSetting, int tempActual ) {
		wrapperBeltEnabled_ = enabled;
		wrapperBeltTempActual_ = tempSetting;
		wrapperBeltTempSetting_ = tempActual;
	}

	/// Set wrapper compressor state.
	/**
	 * @param[in] unreg  unregulated pressure
	 * @param[in] reg  regulated pressure
	 * @param[in] runTime  compressor run time
	 */
	virtual void setWrapperCompressorState( unsigned int unreg, unsigned int reg, unsigned int runTime ) {
		wrapperCompressorLine_ = unreg;
		wrapperCompressorRegulated_ = reg;
		wrapperCompressorRunTime_ = runTime;
	}

	/// Set wrapper cycle count.
	/**
	 * @param[in] value  cycle count
	 */
	virtual void setWrapperCycleCount( unsigned int value ) {
		wrapperCycleCount_ = value;
	}

	/// Set wrapper film state.
	/**
	 * @param[in] upperWidth  film upper width
	 * @param[in] upperUsage  film usage
	 * @param[in] lowerWidth  film lower width
	 * @param[in] lowerUsage  film usage
	 */
	virtual void setWrapperFilmState( unsigned int upperWidth, unsigned int upperUsage, unsigned int lowerWidth, unsigned int lowerUsage ) {
		wrapperUpperFilmWidth_ = upperWidth;
		wrapperUpperFilmUsage_ = upperUsage;
		wrapperLowerFilmWidth_ = lowerWidth;
		wrapperLowerFilmUsage_ = lowerUsage;
	}

	/// Set wrapper info.
    /**
     * @param[in] model  wrapper model
     * @param[in] serial  wrapper serial
     */
	virtual void setWrapperInfo( const std::string &model, const std::string &serial ) {
		wrapperModel_ = model;
		wrapperSerial_ = serial;
	}

	/// Set wrapper node versions.
	/**
	 * @param[in] app  list of app versions
	 * @param[in] data  list of data versions
	 * @param[in] boot  list of boot versions
	 * @param[in] checksum  list of checksum versions
	 */
	virtual void setWrapperNodeVersions( std::vector<std::string> &app, std::vector<std::string> &data, std::vector<std::string> &boot, std::vector<std::string> &checksum ) {
		wrapperNodeAppVersions_ = app;
		wrapperNodeDataVersions_ = data;
		wrapperNodeBootVersions_ = boot;
		wrapperNodeChecksums_ = checksum;
	}

	/// Set wrapper vacuum state.
    /**
     * @param[in] unreg  unregulated pressure
     * @param[in] runTime  vacuum run time
     */
	virtual void setWrapperVacuumState( unsigned int unreg, unsigned int runTime ) {
		wrapperVacuumLine_ = unreg;
		wrapperVacuumRunTime_ = runTime;
	}

	// ========================================================================
	// Methods
	// ========================================================================

	/// Check communications.
	virtual void checkComms();

	/// Check online to gateway.
	virtual void checkOnline();

	/// Send diagnostics message.
	/**
	 * @param[in] code  code diagnosed
	 * @param[in] result  result of diagnosis
	 * @param[in] errorCode  error code
	 * @param[in] routine  routine executed
	 * @param[in] routineExpectedResult  expected result
	 * @param[in] routineActualResult  actual result
	 * @param[in] execOper  @c true if executed by operator
	 * @param[in] execService  @c true if executed by service
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool sendDiagnostics( unsigned int code, diagResult result, int errorCode, int routine, int routineExpectedResult, int routineActualResult, bool execOper, bool execService );

	/// Send event message.
	/**
	 * @param[in] code  status/error
	 * @param[in] severity  severity level
	 * @param[in] diag  @c true if diagnostics available
	 * @param[in] diagForced  @c true if diagnostics forced
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool sendEvent( unsigned int code, unsigned int severity, bool diag, bool diagForced );

	/// Send power up message.
	/**
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool sendPowerUp();

	/// Send status check message.
	/**
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool sendStatusCheck();

protected:

	// ========================================================================
	// Methods
	// ========================================================================

	/// Send message.
	/**
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool sendMsg( const void *b, size_t bs );

private:

	static const unsigned short EDGE_DISCOVERY_PORT = 5090;
	static const unsigned short EDGE_CLIENT_PORT = 5080;

	static const unsigned int EDGE_SERIAL_DEVICE_BAUD_RATE = 115200;
	static const unsigned char EDGE_SERIAL_DEVICE_PARITY = 0;

	mutable util_thread::Mutex<> m_;

	mutable std::string address_;
	mutable unsigned short port_;
	mutable unsigned int logLevel_;

	std::string discoveredAddress_;
	unsigned short discoveredPort_;

	util::BasicSocket *discoverySocket_;
	util::BasicSocket *clientSocket_;

	int clientSerialPort_;

	bool isScale_;
	bool isWrapper_;

	bool enabled_;
	bool powerUpComplete_;

	unsigned long powerUpTime_;

	std::list<std::string> messages_;

	std::string discoveryBuffer_;
	std::string clientBuffer_;

	// GENERAL MACHINE
	dict_type healthInfo_;
	std::string model_;
	std::string serial_;

	// PRINTER
	std::string printerSoftwareVersion_[3];
	std::string printerHardwareVersion_[3];
	std::string printerChecksum_[3];
	unsigned int printerCounts_[3];

	// WEIGHER
	std::string weigherSoftwareVersion_;
	std::string weigherHardwareVersion_;
	std::string weigherChecksum_;

	// WRAPPER
	bool wrapperBeltEnabled_;
	int wrapperBeltTempActual_;
	int wrapperBeltTempSetting_;
	unsigned int wrapperCompressorLine_;
	unsigned int wrapperCompressorRegulated_;
	unsigned int wrapperCompressorRunTime_;
	unsigned int wrapperCycleCount_;
	unsigned int wrapperLowerFilmUsage_;
	unsigned int wrapperLowerFilmWidth_;
	std::string wrapperModel_;
	std::vector<std::string> wrapperNodeAppVersions_;
	std::vector<std::string> wrapperNodeBootVersions_;
	std::vector<std::string> wrapperNodeChecksums_;
	std::vector<std::string> wrapperNodeDataVersions_;
	std::string wrapperSerial_;
	unsigned int wrapperUpperFilmUsage_;
	unsigned int wrapperUpperFilmWidth_;
	unsigned int wrapperVacuumLine_;
	unsigned int wrapperVacuumRunTime_;

#ifdef USE_ACCOUNTANT
	static int dataType_;
	static std::string dataTypeName_;
#endif

	static std::string category_;

	/// Initialize.
	void initialize();

	/// Finalize.
	void finalize();

	/// Process communications.
	void processComms();

	/// Retrieve machine cycle count.
	unsigned int machineCycleCount() const;

	/// Replace all occurrences of string to another string.
	static void replaceAll( std::string &s, const std::string &from, const std::string &to );

	/// Parse sealer on/off times into buffer.
	static void parseBeltTimes( const std::string &on, const std::string &off, unsigned char *buffer );

	/// Append firmware block.
	static void appendFirmware_v1( edgecomm::powerUpMsg_v1_firmware *p, size_t &pi, int type, const std::string version );

	/// Append health info block.
	static void appendHealthInfo_v1( edgecomm::powerUpMsg_v1_healthInfo *p, size_t &pi, unsigned int errorCode, unsigned int date, unsigned int count, unsigned int lastOccurrence, unsigned int severity );

    /// Set interface attributes.
    static bool setInterfaceAttrib( int fd, unsigned int baudRate = 9600, unsigned char parity = 0 );

    /// Set blocking.
    static bool setBlocking( int fd, bool shouldBlock );

	/// Check for existance of edge serial device.
	static bool serialDeviceExists();

	// not implemented
	DiagnosticsEdgeDevice( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSEDGEDEVICE_H
