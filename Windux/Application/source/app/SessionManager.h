/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H



#include <list>



#include "CEvolMsgReceiver.h"

#include "Weigher.h"
#include "Printer.h"
#include "evolution.h"
#include "ScaleSynchronizationType.h"

// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
#include "Weigher.h"
#include "directory.h"

class AbstractSession;
class Session;
class GCDSession;

namespace synchronization
{
	class SynchronizationManager;
	class SynchronizationManagerProxy;
}


class Socket;

class TimerManager;

class DBAccess;

class ChangeSynchronizationTypeMessage;

#include "ServerSocket.h"

#include "mutex.h"
#include "CSmartPtr.h"

typedef enum
{
	ShutdownRequest_TurnOff,
	ShutdownRequest_Reboot,
	ShutdownRequest_Cancel,
	ShutdownRequest_Force,
	ShutdownRequest_ConfigureOS,
	ShutdownRequest_ForceReboot,
	ShutdownRequest_ForceShutdown
} ShutdownRequestType;



class SessionManager : public Hobart::CEvolMsgReceiver, public Util::CRunnableClass
{
	friend class SessionStarter;

	public:
		/* --- Types and subtypes --- */
		typedef enum
		{
			RebootRequest_NotReceived,
			RebootRequest_PowerDown,
			RebootRequest_Reboot,
			RebootRequest_Logout
		} RebootRequest;


		void addDialog(int errorDevice, int errorCode, const std::string msgData);
		void removeDialog(int errorDevice, int errorCode);


		class SessionStarter : public Util::CRunnableClass 
		{
		public:
			SessionStarter(SessionManager* sessionMgr, Socket* socket);
			~SessionStarter();

			bool isDone() const { return done_; }

			void cleanupThread() { if (thread_) { thread_->StopThread(); thread_->WaitForCompletion(); } }

		private:
			Util::CSmartPtr<Util::CTLSThread> thread_;
			SessionManager* sessionMgr_;
			Socket* socket_;
			bool done_;

			DWORD Run();
			bool recordSeparatorPresent(const char * buffer, int* bufferLength) const;
		};
		typedef std::list< Util::CSmartPtr<SessionStarter> > startersContainer_type;
		typedef startersContainer_type::iterator startersContainer_iterator;


		/* --- Constructors and destructor --- */
		/**
		 * Create a serverSocket on the pre-configured wellKnownPort.  It is the job
		 * of the serverSocket to determine the wellKnownPort, which it can get from
		 * Configuration.
		 */
		SessionManager();
		~SessionManager();


		/* --- Getters and setters --- */
		bool isAllowConnections();
		void setAllowConnections(bool allow);

		bool isInputProcessingEnabled() const { return inputProcessingEnabled_; }
		void setInputProcessingEnabled() { inputProcessingEnabled_ = true; }
		void clearInputProcessingEnabled() { inputProcessingEnabled_ = false; }

		int getInputDisablingCondition() const { return inputDisablingCondition_; }
		void setInputDisablingCondition(int condition) { inputDisablingCondition_ = condition; }


		/* --- Instance function members --- */
		void init();

