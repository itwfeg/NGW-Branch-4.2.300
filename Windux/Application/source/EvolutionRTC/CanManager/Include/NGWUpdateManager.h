/**
 * @file NGWUpdateManager.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper firmware update manager
 * Created on: Dec 15, 2015
 *
 **/

#ifndef NGWUPDATEMANAGER_H_
#define NGWUPDATEMANAGER_H_

#include "CEvolMsgReceiver.h"
#include "CTLSThread.h"
#include "CHobartTimer.h"
#include "WrapperEvent.h"
#include "NGWEvent.h"

#include <map>
#include <mutex>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Class that Manages firmware updates of wrapper nodes
class NGWUpdateManager: public Hobart::CEvolMsgReceiver, public Util::CRunnableClass, public Hobart::CHobartTimer
{
public:

    enum Mode
    {
    	_Startup,
    	_Upgrading
    };

    // ========================================================================
    // DTOR
    // ========================================================================

    virtual ~NGWUpdateManager();

    // ========================================================================
    // Properties
    // ========================================================================

    virtual bool canStartUSB() const { return true; }

    virtual bool isManagerFinished()const  { return finished_; }

    virtual bool isWrapperDetected()const { return detected_; }

    // ========================================================================
    // Methods
    // ========================================================================

    /// Event handler for incoming messages
    /**
     * @param msg the message received
     */
    virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

    // ========================================================================
    // Static Methods
    // ========================================================================

    static NGWUpdateManager *getInstance();

    static void shutdown();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handler for Wrapper messages
    /**
     * @param wrEvent the type of wrapper event
     * @param recvBuffer char array containing the wrapper message
     */
    virtual void handleUpdaterEvent(WrEvent wrEvent, const char *recvBuffer);

    /// Handler for UI messages to update manager
    /**
     * @param NGWEvents the type of update event
     * @param recvBuffer char array containing the wrapper message
     */
    virtual void handleUIEvent(NGWEvents ngEvent, const char *recvBuffer);

    /// The timeout handler
    /**
     * @param timer_id the id of the timer that timed out
     */
    virtual void timeoutProcess( __int32 timer_id );

    /// check each device in device list if firmware update is required
    virtual DWORD Run();

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Reset the wrapper
    static void resetDevices();

private:

    std::string systemStyle;
    std::string scaleStyle;

    /// 56F83xx processor node
   	enum NGWWrapperDevices
   	{
   		_InvalidNode = 0,
   		_Node1 = 1,
   		_Node2,
   		_Node3,
   		_Node4,
   		_Node5,
   		_Node6
   	};

   	/// Update manager states
    enum UpdateStates
    {
    	_Init,
    	_RequestStatus,
    	_WaitForStatus,
    	_RequestVersion,
    	_WaitForVersion,
    	_CheckForUpgrade,
    	_StartNode,
    	_Exit,

    	_WaitForUI,

    	_StartUpgrade,
    	_NextNode,
    	_SendUpgradeCommand,
    	_WaitForReady,
    	_Upgrade,
    	_WaitForAck,
    	_EndNodeProg,
    	_WaitForVerify,
    	_ResetNode,
    	_ErrorNode,

    	_Cancel,
    };

    typedef std::vector<NGWWrapperDevices> DeviceList;

    typedef std::map<NGWWrapperDevices, std::string> DeviceListVersions;
    typedef std::map<NGWWrapperDevices, std::vector<unsigned char>> DeviceListFirmware;
    typedef std::map<NGWWrapperDevices, bool> DeviceListFaults;


    static std::mutex instanceMutex_;
   	static NGWUpdateManager *instance_;

	static const std::string upgradePathNGW_;
	static const std::string upgradePathNGS_;

	static const std::string wrapperDirectory_;

	static const int versionTime_ = 4 * 1000;			///< 4 seconds for node version time
	static const int responseTime_ = 10 * 1000;			///< 15 seconds for UI response time
	static const int upgradeTime_ =  5 * 60 * 1000;		///< 5 minute update time

    static const int numRetry = 3;
    static int retryCounter;

    std::mutex mutex_;

    UpdateStates state_;

    DeviceList nodes_;
    DeviceListVersions nodeVersions_;
    DeviceListFaults nodeFaults_;

    size_t currentNodeIndex_;
    size_t offset_;

    size_t retryFrameTimer_;

    DeviceListVersions firmwareVersions_;
    DeviceListFirmware firmwareImages_;

	bool forceUpgrade_;
	bool finished_;
	bool detected_;

	Util::CTLSThread *myRunThread_;

	/// Constructor.
   	NGWUpdateManager();

   	/// Retrieve current node.
   	NGWWrapperDevices currentNode() const;

   	/// Retrieve current node version.
   	std::string currentNodeVersion() const;

	/// handles updating wrapper node firmware image
	/**
	 * @param device to update firmware image
	 * @return int returns 0 if record is sent without error
	 */
	bool updateDevice(NGWWrapperDevices device, bool &done);

	/// checks for available firmware upgrade
	/**
	 * @param device to check for update
	 * @return True if update available and greater than current revision
	 */
	bool checkForUpdate(NGWWrapperDevices device);

	/// converts the firmware binary image image into CAN frames and sends
	/**
	 * @param recordSize is the number of bytes to be sent in multiple frames until the entire record is sent
	 */
	void processImageRecord(NGWWrapperDevices device, const unsigned char *record, size_t recordSize);

	/// converts the ascii value to decimal
	/**
	 * @param value is the ascii value to convert
	 */
	static UINT8 asciiToDec( UINT8 value );

	static void replaceAll( std::string &s, const std::string &from, const std::string &to );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
