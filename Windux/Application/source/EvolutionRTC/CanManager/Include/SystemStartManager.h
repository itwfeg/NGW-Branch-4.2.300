/*
 *
 * @file SystemStartManager.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper firmware update manager
 * Created on: Oct 4, 2016
 *
 */

#ifndef SYSTEMSTARTMANAGER_H_
#define SYSTEMSTARTMANAGER_H_

#include "CEvolMsgReceiver.h"
#include "CTLSThread.h"

#include <mutex>

class NGWUpdateManager;
class KPCBootManager;
class KPCBootUpgrader;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// System startup manager.
class SystemStartManager : public Hobart::CEvolMsgReceiver, public Util::CRunnableClass
{

public:

    // ========================================================================
    // DTOR
    // ========================================================================

	virtual ~SystemStartManager();

    // ========================================================================
    // Properties
    // ========================================================================

    virtual bool isFinished() const {return finished_;}

    // ========================================================================
    // Methods
    // ========================================================================

	virtual bool setModeToScan( bool coldStart );

    // ========================================================================
    // Static Methods
    // ========================================================================

	static SystemStartManager *getInstance();
    static void shutdown();

protected:

    /// Event handler for incoming messages
    /**
     * @param msg the message received
     */
    virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

    /**
     * Start the CAN bus manager and the upgrade manager and wait until
     * upgrade manager is finished before closing down the CAN interface.
     */
    virtual DWORD Run();

private:

    static std::mutex instanceMutex_;
    static SystemStartManager *instance_;

	enum StartModes
	{
		_Idle,
		_Scan,
		_StartupKpc,
		_KPCBootloader,
        _KPCBootUpgrader
	};

    enum ScanModeStates
    {
    	Start_,
    	WaitForFinish_,
    	End_
    };

    StartModes mode_;
    ScanModeStates state_;

    bool coldStart_;
    bool finished_;

    NGWUpdateManager *pNGWUpdateManager_;
    KPCBootManager *kBootManager_;
    KPCBootUpgrader *kBootUpgrader_;

    std::mutex mutex_;

    std::string scaleStyle_;

	Util::CTLSThread *myRunThread;

	SystemStartManager();

	void handleScan();
	void handleKpcStartup();
	void handleKpcBootloader();
    void handleKpcBootUpgrader();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