		static int activeFunction(void *pObject);
		int activeFunctionTask();

		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg ); 

		void verifyDeptBasedReservedRecords(DBAccess *dbAccess);

		bool isRestoringDB();

		/**
		 * Accept a connection on the Server socket by calling serverSocket.accept().
		 * When a connection is established, the serverSocket will return a Socket (clientSocket).
		 * Create a new Session for that Socket connection.
		 * Start that new Session in a separate Thread.
		 */
		void start();

		/**
		 * This method has to be synchronized so that no two threads try to add sessions at the same time.
		 * We don't want the internal integrity to be compromised.
		 * @stereotype synchronized
		 */
		void addSession(AbstractSession *aSession);

		/**
		 * This method should update (subtract 1) the consoleSessionCounts property for the
		 * corresponding sessionType.
		 * @stereotype synchronized
		 */
		void removeSession(AbstractSession *aSession);

		/**
		 * Look for the session in the sessions collection property
		 * with the sessionId passed in.
		 * If we find it, return a pointer to the Session.
		 * If we don't find it, return null.
		 */
		AbstractSession *findSession(const unsigned int sessionID);

		bool doesSessionExist(const short sessionType);

		/**
		 * Same as above, except  passing in a taskId.
		 */
		/*Session *findSessionByTaskId(const int taskID);*/

		/**
		 * This method should use the passed-in session's sessionType property
		 * to check if adding the session would exceed the maximum number of
		 * sessions allowed for that session type.
		 * If the Session Manager already has the maximum allowed, return false.
		 * If the limit has not been exceeded,
		 *    call addSession()
		 *    update (add 1) the consoleSessionCounts property for the
		 *  corresponding sessionType
		 *    return true.
		 * @stereotype synchronized
		 */
		bool registerSessionType(AbstractSession *aSession);

		Session *getPrimaryConsoleSession();

		short getRemoteConsoleSessionCount();

		synchronization::SynchronizationManagerProxy* getSyncManagerProxy() const;

		//ServerSocket *getServerSocket();

		static SessionManager *getInstance();

		bool isWrapperAttached();
		bool isEvolutionWrapperAttached();
		bool isCLAEPSAC2Attached();//4/24/14 Alex Zavala

#ifdef SUPPORT_ULTIMA_WRAPPERS
		bool isUltimaWrapperAttached();
#endif
		bool applyTareSetback();
		bool isWeigherAttached();
		bool isGCDAttached();
		bool isApcWeigherAttached();
		bool isKpcWeigherAttached();
		bool isEzUsbWeigherAttached();
		bool isAtmelWeigherAttached();

		// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
		WeigherKind getFirstWeigher();

		bool isCustomerDisplayAttached();		//	GCDR-158
		bool isGCDOperational();				//	GCDR-183

#if 0	//	Not used - CCD
		bool isWeigherAttached(WeigherType weigherNumber);
		bool isPrinterAttached();
		bool isLabelApplierAttached();
		bool isBakeryLabelApplierAttached();
		bool isCLAAttached();
		bool isILAAttached();
		bool isEvolLAAttached();
#else
		bool isPrinterAttached(PrinterType printerNumber = FirstPrinter);
		bool isPrinterStreamable(PrinterType whichPrinter);
#endif
		bool commSessionExists();
		bool remoteSessionExists();


		static const UINT32 COMMON_BLOCKED = 0x00000001;
		static const UINT32 SUPER_BLOCKED = 0x00000002;

		void registerThread(unsigned threadId, bool superThread);
		void deRegisterThread(unsigned threadId, bool superThread);
		bool managedThreadWouldBlock(bool superThread);
		UINT32 requestToContinue(unsigned threadId, bool superThread);
		void requestToPause(unsigned threadId, bool blockAll, bool superThread);
		void resumePausedThreads();
		bool changeBlockabilityStatus(unsigned threadId, bool outsideOfCS, bool superThread);
		void handleDeleteAll(EvolDeviceType recordType);

