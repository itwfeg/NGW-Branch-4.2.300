#include "Synchronizer.h"

#include "QueueManager.h"

#include "util/thread/mutex.h"
#include "util/time/dbtime.h"

#include "SessionManager.h"

#include "DBException.h"


#include "Messages/CompleteBatchProcessedMessage.h"
#include "Messages/ExecuteCommandMessage.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int Synchronizer::_dataType_ = 0;
	std::string Synchronizer::_dataTypeName_("synchronization::Synchronizer");
#endif

	const std::string Synchronizer::_statusOKText_                       ("OK");
	const std::string Synchronizer::_statusNothingToDoText_              ("Nothing to do");
	const std::string Synchronizer::_statusStoppingSynchronizerText_     ("Stopping Synchronizer");
	const std::string Synchronizer::_statusCommunicationFailedText_      ("Communication failed");
	const std::string Synchronizer::_statusNotFoundText_                 ("Not Found");
	const std::string Synchronizer::_statusStateMachineFailedText_       ("State Machine failed");
	const std::string Synchronizer::_statusFailedToPauseText_            ("Failed to Pause Synchronizer");

	const std::string Synchronizer::_statusDBExceptionText_              ("DB Exception");

	const std::string Synchronizer::_statusTargetScaleNotFoundText_      ("Target Scale not found");

	const std::string Synchronizer::_statusBatchNotFoundText_            ("Batch not found");

	const std::string Synchronizer::_statusUnknownText_                  ("Unknown");

		
	Synchronizer::Synchronizer(Hobart::CEvolMsgReceiver* syncManager, const char* name)
    #if defined(LINUX_FIX)
		: qMsgManager_(syncManager), msgManager_(0), stoppingSynchronizer_(false), pausingProcessing_(false), reinitDB_(false)
    #else
		: msgManager_(0), stoppingSynchronizer_(false), pausingProcessing_(false), qMsgManager_(syncManager), reinitDB_(false)
    #endif
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		msgManager_ = new Hobart::CEvolMsgManager(this, name, true, true, false);			

	}


	Synchronizer::~Synchronizer()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	void Synchronizer::restoreDB(UINT32 result)
	{
		if ( result == (SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED) )
		{
			if ( !reinitDB_ )
			{
				util_thread::Lock< util_thread::Mutex<> > lock (reinitializingDBMutex_);
				if ( !reinitDB_ )
				{
					util_time::Time t0;

					HFCManagerContainer::init();
					DBAccess::initDB();

					util_time::Time t1;
					Debug::trace("Synchronizer", "After compressing DB, its reinitialization took " +
						util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " msecs.");

					reinitDB_ = true;
				}
				
			}
		}
		else
			reinitDB_ = false;
	}


	/* "Use Case 6/7.51: Stopping synchronizer". */
	/* This method is indirectly called from the destructor the corresponding Synchronization Manager
	when it executes "Use Case 5.6: Stopping Synchronization Manager", which calls stopSynchronizer
	from each queue manager. */
	int Synchronizer::usecaseStoppingSynchronizer()
	{
		/* (uc6/7.51) Set the Stopping Synchronizer flag. */
		stoppingSynchronizer_ = true;

		return _statusOK_;
	}


	const std::string& Synchronizer::getStatusText(int status) const
	{
		switch ( status )
		{
			case _statusOK_:                      return _statusOKText_;
			case _statusStoppingSynchronizer_:    return _statusStoppingSynchronizerText_;
			case _statusCommunicationFailed_:     return _statusCommunicationFailedText_;
			case _statusNotFound_:                return _statusNotFoundText_;
			case _statusStateMachineFailed_:      return _statusStateMachineFailedText_;
			case _statusFailedToPause_:           return _statusFailedToPauseText_;

			case _statusDBException_:             return _statusDBExceptionText_;

			case _statusTargetScaleNotFound_:     return _statusTargetScaleNotFoundText_;

			case _statusBatchNotFound_:           return _statusBatchNotFoundText_;

			default:                              return _statusUnknownText_;
		}
	}


}