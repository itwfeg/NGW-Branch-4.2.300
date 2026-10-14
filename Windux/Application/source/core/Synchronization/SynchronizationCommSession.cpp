#include "SynchronizationCommSession.h"


#include "ScaleManager.h"

#include "SynchronizationChange.h"

#include "SessionManager.h"

#include "SessionMessage.h"


#include "Messages/SynchronizationDevEvent.h"

#include "bootParameters.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{



#ifdef USE_ACCOUNTANT
	int SynchronizationCommSession::_dataType_ = 0;
	const std::string SynchronizationCommSession::_dataTypeName_("synchronization::SynchronizationCommSession");
#endif



	SynchronizationCommSession::SynchronizationCommSession(ScaleSynchronizationType scaleSynchronizationType,
			hfc::Scale::ScaleRole scaleRole, CommMechanism * uiClient, const char* name, bool inputEnabled, int reason)
			: CommSession(uiClient, name ? name : "SynchronizationCommSession", inputEnabled, reason),
			scaleSynchronizationType_(scaleSynchronizationType), scaleRole_(scaleRole)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	SynchronizationCommSession::~SynchronizationCommSession()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	bool SynchronizationCommSession::writeBatch(Util::CSmartPtr<hfc::Batch> batch)
	{
		Util::CSmartPtr<SessionMessage> message(new SessionMessage(SynchronizationDev, WriteBatch));
		message->setFormatType(SynchronizationTaggedFormat);
		message->setHfcObject(batch.get());

		return writeMessage(message);
	}


	bool SynchronizationCommSession::writeSynchronizationChange(Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		Util::CSmartPtr<SessionMessage> message(new SessionMessage(SynchronizationDev, WriteSynchronizationChange));
		message->setFormatType(SynchronizationTaggedFormat);
		message->setHfcObject(change.get());

		return writeMessage(message);
	}


	bool SynchronizationCommSession::doSendHeader(CommSessionEvent::CommError& returnedError)
	{
		returnedError = CommSessionEvent::COMMERROR_NO_ERROR;


		/* (uc4.22) Send this scale's communication header to the other scale. */
		Util::CSmartPtr<hfc::HeaderRecord> header(new hfc::HeaderRecord());

		header->setDataType(hfc::HeaderRecord::TypeSyncComm);
		header->setScaleRole(scaleRole_);
		header->setHostName(bootParameters::getDNSHostName());
		header->setIPAddress(bootParameters::getIPAddress());
		header->setCurrentDateTime();

		if ( !writeHeader(header) )
			returnedError = CommSessionEvent::COMMERROR_WRITING_TO_SOCKET;

		return true;
	}


	bool SynchronizationCommSession::doSendFooter(CommSessionEvent::CommError& returnedError)
	{
		returnedError = CommSessionEvent::COMMERROR_NO_ERROR;


		Util::CSmartPtr<SessionMessage> footer(new SessionMessage(SessionDev, SnCommEnd));
		footer->setFormatType(SynchronizationTaggedFormat);

		if ( writeFooter(footer) )
		{
			if ( !isPausingProcessing() )
			{
				/* (uc4.22) Update the Secondary scale's database record with the latest information
				(date/time, result, etc. */
				if ( remoteScale_.notNull() )
				{
					remoteScale_->setLastCommSuccessful();
					getScaleManager()->saveScale(remoteScale_);
				}
			}
		}
		else
			returnedError = CommSessionEvent::COMMERROR_WRITING_TO_SOCKET;


		return true;
	}



}