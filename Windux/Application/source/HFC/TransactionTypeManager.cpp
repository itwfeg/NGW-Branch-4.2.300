#include "TransactionTypeManager.h"

#include "TransactionTypeEvent.h"
#include "TransactionTypeNamesMessage.h"
#include "ProductDevEvent.h"
#include "SessionMessage.h"


#include "SessionManager.h"
#include "Session.h"
#include "HFCManagerContainer.h"
#include "Configuration.h"
#include "GenericHfcObject.h"
#include "ProductTotalManager.h"
#include "HfcProductChangeMsg.h"
#include "ProductChangeMsg.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "DBAccess.h"
#include "DBException.h"

#include "MultiLanguageText.h"


#include "Debug.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
		int TransactionTypeManager::_dataType_ = 0;
		std::string TransactionTypeManager::_dataTypeName_("hfc::TransactionTypeManager"); 
#endif


	TransactionTypeManager::TransactionTypeManager(hfc::HFCManagerContainer * container)
			: mySession_(0), mgrContainer_(container)
	{
#ifdef USE_ACCOUNTANT
			if (_dataType_ == 0)
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
#endif

		prevTransTypeNum  = 0;
	}


	TransactionTypeManager::~TransactionTypeManager()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	Util::CSmartPtr<TransactionType> TransactionTypeManager::getSupervisorModeTransactionType()
	{
		if ( supervisorModeTransactionType_.null() )
		{
			// If we don't have a current transaction type, then automatically make 'Production' the current one.
			Debug::trace("TransactionTypeManager",
				"getSupervisorModeTransactionType():  currentTransactionType is NULL. Setting Prepack totals as current.");

			supervisorModeTransactionType_ = findTransactionType(TransactionType::_PRODUCTION_TYPE_);
			if ( supervisorModeTransactionType_.null() )
			{
				verifyReservedRecords();
				supervisorModeTransactionType_ = findTransactionType(TransactionType::_PRODUCTION_TYPE_);
			}
		}

		return supervisorModeTransactionType_;
	}


	Util::CSmartPtr<TransactionType> TransactionTypeManager::getOperatorModeTransactionType()
	{
		//	This is the Total type selected in Operate mode
		if ( operatorModeTransactionType_.null() )
		{
			// If we don't have a current RunMode total type, then automatically make 'Prepack' the current one.
			Debug::trace("TransactionTypeManager",
					"getOperatorModeTransactionType():   operatorModeTransactionType is NULL. Setting Prepack totals as current.");
				
			operatorModeTransactionType_ = findTransactionType(TransactionType::_PRODUCTION_TYPE_);
			if ( operatorModeTransactionType_.null() )
			{
				verifyReservedRecords();
				operatorModeTransactionType_ = findTransactionType(TransactionType::_PRODUCTION_TYPE_);
			}
		}

		return operatorModeTransactionType_;
	}


	void TransactionTypeManager::handleMessage(SessionMessage * msg)
	{
		if ( !msg ) return;


		if ( msg->getDeviceType() == TransactionTypeDev )
		{
			switch ( msg->getEventType() )
			{
				case DtRead:
					handleRead(dynamic_cast<TransactionType*>(msg->getHfcObject()));
					break;
				case DtWrite:
					handleWrite(dynamic_cast<TransactionType*>(msg->getHfcObject()));
					break;
				case DtDelete:
				case DtBatchApplyDelete:
					handleDelete(msg);
					break;
				case DtDeleteAll:
					handleDeleteAll();
					break;
				case DtSave:
					handleUpdateAndSave(dynamic_cast<TransactionType*>(msg->getHfcObject()));
					break;
				case DtRestore:
					handleRestore(msg);
					break;
				case DtCreateNew:
					handleCreate();
					break;
				case TransactionTypeReadAllNames:
					handleReadAllTransactionTypeNames();
					break;
/*				case TransactionTypeSetCurrentType:
					handleSetCurrentType(dynamic_cast<TransactionType*>(msg->getHfcObject()));
					break;
				case TransactionTypeSetCurrentRunModeType:
					handleSetCurrentRunModeType(dynamic_cast<TransactionType*>(msg->getHfcObject()));
					break; */
				case TransactionTypeReadCurrentType:
					handleReadCurrentType();
					break;
				case TransactionTypeWriteCurrentType:
				{
					GenericHfcObject *msgdata = dynamic_cast<GenericHfcObject *>(msg->getHfcObject());
					if ( msgdata )
						handleWriteCurrentType(util_string::stringtolong(msgdata->getData()));

					break;
				}
				case TransactionTypeRefreshCurrentType:
					handleReadCurrentType();
					break;
				default:
					Debug::trace("TransactionTypeManager", "UNKNOWN EVENT: " + util_string::uinttostring(msg->getEventType()));
					break;
			}
		}
	}


	void TransactionTypeManager::handleRead(TransactionType * transType)
	{
		if ( !transType ) return;


		bool traceOn = Debug::isTracing("TransactionTypeManager");
		std::string method("handleRead():  ");
		if ( traceOn )
			Debug::trace("TransactionTypeManager", method + "TransactionType # " + util_string::longtostring(transType->getNumber()));

		Util::CSmartPtr<TransactionType> lookedUpTransactionType = findTransactionType(transType->getNumber());
		if ( lookedUpTransactionType.notNull() )
		{
			if ( lookedUpTransactionType->isRecordValid() )
			{
				// Send the TransactionType found back to the Browser.
				SessionMessage transTypeData(TransactionTypeDev, DtWrite);
				transTypeData.setFormatType(BrowserFormat);
				transTypeData.setHfcObject(lookedUpTransactionType.get());
				mySession_->send(&transTypeData, Messenger::DEST_UI);

				// We have a new current total type.
				setSupervisorModeTransactionType(lookedUpTransactionType);
			}
			else
			{
				// Send error message to browser indicating a bad Total type.
				if ( mySession_ )
					mySession_->addDialogMessage(ErrTransactionTypeDev, TransactionTypeErr, HfcResponse::RESPONSESTYLE_OK);
			}
		}
	}


	void TransactionTypeManager::handleWrite(TransactionType *transType)
	{
		if ( !transType ) return;

		bool traceOn = Debug::isTracing("TransactionTypeManager");
		std::string method("handleWrite():  ");
		if ( traceOn )
			Debug::trace("TransactionTypeManager", method + "TransactionType # " + util_string::longtostring(transType->getNumber()));


		if ( supervisorModeTransactionType_.null() )
			supervisorModeTransactionType_ = findTransactionType(transType->getNumber());

		if ( supervisorModeTransactionType_.notNull() )
			supervisorModeTransactionType_->updateTransactionType(transType);
	}


	void TransactionTypeManager::handleDelete(SessionMessage * msg)
	{
		if ( !msg ) return;

		TransactionType *transType = dynamic_cast<TransactionType *> (msg->getHfcObject());
		if ( !transType ) return;


		bool traceOn = Debug::isTracing("TransactionTypeManager");
		std::string method("handleDelete():  ");
		if ( traceOn )
			Debug::trace("TransactionTypeManager", method + "TransactionType # " + util_string::longtostring(transType->getNumber()));

		// The following code in the true part of the conditional will not be executed,
		// because there is no 'Delete One' function code defined for Total Types.
		// I put it here for consistency, and if we define this record type in the future, the code will be here.
		if ( mySession_ && mySession_->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::TransactionTypes) )
		{
			// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
			Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
			if ( proxy )
				proxy->addRecordToBatch(mySession_->getBatchId(), receivedChange);
		}
		else
		{
			Util::CSmartPtr<TransactionType> usedTransactionType =
				msg->getEventType() == DtBatchApplyDelete ? findTransactionType(transType->getNumber()) : supervisorModeTransactionType_;

			// Can't delete the Prepack TransactionType!
			if ( usedTransactionType.notNull() && usedTransactionType->getNumber() != TransactionType::_PRODUCTION_TYPE_ )
			{
				HobRecordNum typeNumber = usedTransactionType->getNumber();

				//	Delete the current Total type data
				deleteRecord(usedTransactionType);


				if ( msg->getEventType() != DtBatchApplyDelete )
				{
					// if received via UI, delete it and batch the record being deleted.
					if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::TransactionTypes) )
					{
						std::string serialization;
						serializeForSynchronization(serialization, usedTransactionType.get(), getDeleteEvent());

						synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

						if (proxy)
							proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
								hfc::SynchronizationChange(getDevice(), getDeleteEvent(), serialization)));
					}


					// Have to do the following after batching the delete, or we will have deleted the pointer
					// to the 'used' transaction type in the setSupervisorModeTransactionType method.
					clearSupervisorModeTransactionType();		//	Clear the currentTransactionType.

					//	Clear the OperatorModeTransactionType if it is the same as the transaction type that was just deleted.
					if ( operatorModeTransactionType_.notNull() && typeNumber == operatorModeTransactionType_->getNumber() )
						clearOperatorModeTransactionType();
				}
			}
		}
	}


	void TransactionTypeManager::handleDeleteAll()
	{
		bool traceOn = Debug::isTracing("TransactionTypeManager");
		std::string method("handleDeleteAll():  ");
		if ( traceOn )
			Debug::trace("TransactionTypeManager", method + "deleting all transaction types");

		if ( mySession_ && mySession_->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::TransactionTypes) )
		{
			// batch the delete all command; don't actually do it right now.
			SessionMessage message(getDevice(), getDeleteAllEvent());
			message.setFormatType(SynchronizationTaggedFormat);
			const char * buffer = message.serialize();

			Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getDeleteAllEvent(), std::string(buffer, message.getSerializedLength())));
			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
			if ( proxy )
				proxy->addRecordToBatch(mySession_->getBatchId(), receivedChange);
		}
		else
		{
			DBTransactionTypesRM* dbTransactionTypesRM = mgrContainer_->getDBTransactionTypesRM();
			if ( !dbTransactionTypesRM )
				return;

			dbTransactionTypesRM->deleteAll();

			destroyQueriesAndIters();

			clearSupervisorModeTransactionType();              // Clear the currentTransactionType.
			clearOperatorModeTransactionType();              // Clear the currentRunModeTransactionType.

			if ( traceOn )
				Debug::trace("TransactionTypeManager", method + "will create reserved TransactionType records next");

			verifyReservedRecords();
		}

		if ( traceOn )
			Debug::trace("TransactionTypeManager", method + "leaving");
	}


	void TransactionTypeManager::handleUpdateAndSave(TransactionType * modifiedTransactionType)
	{
		bool traceOn = Debug::isTracing("TransactionTypeManager");
		std::string method("handleUpdateAndSave():  ");
		if ( traceOn )
			Debug::trace("TransactionTypeManager", method + "currentTransactionType = " + util_string::ptrtostring(supervisorModeTransactionType_.get()));

		if ( supervisorModeTransactionType_.notNull() )
		{
			if ( modifiedTransactionType )
			  	supervisorModeTransactionType_->updateTransactionType(modifiedTransactionType);

			handleSaveRecord(supervisorModeTransactionType_);

			//	Clear the OperatorModeTransactionType if it is the same as the transaction type that was just modified.
			if ( operatorModeTransactionType_.notNull() &&
					supervisorModeTransactionType_->getNumber() == operatorModeTransactionType_->getNumber() )
				clearOperatorModeTransactionType();
		}
	}
	

	void TransactionTypeManager::handleSaveRecord(Util::CSmartPtr<TransactionType> recordToSave)
	{
		if ( recordToSave.notNull() && !recordToSave->getName1().empty() )
		{
			saveRecord(recordToSave);

			if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::TransactionTypes) )
			{
				std::string serialization;
				serializeForSynchronization(serialization, recordToSave.get());

				synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

				if (proxy)
					proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
						hfc::SynchronizationChange(getDevice(), getWriteEvent(), serialization)));
			}
		}
	}


	void TransactionTypeManager::handleRestore(SessionMessage *msg)
	{
		if ( !msg ) return;

		Util::CSmartPtr<TransactionType> restoreType(dynamic_cast<TransactionType *>(msg->getHfcObject()));
		if ( restoreType.null() ) return;


		bool traceOn = Debug::isTracing("TransactionTypeManager");
		std::string method("handleRestore():  ");
		if ( traceOn )
			Debug::trace("TransactionTypeManager", method + "restoring TransactionType = " + util_string::longtostring(restoreType->getNumber()));

		msg->setHfcObjectCreatedInternally(false);
		if ( mySession_ && mySession_->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::TransactionTypes) )
		{
			Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

			if ( msg->getFormatType() == UltimaTaggedFormat )
			{
				std::string reserialization;
				serializeForSynchronization(reserialization, restoreType.get());

				receivedChange = new hfc::SynchronizationChange(getDevice(), getWriteEvent(), reserialization);
			}
			else
				receivedChange = new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff());

			// batch the record being restored; don't actually write it to the database right now.
			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
			if ( proxy )
				proxy->addRecordToBatch(mySession_->getBatchId(), receivedChange);
		}
		else
		{
			Util::CSmartPtr<TransactionType> existingType = findTransactionType(restoreType->getNumber());
			if ( existingType.notNull() )
			{
				if ( existingType->isRecordValid() )
				{
					existingType->updateTransactionType(restoreType);
					saveRecord(existingType);
				}
				else
				{
					deleteRecord(existingType);
					saveRecord(restoreType);
				}
			}
			else
				saveRecord(restoreType);
		}
	}


	void TransactionTypeManager::handleCreate()
	{
		if ( !mySession_ )
			return;

		bool traceOn = Debug::isTracing("TransactionTypeManager");
		std::string method("handleCreate():  ");

		if ( countAllRecords() < _MaximumRecords_ )
		{
			// FT-759 refresh query to current db snapshot
			destroyQueriesAndIters();

			// Find the next available "slot"
			HobRecordNum availableSlot = 1;
			Util::CSmartPtr<TransactionType> transType = getFirstTransactionType();

			for ( ; transType.notNull() && availableSlot == transType->getNumber(); transType = getNextTransactionType(), ++availableSlot)
				;

			if ( 1 < availableSlot && availableSlot <= _MaximumRecords_ )
			{
				transType = new TransactionType();
				transType->setNumber(availableSlot);
				transType->setErrorCode(RECORD_OK);

				// Send the NEW TransactionType to the Browser.
				SessionMessage transTypeData(TransactionTypeDev, DtWrite);
				transTypeData.setFormatType(BrowserFormat);
				transTypeData.setHfcObject(transType.get());
				mySession_->send(&transTypeData, Messenger::DEST_UI);

				// We have a new current transaction type.
				setSupervisorModeTransactionType(transType);

				if ( traceOn )
					Debug::trace("TransactionTypeManager", method + "created new type with TransactionType# = " + util_string::longtostring(availableSlot));
			}
		}
		else
		{
			if ( traceOn )
				Debug::trace("TransactionTypeManager", method + "unable to create new TransactionType: reached maximum number of records (" +
						util_string::longtostring(_MaximumRecords_) + ")");
		}
	}


	void TransactionTypeManager::handleReadAllTransactionTypeNames()
	{
		Session* session = dynamic_cast<Session*>(mySession_);
		if ( !session ) return;


		bool traceOn = Debug::isTracing("TransactionTypeManager");
		std::string method("handleReadAllTransactionTypeNames():  ");
		if ( traceOn )
			Debug::trace("TransactionTypeManager", method + "starting");

		destroyQueriesAndIters();

		TransactionTypeNamesMessage typeNames;

		Util::CSmartPtr<TransactionType> transType(firstTransactionType());
		while ( transType.notNull() )
		{
			typeNames.addTransactionType(util_string::longtostring(transType->getNumber()),
				transType->getName1(), transType->getName2());

			transType = nextTransactionType();
		}


		/* Send the response to the User Interface. */
		SessionMessage outputMessage(TransactionTypeDev, TransactionTypeWriteAllNames);
		outputMessage.setFormatType(BrowserFormat);
		outputMessage.setHfcObject(&typeNames);

		mySession_->send(&outputMessage, Messenger::DEST_UI);
	}


	void TransactionTypeManager::handleSetCurrentType(TransactionType *transType)
	{
		if ( !transType ) return;


		bool traceOn = Debug::isTracing("TransactionTypeManager");
		std::string method("handleSetCurrentType():  ");
		if ( traceOn )
			Debug::trace("TransactionTypeManager", method + "Read TransactionType # " + util_string::longtostring(transType->getNumber()));

		Util::CSmartPtr<TransactionType> lookedUpTransactionType = findTransactionType(transType->getNumber());
		if ( lookedUpTransactionType.notNull() )
		{
			if ( lookedUpTransactionType->isRecordValid() )
			{
				// We do NOT send the TransactionType found back to the Browser.
				// We are simply selecting the TransactionType for use with Totals.

				// We have a new current transaction type.
				setSupervisorModeTransactionType(lookedUpTransactionType);
				if ( traceOn )
					Debug::trace("TransactionTypeManager", method + "We have a new TransactionType");

				// Notify the ProductManager that the Transaction type changed.
				SessionMessage notifyMsg(ProductDev, ProductTransactionTypeChanged);
				notifyMsg.setFormatType(BrowserFormat);
				//mySession_->getProductManager()->handleMessage(&notifyMsg);
				Session *s = dynamic_cast<Session *> (mySession_);
				if (s)
					s->handleMessage(&notifyMsg);
			}
			else
			{
				// Send error message to browser indicating a bad Total type.
				if ( mySession_ )
					mySession_->addDialogMessage(ErrTransactionTypeDev, TransactionTypeErr, HfcResponse::RESPONSESTYLE_OK);
			}
		}
	}


	void TransactionTypeManager::handleSetCurrentRunModeType(TransactionType *transType)
	{
		Session * session = dynamic_cast<Session *>(mySession_);
		if ( !session || !transType ) return;


		bool traceOn = Debug::isTracing("TransactionTypeManager");
		std::string method("handleSetCurrentRunModeType():  ");
		if ( traceOn )
			Debug::trace("TransactionTypeManager", method + "Read TransactionType # " + util_string::longtostring(transType->getNumber()));

		Util::CSmartPtr<TransactionType> lookedUpTransactionType = findTransactionType(transType->getNumber());
		if ( lookedUpTransactionType.null() || (lookedUpTransactionType.notNull() && lookedUpTransactionType->getErrorCode() != RECORD_OK) )
		{
			// Since we can't find this total type, or it has some problems, default to Production totals.
			Util::CSmartPtr<TransactionType> tempTransactionType = findTransactionType(TransactionType::_PRODUCTION_TYPE_);

			if ( tempTransactionType.null() )
			{
				if ( traceOn )
					Debug::trace("TransactionTypeManager", method + "Cannot read the selected TransactionType from the Database. Attempting to read Production Total type.");

				verifyReservedRecords();
				tempTransactionType = findTransactionType(TransactionType::_PRODUCTION_TYPE_);
			}

			if ( tempTransactionType.null() )
			{
				if ( traceOn )
					Debug::trace("TransactionTypeManager", method + "Cannot read the Production TransactionType from the Database either!!!");
			}
			else
			{
				setOperatorModeTransactionType(tempTransactionType);

				if ( traceOn )
					Debug::trace("TransactionTypeManager", method + "We have a new Run mode TransactionType:  Production");

				//	Resend the Weigh mode / Total type message back to the Browser, so it can update the Select box.
				SessionMessage updateTransactionTypeMsg(SessionDev, SnReadWeighModeTransactionType);
				session->handleMessage(&updateTransactionTypeMsg);
			}
		}
		else
		{
			// We do NOT send the TransactionType found back to the Browser.
			// We are simply selecting the TransactionType for use with Totals.

			// We have a new current total type.
			setOperatorModeTransactionType(lookedUpTransactionType);

			if ( traceOn )
				Debug::trace("TransactionTypeManager", method + "We have a new Run mode TransactionType");

			// Notify the ProductManager that the Total type changed.
			SessionMessage notifyMsg(ProductDev, ProductTransactionTypeChanged);
			notifyMsg.setFormatType(BrowserFormat);
			session->handleMessage(&notifyMsg);

			// Have the Session send a new Weigh mode / Total type message.
			SessionMessage wmttMsg(SessionDev, SnReadWeighModeTransactionType);
			session->handleMessage(&wmttMsg);
		}
	}


	void TransactionTypeManager::handleReadCurrentType()
	{
		const bool traceOn( Debug::isTracing("TransactionTypeManager") );
		const std::string method( "handleReadCurrentType(): ");

		Util::CSmartPtr<TransactionType> type( getOperatorModeTransactionType() );

		if (( !mySession_ ) || ( type.null() ))
			return;

		Session *s( dynamic_cast<Session*>( mySession_ ) );

		const bool isInOperateMode( (s->isInOperateMode()) ? true : false );
		const bool isPrimarySession( (mySession_->isPrimarySession()) ? true : false );

		const int previousFormatType( type->getFormatType() );

		if ( traceOn )
			Debug::trace("TransactionTypeManager", method + "PrevTransactionType # " + util_string::longtostring(previousFormatType));

		SessionMessage outputMessage( TransactionTypeDev, TransactionTypeWriteCurrentType );
		outputMessage.setFormatType( BrowserFormat );
		outputMessage.setHfcObject( type.get() );

		type->setFormatType( CurrentRunFormat );

		// SOF-3216: only send when primary session connected
		if ( isPrimarySession )
			mySession_->send( &outputMessage, Messenger::DEST_UI );

		type->setFormatType( previousFormatType );

		// we just changed the transaction type and it might be printing the transaction type text
		// on the label which is a preimage field so we need to send this ProductChange smart to the
		// mode so we are forced to preimage the label again (like when we change the label in Operate)
		if (( isPrimarySession ) && ( isInOperateMode ))
		{
			// Send a Product Change message to the RTC so it knows the Imager needs to image the new label.
			type->getTextForLabel();
			
			SessionMessage pluChg( ProductDev, ProductChangeSmart );
			pluChg.setFormatType( InternalFormat );

			HfcProductChangeMsg hpcm( mySession_, mySession_->getProductManager() );
			pluChg.setHfcObject( &hpcm );
			mySession_->send( &pluChg, Messenger::DEST_RTC );
		}
	}


	void TransactionTypeManager::handleWriteCurrentType(HobRecordNum number)
	{
		Session * session = dynamic_cast<Session *>(mySession_);
		if ( !session || number <= 0 ) return;


		bool traceOn = Debug::isTracing("TransactionTypeManager");
		std::string method("handleWriteCurrentType():  ");
		if ( traceOn )
			Debug::trace("TransactionTypeManager", method + "TransactionType # " + util_string::longtostring(number));

		Util::CSmartPtr<TransactionType> lookedUpTransactionType = findTransactionType(number);
		if ( lookedUpTransactionType.null() || (lookedUpTransactionType.notNull() && lookedUpTransactionType->getErrorCode() != RECORD_OK) )
		{
			// Since we can't find this total type, or it has some problems, default to Production totals.
			Util::CSmartPtr<TransactionType> tempTransactionType = findTransactionType(TransactionType::_PRODUCTION_TYPE_);

			if ( tempTransactionType.null() )
			{
				if ( traceOn )
					Debug::trace("TransactionTypeManager", method + "Cannot read the selected TransactionType from the Database. Attempting to read Production Total type.");

				verifyReservedRecords();
				tempTransactionType = findTransactionType(TransactionType::_PRODUCTION_TYPE_);
			}

			if ( tempTransactionType.null() )
			{
				if ( traceOn )
					Debug::trace("TransactionTypeManager", method + "Cannot read the Production TransactionType from the Database either!!!");
			}
			else
			{
				setOperatorModeTransactionType(tempTransactionType);

				if ( traceOn )
					Debug::trace("TransactionTypeManager", method + "We have a new Run mode TransactionType:  Production");

				//	Resend the Transaction type message back to the Browser, so it can update the Select box.
				handleReadCurrentType();
//				SessionMessage updateTransactionTypeMsg(SessionDev, SnReadWeighModeTransactionType);
//				session->handleMessage(&updateTransactionTypeMsg);
			}
		}
		else
		{
			// We do NOT send the TransactionType found back to the Browser.
			// We are simply selecting the TransactionType for use with Totals.

			// We have a new current total type.
			setOperatorModeTransactionType(lookedUpTransactionType);

			if ( traceOn )
				Debug::trace("TransactionTypeManager", method + "We have a new Run mode TransactionType");

			// Notify the ProductManager that the Total type changed.
			SessionMessage notifyMsg(ProductDev, ProductTransactionTypeChanged);
			notifyMsg.setFormatType(BrowserFormat);
			session->handleMessage(&notifyMsg);

			// Have the Session send a new Transaction type message.
			handleReadCurrentType();
//			SessionMessage wmttMsg(SessionDev, SnReadWeighModeTransactionType);
//			session->handleMessage(&wmttMsg);
		}
		if (mgrContainer_->getProductTotalManager())
			mgrContainer_->getProductTotalManager()->clearAndDisplayProductRunTotal();
	}


	Util::CSmartPtr<TransactionType> TransactionTypeManager::findTransactionType(HobRecordNum typeNumber)
	{
		Util::CSmartPtr<TransactionType> retrievedTransactionType;
		edb::DBTransactionTypesRM* dbTransactionTypesRM = mgrContainer_->getDBTransactionTypesRM();
		if ( !dbTransactionTypesRM || typeNumber <= 0 ) return retrievedTransactionType;


		try
		{
			Util::CSmartPtr<edb::DBTransactionType> dbTransactionTypeRec(dbTransactionTypesRM->findNumber(util_string::longtostring(typeNumber)));
			if ( dbTransactionTypeRec.notNull() )
			{
				retrievedTransactionType = createTransactionType(dbTransactionTypeRec.get());

				if ( retrievedTransactionType.notNull() )
				{
					if ( retrievedTransactionType->getErrorCode() == RECORD_CORRUPT )
					{
						dbTransactionTypesRM->deleteRecord(dbTransactionTypeRec.get());
						retrievedTransactionType = 0;
					}
					else
						Debug::trace("TransactionTypeManager", "findTransactionType() - found transaction type record in the db");
				}
			}
		}
		catch (DBException &dbe)
		{
			if (Debug::isTracing("DBAccess.exception"))
			{
				Debug::trace("DBAccess.exception", "TransactionTypeManager::findTransactionType() ********** database exception **********");
				HobString myDebug("     DBException: type: ");
				myDebug.append(util_string::inttostring(dbe.getType())).append("  info: ").append(dbe.info());
				Debug::trace("DBAccess.exception", myDebug);
			}

			retrievedTransactionType = 0;
		}

		return retrievedTransactionType;
	}


	TransactionType* TransactionTypeManager::createTransactionType(const edb::DBTransactionType* dbTransactionTypeRec)
	{
		TransactionType* transactionType = 0;
		if ( !dbTransactionTypeRec ) return transactionType;


		transactionType = new TransactionType();
		if ( dbTransactionTypeRec->isRecordValid() )
		{
			transactionType->setNumber(util_string::stringtolong(dbTransactionTypeRec->getNumber()));
			transactionType->setName1(dbTransactionTypeRec->getName1());
			transactionType->setName2(dbTransactionTypeRec->getName2());
			transactionType->setAffectsProduction((TransactionType::AffectsProductionFlag)dbTransactionTypeRec->getAffectingProduction());
			transactionType->setAutomaticChangeUPCDigit(dbTransactionTypeRec->getAutomaticallyChangingUPCDigit());
			transactionType->setManualChangeUPCDigit(dbTransactionTypeRec->getManuallyChangingUPCDigit());
			transactionType->setUPCChangeDigit(dbTransactionTypeRec->getUPCDigit());
			transactionType->setAllowProductBackDating(dbTransactionTypeRec->getAllowingBackDating());
			transactionType->setUseBackgroundColor(dbTransactionTypeRec->getUseBackgroundColor());
			transactionType->setBackgroundColor(dbTransactionTypeRec->getBackgroundColor());
			transactionType->setTextForLabel(dbTransactionTypeRec->getTextForLabel());

			transactionType->setErrorCode(RECORD_OK);

			Debug::trace("TransactionTypeManager", "Read TransactionType db record OK");
		}
		else
		{
			transactionType->setErrorCode(RECORD_CORRUPT);

			Debug::trace("TransactionTypeManager", "Read TransactionType db record CORRUPT!!");
		}

		return transactionType;
	}


	void TransactionTypeManager::saveRecord(Util::CSmartPtr<TransactionType> transType)
	{
		DBTransactionTypesRM* dbTransactionTypesRM = mgrContainer_->getDBTransactionTypesRM();
		if ( !dbTransactionTypesRM || transType.null() || transType->getNumber() <= 0 )
			return;


		HobRecordNum number = transType->getNumber();
		Util::CSmartPtr<DBTransactionType> dbTransactionType(dbTransactionTypesRM->findNumber(util_string::longtostring(number)));
		if ( dbTransactionType.null() )
		{
			Debug::trace("TransactionTypeManager", "saveRecord() - don't have a db record, creating it...");

			dbTransactionType = dbTransactionTypesRM->createRecord();
			dbTransactionType->setNumber(util_string::longtostring(number));
		}

		if ( dbTransactionType.notNull() )
		{
			//	Plug the changes back into the dbTransactionTypeRecord
			dbTransactionType->setName1(transType->getName1());
			dbTransactionType->setName2(transType->getName2());
			dbTransactionType->setAffectingProduction(transType->getAffectsProduction());
			dbTransactionType->setAllowingBackDating(transType->isAllowProductBackDating());
			dbTransactionType->setManuallyChangingUPCDigit(transType->isManualChangeUPCDigit());
			dbTransactionType->setAutomaticallyChangingUPCDigit(transType->isAutomaticChangeUPCDigit());
			dbTransactionType->setUPCDigit(transType->getUPCChangeDigit());
			dbTransactionType->setUseBackgroundColor(transType->isUseBackgroundColor());
			dbTransactionType->setBackgroundColor(transType->getBackgroundColor());
			dbTransactionType->setTextForLabel(transType->getTextForLabel());

			try
			{
				dbTransactionTypesRM->saveRecord(dbTransactionType.get());		//save record to the database
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "TransactionType::saveRecord() ********** database exception **********");
				Debug::trace("DBAccess.exception", "     DBException: type:" + util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
			}
		}
	}


	int TransactionTypeManager::deleteRecord(Util::CSmartPtr<TransactionType> transType)
	{
		edb::DBTransactionTypesRM* dbTransactionTypesRM = mgrContainer_->getDBTransactionTypesRM();
		if ( transType.null() || !dbTransactionTypesRM )
			return 0;


		Debug::trace("TransactionTypeManager", "Going to delete transaction type (" + util_string::longtostring(transType->getNumber()) + ")");

		Util::CSmartPtr<DBTransactionType> dbTransactionType(dbTransactionTypesRM->findNumber(util_string::longtostring(transType->getNumber())));
		if ( dbTransactionType.notNull() )   //No need to delete if there is no record
			return dbTransactionTypesRM->deleteRecord(dbTransactionType.get());

		Debug::trace("TransactionTypeManager", "Could not find transaction type in database");

		return 0;
	}


	void TransactionTypeManager::verifyReservedRecords()
	{
		Debug::trace("TransactionTypeManager", "verifying all reserved TransactionTypes ...");

		hfc::MultiLanguageText internationalize;

		Util::CSmartPtr<TransactionType> reservedType = findTransactionType(TransactionType::_PRODUCTION_TYPE_);
		if ( reservedType.null() )
		{
			Debug::trace("TransactionTypeManager", "creating reserved TransactionType " +
					util_string::longtostring(TransactionType::_PRODUCTION_TYPE_) + " (Production)");
			
			reservedType = new TransactionType();

			reservedType->setNumber(TransactionType::_PRODUCTION_TYPE_);
			reservedType->setName1(internationalize("TransactionType.Production"));
			reservedType->setAffectsProduction(TransactionType::AffectsProductionPositively);
			reservedType->setAutomaticChangeUPCDigit(false);
			reservedType->setManualChangeUPCDigit(false);
			reservedType->setUPCChangeDigit("0");
			reservedType->setAllowProductBackDating(false);
			reservedType->setUseBackgroundColor(false);
			reservedType->setBackgroundColor(0);
			reservedType->setTextForLabel("");

			saveRecord(reservedType);
		}
	}


	int TransactionTypeManager::initializeAllTransactionTypesQuery()
	{
		edb::DBTransactionTypesRM* transactionTypesRM = mgrContainer_->getDBTransactionTypesRM();
		if ( !transactionTypesRM )
			return false;

		allTransactionTypesQuery_ = transactionTypesRM->retrieveAll();

		return (int)allTransactionTypesQuery_->size();
	}


	TransactionType* TransactionTypeManager::firstTransactionType()
	{
		if ( (allTransactionTypesQuery_.null() || !allTransactionTypesQuery_->isEnabled()) && !initializeAllTransactionTypesQuery() )
			return 0;

		allTransactionTypesIterator_ = new DBQuery<edb::DBTransactionType>::iterator(allTransactionTypesQuery_->begin());

		return *allTransactionTypesIterator_ != allTransactionTypesQuery_->end() ? createTransactionType(&*(*allTransactionTypesIterator_)) : 0;
	}


	TransactionType* TransactionTypeManager::nextTransactionType()
	{
		if ( allTransactionTypesQuery_.null() || !allTransactionTypesQuery_->isEnabled() || allTransactionTypesIterator_.null() )
			return firstTransactionType();

		++*allTransactionTypesIterator_;

		return *allTransactionTypesIterator_ != allTransactionTypesQuery_->end() ? createTransactionType(&*(*allTransactionTypesIterator_)) : 0;
	}


	int TransactionTypeManager::countAllRecords()
	{
		edb::DBTransactionTypesRM* transactionTypesRM = mgrContainer_->getDBTransactionTypesRM();
		if ( !transactionTypesRM )
			return -1;

		return transactionTypesRM->countAll();
	}


	void TransactionTypeManager::destroyQueriesAndIters()
	{
		/* Destroy first the iterators. */
		allTransactionTypesIterator_ = 0;

		/* Destroy now the queries. */
		allTransactionTypesQuery_ = 0;
	}


	bool TransactionTypeManager::currentTypeAllowsBackdating()
	{
		Util::CSmartPtr<TransactionType> transType = getOperatorModeTransactionType();

		// FT-887 re-read oper mode trans type
		Util::CSmartPtr<TransactionType> currentTransType = findTransactionType( transType->getNumber() );

		if ( currentTransType.null() )
			currentTransType = findTransactionType( TransactionType::_PRODUCTION_TYPE_ );

		if ( currentTransType.notNull() )
			return currentTransType->isAllowProductBackDating();

		return transType.notNull() && transType->isAllowProductBackDating();
	}


