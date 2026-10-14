#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H



#include "HFCManagerContainer.h"
#include "IEvolMsgHandler.h"

#include "CEvolMsgManager.h"


class EvolMessage;



namespace synchronization
{


	class QueueManager;
	class Command;
	class ExecuteCommandMessage;


	class Synchronizer: public hfc::HFCManagerContainer, public Hobart::IEvolMsgHandler
	{
		public:
			/* --- Types and subtypes --- */
			static const int _statusOK_                    =   0;
			static const int _statusNothingToDo_           =   0;
			static const int _statusStoppingSynchronizer_  =  -1;
			static const int _statusCommunicationFailed_   =  -2;
			static const int _statusNotFound_              =  -3;
			static const int _statusStateMachineFailed_    =  -4;
			static const int _statusFailedToPause_         = - 5;

			static const int _statusDBException_           = -10;

			static const int _statusTargetScaleNotFound_   = -20;

			static const int _statusBatchNotFound_         = -30;


			/* --- Constructors and destructor --- */
			Synchronizer(Hobart::CEvolMsgReceiver* syncManager, const char* name = "Synchronizer");
			virtual ~Synchronizer();


			/* --- Getter and setter methods --- */
			Hobart::CEvolMsgManager* getMessageReceiver() { return msgManager_.get(); }

			bool isPausingProcessing() const { return pausingProcessing_; }


			/* --- Instance Function members --- */
			/* Use cases */
			/* "Use Case 5.51: Stopping synchronizer". */
			int usecaseStoppingSynchronizer();
			/* "Use Case 5.58: Resuming processing". */
			virtual int usecaseResumingProcessing() = 0;
			/* "Use Case 5.59: Pausing processing". */
			virtual int usecasePausingProcessing(int reason) = 0;

			virtual void waitUntilRunThreadEnds() = 0;


		protected:
			/* --- Instance Data members --- */
			// The following member is not a const pointer because we could not dispatch
			// an event to it if it were that way. For any other purpose it is as if it
			// were a const pointer.
			Hobart::CEvolMsgReceiver* qMsgManager_;
			Util::CSmartPtr<Hobart::CEvolMsgManager> msgManager_;
			bool stoppingSynchronizer_;
			bool pausingProcessing_;
			util_thread::Mutex<> reinitializingDBMutex_;
			bool reinitDB_;

			/* --- Instance function members --- */
			const std::string& getStatusText(int status) const;
			void restoreDB(UINT32 result);

		private:
			/* --- Constructors and destructor --- */
			// Copy constructor is not available
			Synchronizer(const Synchronizer& sm);


			/* --- Function members --- */
			/* Operators */
			// Operator asignment is not available
			Synchronizer& operator = (const Synchronizer& rhs);


			/* --- Class data members --- */
	#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
	#endif

			static const std::string _statusOKText_;
			static const std::string _statusNothingToDoText_;
			static const std::string _statusStoppingSynchronizerText_;
			static const std::string _statusCommunicationFailedText_;
			static const std::string _statusNotFoundText_;
			static const std::string _statusStateMachineFailedText_;
			static const std::string _statusFailedToPauseText_;

			static const std::string _statusDBExceptionText_;

			static const std::string _statusTargetScaleNotFoundText_;

			static const std::string _statusBatchNotFoundText_;

			static const std::string _statusUnknownText_;
	};


}



#endif /* SYNCHRONIZER_H */
