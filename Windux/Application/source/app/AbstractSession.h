/* LX Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ABSTRACTSESSION_H
#define ABSTRACTSESSION_H

#include "CEvolMsgReceiver.h"
#include "EvolMessage.h"
#include "SessionEvent.h"
#include "Messenger.h"
#include "HfcResponse.h"

#include "util/thread/mutex.h"

class CommMechanism;
class SessionMessage;


class AbstractSession : public Hobart::CEvolMsgReceiver, public Util::CRunnableClass
{
	public:

		static const short STATUS_UNKNOWN		= 0;
		static const short STATUS_ACTIVE		= 1;
		static const short STATUS_TIMEDOUT		= 2;
		static const short STATUS_TERMINATED	= 3;

		static const short SESSIONTYPE_MIN		= 0;
		static const short SESSIONTYPE_UNKNOWN	= 0;
		static const short SESSIONTYPE_PRIMARY	= 1;
		static const short SESSIONTYPE_REMOTE	= 2;
		static const short SESSIONTYPE_COMM		= 3;
		static const short SESSIONTYPE_INTRANET = 4;
		static const short SESSIONTYPE_SCALEMANAGEMENT = 5;
		static const short SESSIONTYPE_GCD		= 6;	//	GCDR-191
		static const short SESSIONTYPE_HELPER	= 7;	// Sessions created by SessionManager to do some task for it.
		static const short SESSIONTYPE_HELPER_PROCESSDOWNLOADFILES	= 8;	// Sessions created by SessionManager to process files found in the download directory.
		static const short SESSIONTYPE_HELPER_AUTOBACKUPDB	= 9;	// Session created by SessionManager to auto-backup the database to separate files, based on datatype grouping, to the 'backup' directory.
		static const short SESSIONTYPE_SMBACKEND = 10;				// Session created by SessionManager to listen and manage messages coming into the smBackend from anyone, not just HTe (ex: walmart remote print)
		static const short SESSIONTYPE_REMOTE_PRINT = 11;
		static const short SESSIONTYPE_HANDSFREEDEVICEMGR	= 12;
		static const short SESSIONTYPE_HELPER_SENDWRAPPERDATA = 13;
		static const short SESSIONTYPE_MAX			= 13;


		AbstractSession(CommMechanism *uiClient = 0, const char* name = 0, bool start = true, bool managedThread = false, bool superThread = false);

		virtual ~AbstractSession();

		
		void autoGenerateName();

		unsigned int getId() const;
		const HobString &getName() const;
		const HobString &getExtName() const;

		// Made virtual so BackupRestoreManager could override it.
		virtual short getSessionType() const;
		virtual void setSessionType(const short sessionType);


		bool isUnknownSession() const;
		bool isPrimarySession() const;
		bool isRemoteSession() const;
		bool isCommSession() const;
		bool isIntranetSession() const;
		bool isGCDSession() const;						//	GCDR-191
		bool isScaleManagementSession() const;			// used for messages from smbackend like print remote label
		bool isHelperSession() const;
		bool isHelperProcessDownloadFilesSession() const;
		bool isHelperBackupDBSession() const;
		bool isSmBackendSession() const;
		bool isRemotePrintSession() const;
		bool isHandsFreeDeviceMgrSession() const;

		short getStatus() const;
		void setStatus(const short status);

		/**
		 * This Messenger is used by other processes to send the
		 * instance of a Session a message,
		 *  using a message queue.
		 */
		Messenger &getMessenger();


		virtual int send(EvolMessage *message, int destination, bool shouldDelayAfterSending = true);

		// The following is used for syncing up the DOM's.
		virtual int send(const HobString &message, int destination, bool shouldDelayAfterSending = true);


		virtual void terminate(bool shouldSendUINotification = false);

		bool operator==(const AbstractSession &aSession) const;


		/* pauseProcessing is called so that by the time it returns the corresponding Session has
		temporarily stopped any processing (i.e., no more database access, input processing, etc.).
		Calling the other method, resumeProcessing, tells the corresponding session that it is now
		OK to continue processing events, etc. */
		virtual void pauseProcessing(int /*reason*/) {}
		virtual void resumeProcessing() {}



		/** Even though this functionality is only performed by a PrimaryConsoleSession,
		 * the SessionManager does not know what the session type is at the point a
		 * Session is instantiated; therefore, it won't be able to create an instance of a
		 * PrimaryConsoleSession class. This method will initially check to make sure the
		 * sessionType property is set for "primary console". */
		void addMonitoringSession(AbstractSession * session);
		void removeMonitoringSession(AbstractSession * session);

		
		bool unlockCritSectionForLongRunningProcess();
		void lockCritSectionAfterLongRunningProcess();

		/**
		 * This method should send a "registration" message to the primary console,
		 * registering for everything sent to and received from the primary console.
		 */
		static const short LEAVEMONITORMODE = 0;
		static const short ENTERMONITORMODE = 1;


	protected:
		Messenger myMessenger;
		Util::CSmartPtr<Util::CTLSThread>	sessionThread_;


		/** Used by the Primary Console Session to keep track of who is monitoring this scale.
		 */
		std::vector<AbstractSession *> monitoringSessions;

		bool isMonitoring(const AbstractSession *session) const;
		void sendToMonitoringSessions(EvolMessage *message, int destination);
		// The following is used for syncing up the DOM's.
		void sendToMonitoringSessions(const HobString &message, int destination);

		virtual void sessionMessageProcess( SessionMessage * msg );
		virtual void noMessageFromMessengerProcessing();
		virtual void initializationBeforeRun();
		virtual void finalizationAfterRun();
		virtual void quitGracefully();

		void RestartRunThread() { sessionThread_->RestartThread(); }
		DWORD Run();

		bool reinitDB_;
		util_thread::Mutex<> reinitializingDBMutex_;
		virtual void handleResultOfRTC(UINT32 /*result*/) {}
		
		//Only sleep in run function if this is set!!
		bool sleepInRunFunction;

	private:

		Util::CSmartCriticalSection sessionCS;
		bool sessionLocked;

		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper > & msg );


		/** The constructors need to call this to generate the task name for the new Session. */
		static unsigned int nextID;

		/**
		 * This is the unique identifier for the Session, automatically generated via
		 * the autoGenerateID() method.  This id is then used by the browser when
		 * requesting data (See SSUC_BrowserDataRequest Sequence diagram).
		 */
		unsigned int id;

		/** A user readable/comprehendable name for the session.
		 * This should be set up when the browser indicates what type
		 * of browser it is (primary, secondary, remote).
		 * The id property could be a part of the name as well. */
		HobString name;
		HobString extName;  // the name along with a string representation of sessionType

		/**
		 * The valid session types are:
		 * Unknown - a Session starts with this type before it is able to identify the connecting interface.
		 * Primary
		 * RemoteUI
		 * Communication
		 */
		short sessionType;

		int remoteRegFailed;

		/**
		 * The status, or state, of the Session.
		 * Refer to the static consts STATUS_...
		 */
		short status;

		void init(CommMechanism *uiClient);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //ABSTRACTSESSION_H
