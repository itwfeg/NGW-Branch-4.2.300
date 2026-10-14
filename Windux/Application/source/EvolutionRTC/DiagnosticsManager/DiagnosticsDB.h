/**
 * @file DiagnosticsDB.h
 * @brief Database class for diagnostic events.
 *
 * @copyright Copyright (c) 2016-2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSDB_H
#define DIAGNOSTICSDB_H

namespace util {
	class DictionaryList;
}

#include <list>
#include <map>
#include <string>
#include <vector>

struct sqlite3;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Manager for diagnostic related events.
class DiagnosticsDB
{
	typedef DiagnosticsDB _Myt;

public:

	/// Diagnostics test result codes.
	enum diagStepTestCode
	{
		Pass = 0,									///< Test passed.
		Fail,										///< Test failed.
		FailTimeout									///< Failed due to timeout.
	};

	/// Resolution reasons.
	enum maintLogResolutionReason
	{
		Ignored = 0,								///< Ignored maintenance item.
		ResolvedByService,							///< Resolved maintenance item.
		ResolvedByDiag								///< Resolved by diagnostics over time.
	};

	/// Wrapper timings parameter.
	typedef std::pair<unsigned int, unsigned int> timingsParam;

	/// Wrapper timings parameter list.
	typedef std::list<timingsParam> timingsParamList;

	/// Wrapper timings for height box.
	typedef std::map<unsigned int, timingsParamList> timingsMap;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	DiagnosticsDB( const std::string &filename );

	/// Destructor.
	virtual ~DiagnosticsDB();

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve edge device timer values.
	/**
	 * @param[out] init  initialization time (seconds)
	 * @param[out] powerUp  power up time (seconds)
	 * @param[out] statusCheck  status check frequency (seconds)
	 */
	virtual void edgeDeviceTimers( unsigned int &init, unsigned int &powerUp, unsigned int &statusCheck ) const;

	/// Retrieve housekeeping days.
	/**
	 * @return  housekeeping (days)
	 */
	virtual unsigned int housekeepingDays() const;

	/// Check if database is latest version.
	/**
	 * @return  @c true if latest version, @c false otherwise
	 */
	virtual bool isLatestVersion() const;

	/// Check if database is open.
	/**
	 * @return  @c true if open, @c false otherwise
	 */
	virtual bool isOpen() const;

	/// Retrieve version.
	/**
	 * @return  version
	 */
	virtual unsigned int version() const {return version_;}

	// ========================================================================
	// Methods
	// ========================================================================

	/// Create event.
	/**
	 * @param[in] device  device
	 * @param[in] deviceExt  device extended
	 * @param[in] deviceCycles  device cycles/counts
	 * @param[in] errorCode  error code
	 * @param[in] forceDiagnostics  force diagnostics for this event
	 * @return  @c true if event created, @c false otherwise
	 */
	virtual bool createEvent( int device, int deviceExt, unsigned int deviceCycles, int errorCode, bool forceDiagnostics = false );

	/// Lookup event.
	/**
	 * @param[in] device  device
	 * @param[in] deviceExt  device extended
	 * @param[in] errorCode  error code
	 * @param[out] event  event code
	 * @param[out] diag  diag event code
	 * @param[out] isWizardAvail  @c true if diag wizard available, @c false otherwise
	 * @param[out] count  count of duplicate events
	 * @return  @c true if diag event found, @c false otherwise
	 */
	virtual bool lookupEvent( int device, int deviceExt, int errorCode, std::string &event, std::string &diag, bool &isWizardAvail, int &count );

	/// Lookup event max count.
	/**
	 * @param[in] device  device
	 * @param[in] deviceExt  device extended
	 * @param[in] errorCode  error code
	 * @return  max count
	 */
	virtual unsigned int lookupEventMaxCount( int device, int deviceExt, int errorCode );

	/// Lookup event severity.
	/**
	 * @param[in] device  device
	 * @param[in] deviceExt  device extended
	 * @param[in] errorCode  error code
	 * @return  severity
	 */
	virtual unsigned int lookupEventSeverity( int device, int deviceExt, int errorCode );

	/// Lookup device information.
	/**
	 * @param[in] event  event code
	 * @param[out] device  device
	 * @param[out] deviceExt  device extended
	 * @param[out] errorCode  errorCode
	 * @return  @c true if diag event found, @c false otherwise
	 */
	virtual bool lookupDeviceInfo( const std::string &event, int &device, int &deviceExt, int &errorCode );

	/// Lookup device information.
	/**
	 * @param[in] diagStep  diag step event code
	 * @param[out] device  device
	 * @param[out] deviceExt  device extended
	 * @param[out] errorCode  errorCode
	 * @return  @c true if diag event found, @c false otherwise
	 */
	virtual bool lookupDeviceInfoFromDiagStep( const std::string &diagStep, int &device, int &deviceExt, int &errorCode );

	/// Update hardware info.
	/**
	 * @param[in] diag  diag event identifier
	 * @param[in] hardwareModel  hardware model
	 * @param[in] hardwareSerial  hardware serial number
	 * @param[in] hardwareCycles  hardware cycles
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool updateHardwareInfo( const std::string &diag, const std::string &hardwareModel, const std::string &hardwareSerial, unsigned int hardwareCycles );

	/// Lookup hardware info.
	/**
	 * @param[in] diag  diag event identifier
	 * @param[out] hardwareModel  hardware model
	 * @param[out] hardwareSerial  hardware serial number
	 * @param[out] hardwareCycles  hardware cycles
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool lookupHardwareInfo( const std::string &diag, std::string &hardwareModel, std::string &hardwareSerial, unsigned int &hardwareCycles );

	/// Create diagnostics wizard.
	/**
	 * @param[in] event  event identifier
	 * @param[in] method  method of entry
	 * @param[out] diag  diagnostics wizard event
	 * @param[out] diagStep  diagnostics wizard step event
	 * @param[out] diagErrorCode  diagnostics wizard error code
	 * @return  @c true if diag started, @c false otherwise
	 */
	virtual bool createDiagWizard( const std::string &event, int method, std::string &diag, std::string &diagStep, int &diagErrorCode );

	/// Create diagnostics wizard from maint record.
	/**
	 * @param[in] maintRecord  maint record
	 * @param[in] method  method of entry
	 * @param[out] event  event identifier
	 * @param[out] diag  diagnostics wizard event
	 * @param[out] diagStep  diagnostics wizard step event
	 * @param[out] diagErrorCode  diagnostics wizard error code
	 * @return  @c true if diag started, @c false otherwise
	 */
	virtual bool createDiagWizardFromMaintRecord( const std::string &maintRecord, int method, std::string &event, std::string &diag, std::string &diagStep, int &diagErrorCode );

	/// Retrieve diagnostics method.
	/**
	 * @param[in] diagStep  diag step event code
	 * @param[out] method  methd
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool lookupDiagWizardMethod( const std::string &diagStep, int &method );

	/// Start diagnostics wizard event test.
	/**
	 * @param[in] diagStep  diag step event code
	 * @return  @c true if diag event started, @c false otherwise
	 */
	virtual bool startDiagWizardEventTest( const std::string &diagStep );

	/// Finish diagnostics wizard event test.
	/**
	 * @param[in] diagStep  diag step event code
	 * @param[in] testCode  diag step testing code
	 * @param[in] result  diag result code
	 * @param[in] routine  diagnostic routine executed
	 * @param[in] routineExpectedResult  expected result from routine
	 * @param[in] routineActualResult  actual result from routine
	 * @return  @c true if diag event finished, @c false otherwise
	 */
	virtual bool finishDiagWizardEventTest( const std::string &diagStep, diagStepTestCode testCode, int result, int routine = 0, int routineExpectedResult = 0, int routineActualResult = 0 );

	/// Retrieve service action records.
	/**
	 * @param[in] errorCode  error code
	 * @param[out] results  list of results
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool retrieveActionRecords( int errorCode, util::DictionaryList &results );

	/// Retrieve maintenance log records.
	/**
	 * @param[in] filter  filtering options
	 * @param[in] age  record age (in days, zero for all)
	 * @param[out] results  list of results
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool retrieveMaintLogRecords( int filter, int age, util::DictionaryList &results );

	/// Clear a maintenance record.
	/**
	 * @param[in] maintRecord  record to clear
	 * @param[in] reason  resolution reason
	 * @param[in] serviceActionCount  number of actions
	 * @param[in] serviceMethods  service methods
	 * @param[in] serviceParts  service parts
	 * @param[in] serviceNotes  service notes
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool clearMaintLogRecord( const std::string &maintRecord, maintLogResolutionReason reason, int serviceActionCount = 0, int *serviceMethods = NULL, int *serviceParts = NULL, const std::string &serviceNotes = std::string() );

	/// Generate maintenance log report suitable for printing.
	/**
	 * @param[in] filter  filtering options
	 * @param[in] age  record age (in days, zero for all)
	 * @param[out] report  report to print
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool generateMaintLogReport( int filter, int age, std::string &report );

	/// Resolve maintenance log records by running cycles.
	/**
	 * @param[in] deviceCycles  current cycle count
	 * @param[out] resolved  number of records resolved
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool resolveMaintLog( unsigned int deviceCycles, int &resolved );

	/// Begin update of wrapper boxes.
	/**
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool updateWrapperBoxBegin();

	/// Update wrapper box.
	/**
	 * @param[in] name  tray name
	 * @param[in] plu  plu exception
	 * @param[in] lengthMin  min length
	 * @param[in] lengthMax  max length
	 * @param[in] widthMin  min width
	 * @param[in] widthMax  max width
	 * @param[in] lip  lip height
	 * @param[in] timings  timings data
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool updateWrapperBox( const std::string &name, unsigned int plu, unsigned int lengthMin, unsigned int lengthMax, unsigned int widthMin, unsigned int widthMax, unsigned int lip, const timingsMap &timings );

	/// End update of wrapper boxes.
	/**
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool updateWrapperBoxEnd();

	/// Clear all wrapper box timings data.
	virtual bool clearAllWrapperBoxes();

	/// Update film status.
	/**
	 * @param[in] upperFilmWidth  upper film width
	 * @param[in] lowerFilmWidth  lower film width
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool updateFilmStatus( unsigned int upperFilmWidth, unsigned int lowerFilmWidth );

	/// Retrieve audit records.
	/**
	 * @param[in] filter  filtering options
	 * @param[in] age  record age (in days, zero for all)
	 * @param[out] results  list of results
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool retrieveAuditLog( int filter, int age, util::DictionaryList &results );

	/// Retrieve audit log record.
	/**
	 * @param[in] record  record
	 * @param[out] severity  event severity
	 * @param[out] source  event source
	 * @param[out] sourceExt  event source extended
	 * @param[out] desc  event description
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool retrieveAuditRecord( const std::string &record, int &severity, int &source, int &sourceExt, std::string &description );

	/// Create service visit record in the audit log.
	/**
	 * @param[in] start  @c true if visit is starting
	 * @param[in] stop  @c true if visit is ending
	 * @param[in] description  visit description
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool createServiceVisit( bool start, bool stop, const std::string &description = std::string() );

	/// Create service visit record in the audit log.
	/**
	 * @return  @c true if in service visit, @c false otherwise
	 */
	virtual bool checkServiceVisit();

	/// Create service visit record in the audit log.
	/**
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool createUnlockTimings();

	/// Create head status event.
	/**
	 * @param[in] printerNum  printer number
	 * @param[in] cycles  cycles/counts of printer
	 * @param[in] status  printer head status
	 * @return  @c true if event created, @c false otherwise
	 */
	virtual bool createHeadStatusEvent( int printerNum, unsigned int cycles, int status );

	/// Update printer dot wear.
	/**
	 * @param[in] printerNum  printer number
	 * @param[in] status  dot status for each dot
	 * @param[in] value  dot resistance value for each dot
	 * @return  @c true if event created, @c false otherwise
	 */
	virtual bool updateDotWear( int printerNum, const std::vector<int> &status, const std::vector<int> &value );

	/// Update printer dot usage.
	/**
	 * @param[in] printerNum  printer number
	 * @param[in] printed  dots printed
	 * @param[in] removed  dots removed from label
	 * @return  @c true if event created, @c false otherwise
	 */
	virtual bool updateDotUsage( int printerNum, const std::vector<int> &printed, const std::vector<int> &removed );

	/// Create Value Max product record.
	/**
	 * @param[in] transNum  transaction number
	 * @param[in] productNum  product number
	 * @param[in] unitPrice  item unit price
	 * @param[in] quantity  quantity sold
	 * @param[in] netWeight  net weight of product sold
	 * @param[in] tare  tare of product sold
	 * @param[in] totalPrice  item total price
	 * @param[in] counts  zeroed weigher counts
	 * @param[in] scaleFactor  scale factor
	 * @param[in] tiltX  x axis tilt when product sold
	 * @param[in] tiltY  x axis tilt when product sold
	 * @param[in] adjNetWeight  adjusted net weight for scale out of level
	 * @param[in] adjTotalPrice  adjusted total price for scale out of level
	 * @param[in] savings  savings gained from value max enabled
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool createValueMax( int transNum, int productNum, double unitPrice, int quantity, double netWeight, double tare, double totalPrice, int counts, int scaleFactor, double tiltX, double tiltY, double adjNetWeight, double adjTotalPrice, double savings );

	/// Retrieve total savings generated with Value Max.
	/**
	 * @param[out] savings  savings gained from value max enabled
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool retrieveValueMaxSavings( double& savings );

	/// Convert old database to new new format.
	/**
	 * @param[in,out] db  db to convert
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool upgrade( _Myt &db );

	/// Perform housekeeping.
	/**
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool housekeeping();

	// ========================================================================
	// Static Properties
	// ========================================================================

	/// Retrieve database filename configuration.
	/**
	 * @return  filename
	 */
	static std::string dbFile();

	/// Retrieve database backup filename configuration.
	/**
	 * @return  filename
	 */
	static std::string dbBackupFile();

	/// Retrieve database old (before upgrade) filename configuration.
	/**
	 * @return  filename
	 */
	static std::string dbOldFile();

protected:

	sqlite3 *db_;									///< Database.

	std::string filename_;							///< Database filename.
	unsigned int version_;							///< Database version.

private:

#ifdef USE_ACCOUNTANT
	static int dataType_;
	static std::string dataTypeName_;
#endif

	static std::string category_;

	/// Initialize.
	void initialize();

	/// Finalize.
	void finalize();

	/// Create database from scratch.
	bool createDatabase();

	/// Update version info from config.
	void updateVersionInfo();

	/// Abort unfinished event steps.
	void abortUnfinishedEventSteps();

	// not implemented
	DiagnosticsDB();

	// not implemented
	DiagnosticsDB( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSMANAGER_H