//		void testDumpRegistry();
//		void RunTestCommon();

		GCDSession* GetGCDSession() { return gcdSession; };
		void InitializeGCDSession();

		void panicShutdown();

		bool isInitialized() { return initialized_; }

		bool isDeleteAllFilesInProgress() const;


	private:

		typedef struct tagTHREADSTATUS
		{
			bool	safe;
			UINT32	status;
		}threadStatus, *pThreadStatus;

		static SessionManager *instance;

		Util::CSmartPtr<Util::CTLSThread>	sessionManagerThread;
		Util::CSmartCriticalSection  sessionManagerCS;		
		virtual DWORD Run();
		

		/**
		 * Maintain a pointer (reference) to the Session in the sessions collection
		 * that is the Primary Console Session (for faster access).
		 * Could loop through all sessions in the 'sessions' collection, checking the sessionType
		 * for each one, but maintaining this property would allow for finding the primary console
		 * faster.
		 * @supplierCardinality 0..*
		 * @undirected
		 * @clientCardinality 1
		 */
		Session *primaryConsoleSession;
		Session *remotePrintSession;

		/**
		 * How many remote consoles are currently connected (have Sessions instantiated).
		 *
		 *	Could determine this by counting the number of Sessions for the particular type
		 * in the instantiated Sessions maintained via the 'remoteSessions' Collection.
		 * But it may be quicker to just maintain a separate property (of course this will
		 * have to be updated when adding or deleting each Session).
		 */
		/** A collection of all remote Sessions that have been instantiated. */
		//HobCollection remoteSessions;
		//RWTPtrSlist<Session> remoteSessions;
		std::list<AbstractSession *> remoteSessions;

		/**
		 * This socket allows user interfaces (browsers, scale management applications)
		 * to establish a connection with the scale.
		 */
		ServerSocket serverSocket;


		TimerManager *timerManager;

		bool sentFirmwareUpdateMessage;

		bool sentGCDMediaUpdateMessage;

		bool dbIndDataExists;

		bool timeToDoDBCompress;
		void performDBCompress();
		void performDBBackup();
		void checkDailyTimers();
		void performSynchronizationRequest();
		void performAutoBackupDB();
		void checkForDownloadedFiles();
		void lookForDownloadedFiles(std::string dirPathStr, std::string fileSpecifiersStr);
        int downloadingFilesTimerId_;
		util_filesystem::Directory::DirectoryFileList availableFiles;
		util_filesystem::Directory::DirectoryFileList filesInProcess;
		util_filesystem::Directory::DirectoryFileList filesInMergeProcess;
		void prepareToProcessDownloadedFiles();
		void processDownloadedFiles();
		void checkForFilesToMerge();
		void checkForFilesToMergeAfterComm();

		unsigned int configDebugFileChecksum_;

		bool allowConnections;

		int shutdownStep;
		RebootRequest rebootRequest_;

		void handleLogoffRequest();
		void handleShutdownRequest();
		void handleRebootMessage(RebootRequest request);


		void displayAllSessions();

		bool requestToCompressDB_;

		bool requestToPerformAutoBackupDB_;

		bool requestToInitializeSynchronization_;
		bool requestToTerminateSynchronization_;
		bool requestToChangeSynchronizationType_;
		ScaleSynchronizationType requestedSyncType_;
		std::string requestedSyncTypePrimaryScale_;
		bool requestToChangePrimaryScale_;
		std::string requestedPrimaryScale_;

		/*used for firmware upgrade via ftp on bootup*/
		int bootupUpgradeCounter_;
		
		/* used to detect if running board site */
		static const std::string boardSiteLvdsFile_;

		/* --- Primary/Secondary feature --- */
		ScaleOnlineType scaleOnline_;
		bool inputProcessingEnabled_;
		int inputDisablingCondition_;

		startersContainer_type starters_;
		util_thread::Mutex<> startersMutex_;
		void addSessionStarter(SessionStarter* starter);
		void removeSessionStarter(SessionStarter* starter);

		/* "Use Case 1.1: Booting up" */
		int usecaseBootingUp();
		/* "Use Case 1.2: Starting up Synchronization Manager" */
		synchronization::SynchronizationManager* synchronizationManager_;
		int usecaseStartingupSynchronizationManager(ScaleSynchronizationType scaleType, bool register = false,
				const std::string & primaryScale = "");
		/* "Use Case 1.3: Coming up on-line" */
		int usecaseComingUpOnLine();
		/* "Use Case 1.5: Going off-line" */
		int usecaseGoingOffLine();
		/* "Use Case 1.6: Stopping Synchronization Manager" */
		void processShutdownSynchronization();
		int usecaseStoppingSynchronizationManager();
		/* "Use Case 1.8: Enabling input processing" */
		int usecaseEnablingInputProcessing();
		/* "Use Case 1.9: Disabling input processing" */
		int usecaseDisablingInputProcessing(int condition);
		/* "Use Case 1.10: Changing Synchronization Type" */
		int usecaseChangingSynchronizationType(ScaleSynchronizationType scaleType, const std::string & primaryScale);
		/* "Use Case 1.11: Changing Primary Scale" */
		int usecaseChangingPrimaryScale(const std::string & primaryScale);

		const std::string& getStatusText(int status) const;