//	AWL-52
void TransactionTypeManager::tempSwitchToTransactionType(HobRecordNum transactionTypeNum)
{
	Debug::trace("TransactionTypeManager", "tempSwitchToTransactionType():  transactionTypeNum = " + longtostring(transactionTypeNum));

	Session* session = dynamic_cast<Session*>(mySession_);
	if ( !session ) return;

	//	Do we need to make sure we are in Operate mode here?
	if (mySession_ && session->isInOperateMode() && transactionTypeNum && (transactionTypeNum <= _MaximumRecords_))
	{
		Util::CSmartPtr<TransactionType> currentTransactionType = getOperatorModeTransactionType();
		HobRecordNum currentTransactionTypeNum = 0;
		if (currentTransactionType)
			currentTransactionTypeNum = currentTransactionType->getNumber();
		if (currentTransactionTypeNum == 0)
		{
			Debug::trace("TransactionTypeManager", "tempSwitchToTransactionType():  currentTransactionTypeNum was 0. Set to 1.");
			currentTransactionTypeNum = 1;		//	Assume Production total type.
		}
		Debug::trace("TransactionTypeManager", "tempSwitchToTransactionType():  currentTransactionTypeNum = " + longtostring(currentTransactionTypeNum));
		if (transactionTypeNum != currentTransactionTypeNum)
		{
			//	Load the new total type.
			Util::CSmartPtr<TransactionType> newTransactionType = findTransactionType(transactionTypeNum);
			if (newTransactionType)
			{
				Debug::trace("TransactionTypeManager", "tempSwitchToTransactionType():  We have a new TransactionType # " + longtostring(transactionTypeNum));

				setOperatorModeTransactionType(newTransactionType);
				prevTransTypeNum = currentTransactionTypeNum;

				//	Send the Weigh mode / Total type message back to the Browser with the new Total type.
				SessionMessage updateTotalTypeMsg(SessionDev, SnReadWeighModeTransactionType);
				session->handleMessage(&updateTotalTypeMsg);

				// Notify the ProductManager that the Total type changed.
				SessionMessage notifyMsg(ProductDev, ProductTransactionTypeChanged);
				notifyMsg.setFormatType(BrowserFormat);
				session->handleMessage(&notifyMsg);
			}
		}
	}
}

