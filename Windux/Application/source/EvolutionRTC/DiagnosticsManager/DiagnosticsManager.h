/**
 * @file DiagnosticsManager.h
 * @brief Manager for diagnostic related events.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSMANAGER_H
#define DIAGNOSTICSMANAGER_H

#include <evolution.h>
#include <EvolMessage.h>

#include <CEvolMsgReceiver.h>

#include <EvolWrapperStatus.h>
#include <EvolWrapperSystemInfo.h>

#include <PrinterStatus.h>

#include <util/thread/mutex.h>

#include <list>

class DiagnosticsDB;
class DiagnosticsEdgeDevice;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Manager for diagnostic related events.
class DiagnosticsManager : public Hobart::CEvolMsgReceiver, Util::CRunnableClass
{
	typedef DiagnosticsManager _Myt;
	typedef Hobart::CEvolMsgReceiver _Mybase;

	typedef Util::CRunnableClass _Myimpl;

public:

	/// Diagnostic manager states.
	enum State
	{
		DiagStopped,								///< Not running.
		DiagStart,									///< Start state.
		DiagStarting,								///< Manager starting state.
		DiagRunning,								///< Running state.
		DiagStop,									///< Stop state.
		DiagStopping								///< Manager stopping state.
	};

	// ========================================================================
	// DTOR
	// ========================================================================

	/// Destructor.
	virtual ~DiagnosticsManager();

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve state.
	/**
	 * @return  state
	 */
	virtual State state() const;

	// ========================================================================
	// Methods
	// ========================================================================

	/// Process event.
	/**
	 * @param[in,out] msg  event
	 */
	virtual void processEvent( Util::CSmartPtr<EvolMessageWrapper> &msg );

	// ========================================================================
	// Static Methods
	// ========================================================================

	/// Terminate singleton instance.
	static void finalize();

	/// Retrieve singleton instance.
	/**
	 * @return  pointer to instance
	 */
	static _Myt *instance();

protected:

	/// Wizard states.
	enum WizardState
	{
		WizardIdle,									///< Not running.
		WizardInitializing,							///< Initializing.
		WizardReady,								///< Wizard ready to perform testing.
		WizardWrapperNeedPower,						///< Wrapper needs power to test.
		WizardTesting,								///< Test in progress.
		WizardWaitWrapperOnline						///< Waiting for wrapper online.
	};

	DiagnosticsDB *db_;
	DiagnosticsEdgeDevice *edge_;

	bool isScale_;
	bool isWrapper_;

	unsigned int wrapperCycleCounter_;
	unsigned int printer1Counts_;
	unsigned int printer2Counts_;
	unsigned int printer3Counts_;

	EvolWrapperStatus lastWrapperStatus_;
	EvolWrapperSystemInfo lastWrapperSystemInfo_;

	PrinterStatus lastPrinter1Status_;
	PrinterStatus lastPrinter2Status_;
	PrinterStatus lastPrinter3Status_;

	std::list<int> wrapperStatus_;
	std::list<int> wrapperErrorHistory_;

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve hardware information by device.
	/**
	 * @param[in] device  hardware device
	 * @param[out] model  model name
	 * @param[out] serialNumber  serial number
	 * @param[out] cycleCount  cycle count
	 */
	virtual void hardwareInfo( EvolDeviceType device, std::string &model, std::string &serialNumber, unsigned int &cycleCount ) const;

	/// Set state.
	/**
	 * @param[in] value  state
	 */
	virtual void setState( State value );

	/// Retrieve wizard state.
	/**
	 * @return  state
	 */
	virtual WizardState wizardState() const {return wizardState_;}

	// ========================================================================
	// Methods
	// ========================================================================

	/// Handle message.
	/**
	 * @param[in] msg  message
	 */
	template <class _Msg>
	void handleMessage( _Msg &msg );

	/// Handle message.
	/**
	 * @param[in] data  message data
	 */
	template <class T>
	void handleMessageT( const char *data );

private:

	typedef std::map<size_t, std::string> timingParametersMap;

	mutable util_thread::Mutex<> m_;

	Util::CSmartPtr<Util::CTLSThread> thread_;

	State state_;

	WizardState wizardState_;
	WizardState requestedWizardState_;

	std::string current_;
	int currentDevice_;
	int currentErrorCode_;

	timingParametersMap timingParameters_;

#ifdef USE_ACCOUNTANT
	static int dataType_;
	static std::string dataTypeName_;
#endif

	static std::string category_;

	static util_thread::Mutex<> instanceMutex_;
	static _Myt *instance_;

	// Constructor.
	DiagnosticsManager( const char *name = "DiagnosticsManager" );

	// Initialize.
	void initialize();

	// Set wizard state.
	void setWizardStateLabelApplier( WizardState value );

	// Set wizard state.
	void setWizardStatePrinter( WizardState value );

	// Set wizard state.
	void setWizardStateWeigher( WizardState value );

	// Set wizard state.
	void setWizardStateWrapper( WizardState value );

	// Set wizard state.
	void setWizardStateWrapperSystem( WizardState value );

	// Set wizard state.
	void setWizardState( const std::string &diagEvent, WizardState value );

	// Update wizard state.
	void updateWizardState();

	// Open database.
	bool openDatabase();

	// Restore database.
	bool restoreDatabase();

	// Upgrade database.
	bool upgradeDatabase();

	// Close database.
	void closeDatabase();

	// Delete database.
	void deleteDatabase();

	// Thread main loop.
	DWORD Run();

	// Check if should exit main loop.
	bool ExitThread( bool block = false );

	// not implemented
	DiagnosticsManager( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSMANAGER_H
