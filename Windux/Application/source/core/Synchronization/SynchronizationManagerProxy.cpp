#include "SynchronizationManagerProxy.h"



#include "CEvolMsgManager.h"


#include "Messages/SynchronizationDevEvent.h"
#include "Messages/ProcessUserChangesMessage.h"
#include "Messages/ProcessHQBatchMessage.h"
#include "Messages/ProcessUserFileMessage.h"
#include "Messages/ProcessSynchronizedBatchMessage.h"
#include "Messages/CreateBatchMessage.h"
#include "Messages/CreateBatchProcessedMessage.h"
#include "Messages/AddRecordToBatchMessage.h"
#include "Messages/AbortBatchMessage.h"
#include "Messages/CompleteBatchMessage.h"
#include "ProxyMessage.h"


#include "util/e_string/utilstring.h"
#include "util/time/dbtime.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


	#ifdef USE_ACCOUNTANT
		int SynchronizationManagerProxy::_dataType_ = 0;
		std::string SynchronizationManagerProxy::_dataTypeName_("synchronization::SynchronizationManagerProxy");
	#endif


	SynchronizationManagerProxy::SynchronizationManagerProxy(Hobart::CEvolMsgReceiver* targetMsgReceiver)
			: Hobart::MsgReceiverProxy(targetMsgReceiver)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	SynchronizationManagerProxy::~SynchronizationManagerProxy()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	/* "Use Case 5.20: Processing user changes". */
	bool SynchronizationManagerProxy::processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange> record)
	{
		util_time::Time t0 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "processUserChanges: Attempting processing.");

		Util::CSmartPtr<EvolMessageWrapper> response = processAsProxyMessage(Util::CSmartPtr<EvolMessage>(new ProcessUserChangesMessage(record)));

		util_time::Time t1 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "processUserChanges: Waited " + 
				util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds for execution.");

		if ( response->getDeviceType() == 0 && response->getEventType() == 0 )
			return false;

		return true;
	}


	/* "Use Case 5.21: Processing HQ batch". */
	bool SynchronizationManagerProxy::processHQBatch(int batchNumber)
	{
		util_time::Time t0 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "processHQBatch (batch number " + util_string::inttostring(batchNumber) +
				"): Attempting processing.");

		Util::CSmartPtr<EvolMessageWrapper> response = processAsProxyMessage(Util::CSmartPtr<EvolMessage>(new ProcessHQBatchMessage(batchNumber)));

		util_time::Time t1 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "processHQBatch (batch number " + util_string::inttostring(batchNumber) +
				"): Waited " +  util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds for execution.");

		if ( response->getDeviceType() == 0 && response->getEventType() == 0 )
			return false;

		return true;
	}


	/* "Use Case 5.28: Processing user file". */
	bool SynchronizationManagerProxy::processUserFile(int batchNumber)
	{
		util_time::Time t0 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "processUserFile (batch number " + util_string::inttostring(batchNumber) +
				"): Attempting processing.");

		Util::CSmartPtr<EvolMessageWrapper> response = processAsProxyMessage(Util::CSmartPtr<EvolMessage>(new ProcessUserFileMessage(batchNumber)));

		util_time::Time t1 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "processUserFile (batch number " + util_string::inttostring(batchNumber) +
				"): Waited " +  util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds for execution.");

		if ( response->getDeviceType() == 0 && response->getEventType() == 0 )
			return false;

		return true;
	}


	/* "Use Case 5.37: Processing synchronized batch". */
	bool SynchronizationManagerProxy::processSynchronizedBatch(int batchNumber)
	{
		util_time::Time t0 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "processSynchronizedBatch (batch number " +
				util_string::inttostring(batchNumber) + "): Attempting processing.");

		Util::CSmartPtr<EvolMessageWrapper> response = processAsProxyMessage(Util::CSmartPtr<EvolMessage>(new ProcessSynchronizedBatchMessage(batchNumber)));

		util_time::Time t1 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "processSynchronizedBatch (batch number " + util_string::inttostring(batchNumber) +
				"): Waited " + util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds for execution.");

		if ( response->getDeviceType() == 0 && response->getEventType() == 0 )
			return false;

		return true;
	}


	/* "Use Case 5.40: Creating batch". */
	bool SynchronizationManagerProxy::createBatch(int* batchNumber, hfc::Batch::BatchOrigin origin, hfc::Batch::BatchType type)
	{
		util_time::Time t0 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "createBatch (origin " + hfc::Batch::getOriginName(origin) +
			", type " + hfc::Batch::getTypeName(type) + "): Attempting processing.");

		Util::CSmartPtr<EvolMessageWrapper> response = processAsProxyMessage(Util::CSmartPtr<EvolMessage>(new CreateBatchMessage(origin, type)));

		util_time::Time t1 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "createBatch (origin " + hfc::Batch::getOriginName(origin) + ", type " +
			hfc::Batch::getTypeName(type) + "): Waited " + util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds for execution.");


		/* If we received the expected response, convert it into the corresponding EvolMessage and
		extract the information to be returned. */
		if ( response->getDeviceType() == SynchronizationDev && response->getEventType() == CreateBatchProcessedEvent )
		{
			Util::CSmartPtr<CreateBatchProcessedMessage> result(new CreateBatchProcessedMessage(response->Data()));

			if ( batchNumber )
				*batchNumber = result->getBatchId();

			Debug::trace("SynchronizationManagerProxy", "createBatch (origin " + hfc::Batch::getOriginName(origin) +
				", type " + hfc::Batch::getTypeName(type) + "): New batchId = " + util_string::inttostring(result->getBatchId()));

			return true;
		}
		else
		{
			*batchNumber = hfc::Batch::_invalidBatchNumber_;

			Debug::trace("SynchronizationManagerProxy", "createBatch (origin " + hfc::Batch::getOriginName(origin) +
				", type " + hfc::Batch::getTypeName(type) + "): Did not receive the expected response");

			return false;
		}
	}


	/* "Use Case 5.41: Adding record to batch". */
	bool SynchronizationManagerProxy::addRecordToBatch(int batchNumber, Util::CSmartPtr<hfc::SynchronizationChange> record)
	{
		util_time::Time t0 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "addRecordToBatch (batch number " + util_string::inttostring(batchNumber) +
			"): Attempting processing.");

		Util::CSmartPtr<EvolMessageWrapper> response = processAsProxyMessage(Util::CSmartPtr<EvolMessage>(new AddRecordToBatchMessage(batchNumber, record)));

		util_time::Time t1 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "addRecordToBatch (batch number " + util_string::inttostring(batchNumber) +
			"): Waited " + util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds for execution.");

		if ( response->getDeviceType() == 0 && response->getEventType() == 0 )
			return false;

		return true;
	}


	/* "Use Case 5.42: Aborting batch". */
	bool SynchronizationManagerProxy::abortBatch(int batchNumber)
	{
		util_time::Time t0 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "abortBatch (batch number " + util_string::inttostring(batchNumber) +
			"): Attempting processing.");

		Util::CSmartPtr<EvolMessageWrapper> response = processAsProxyMessage(Util::CSmartPtr<EvolMessage>(new AbortBatchMessage(batchNumber)));

		util_time::Time t1 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "abortBatch (batch number " + util_string::inttostring(batchNumber) +
			"): Waited " + util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds for execution.");

		if ( response->getDeviceType() == 0 && response->getEventType() == 0 )
			return false;

		return true;
	}


	/* "Use Case 5.43: Completing batch". */
	bool SynchronizationManagerProxy::completeBatch(int batchNumber)
	{
		util_time::Time t0 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "completeBatch (batch number " + util_string::inttostring(batchNumber) +
			"): Attempting processing.");

		Util::CSmartPtr<EvolMessageWrapper> response = processAsProxyMessage(Util::CSmartPtr<EvolMessage>(new CompleteBatchMessage(batchNumber)));

		util_time::Time t1 = util_time::Time();
		Debug::trace("SynchronizationManagerProxy", "completeBatch (batch number " + util_string::inttostring(batchNumber) +
			"): Waited " + util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds for execution.");

		if ( response->getDeviceType() == 0 && response->getEventType() == 0 )
			return false;

		return true;
	}


}