void TransactionTypeManager::revertToPrevTransactionType()
{
	Debug::trace("TransactionTypeManager", "revertToPrevTransactionType():  prevTransTypeNum = " + longtostring(prevTransTypeNum));

	Session* session = dynamic_cast<Session*>(mySession_);
	if ( !session ) return;

	//	Do we need to make sure we are in Operate mode here?
	if (mySession_ && session->isInOperateMode() && prevTransTypeNum && (prevTransTypeNum <= _MaximumRecords_))
	{
		Util::CSmartPtr<TransactionType> newTransactionType = findTransactionType(prevTransTypeNum);
		if (newTransactionType)
		{
			Debug::trace("TransactionTypeManager", "revertToPrevTransactionType():  We have a new TransactionType # " + longtostring(prevTransTypeNum));

			setOperatorModeTransactionType(newTransactionType);
			prevTransTypeNum = 0;

			//	Send the Weigh mode / Total type message back to the Browser with the new Total type.
			SessionMessage updateTotalTypeMsg(SessionDev, SnReadWeighModeTransactionType);
			session->handleMessage(&updateTotalTypeMsg);

			// Notify the ProductManager that the Total type changed.
			SessionMessage notifyMsg(ProductDev, ProductTransactionTypeChanged);
			notifyMsg.setFormatType(BrowserFormat);
			session->handleMessage(&notifyMsg);
		}
	}
}


}