#if defined( WIN32 )
		void CheckIfDaylightSavingsSwitched();
#endif
		LONG queuedPausers_;
		std::map< unsigned, threadStatus > commonThreads_;
		std::map< unsigned, threadStatus > superThreads_;
		typedef std::map<unsigned, threadStatus>::iterator ManagedThreadMapIterator;
		Util::CMSGEvent blockCommonThreadsEvent_;
		Util::CMSGEvent blockSuperThreadsEvent_;
		Util::CMSGEvent blockAllRequestsToPauseEvent_;
		Util::CMSGEvent blockPauseInitiatorAllEvent_;
		Util::CMSGEvent blockPauseInitiatorCommonEvent_;
		Util::CSmartCriticalSection managedThreadListCS_;

		void updatePauseInitiatorEvents();
		bool areAllCommonThreadsSafe();
		bool areAllSuperThreadsSafe();
		void markManagedThreadsStatus(UINT32 status);
		UINT32 getLastBlockedStatus(unsigned threadId, bool superThread);

		int lastHourExpiredLabelsChecked;

		bool usingEmptyDatabase;
		bool usingMirrorDatabase;

		bool checkDatabaseIndices(/*edb::DBManager *myDatabase = 0*/);
		bool checkDatabaseIntegrity(bool reportAllStatus = false);
		bool handleCorruptDatabase();

		bool openTheDatabase(/*edb::DBManager *myDatabase = 0*/);
		bool closeTheDatabase(/*DBManager *myDatabase*/);
		bool validateDatabaseDirectories(std::string &cfDbLoc, std::string &cfEmptyDbLoc, std::string &cfMirrorDbLoc, std::string &cfCorruptDbLoc);
		bool copyDatabaseFiles(const std::string cfDestinationDbLoc, const std::string cfSourceDbLoc);


		/* --- Class data members --- */
		static const int _statusOK_                                 =  0;
		static const int _statusUnknownSynchronizationType_         = -1;
		static const int _statusNoSynchronizationManager_           = -2;
		static const int _statusCannotChangePrimaryIfNotSecondary_  = -3;
		static const int _statusCouldNotInitializeSynchronizationManager_ = -4;

		static const std::string _statusOKText_;
		static const std::string _statusUnknownSynchronizationTypeText_;
		static const std::string _statusNoSynchronizationManagerText_;
		static const std::string _statusCannotChangePrimaryIfNotSecondaryText_;
		static const std::string _statusUnknownText_;
		static const std::string _statusCouldNotInitializeSynchronizationManagerText_;

		// GCD Session
		GCDSession *gcdSession;

		//holds daylight saving status -> retrieved from C:\Program Files\Hobart\Access Scale\DST.txt 
		int ctz_stat_on_file;
		
		//used when obtaining daylight savings status to compare against saved status (located in ctz_stat_on_file)
		int ctz_stat;

		bool createGCDSession;	//	GCDR-195: Start the GCD Session after we get a Primary Session
		void startGCDSession();

		bool panicShutdown_;
		bool initiateShutdown_;

		bool virtualServiceKeyPresent_;

		void monitorMemory();

		bool initialized_;

		bool deleteAllFilesInProgress_;

		bool needToSeeWrStatus_;

		void sendWrapperReset();//Added 4/24/14 Alex Zavala
		void sendLabelApplierReset();//Added 4/24/14 Alex Zavala

		void checkMetrologySealableParameters();
		void checkMetrologySealableParametersLots();
};

#endif //SESSIONMANAGER_H




