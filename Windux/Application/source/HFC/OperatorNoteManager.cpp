/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "OperatorNoteManager.h"
#include "OperatorManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "Session.h"
#include "HobOperator.h"
#include "OperatorNote.h"
#include "Configuration.h"
#include "HobList.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "DBAccess.h"
#include "DBException.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperatorNoteManager::_dataType_ = 0;
std::string OperatorNoteManager::_dataTypeName_("OperatorNoteManager"); 
#endif


OperatorNoteManager::OperatorNoteManager()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();
}

OperatorNoteManager::OperatorNoteManager(hfc::HFCManagerContainer *container)
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	init();
	dbAccess = container;
	mgrContainer_ = container;
}

OperatorNoteManager::~OperatorNoteManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


void OperatorNoteManager::init()
{
	mySession = 0;
}


DBSession *OperatorNoteManager::getMySession()
{
	return mySession;
}
void OperatorNoteManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
OperatorNote *OperatorNoteManager::firstOperatorNote()
{
	if (!dbAccess)
		return 0;

	OperatorNote *opNote = OperatorNote::findOperatorNote(dbAccess);
	return opNote;
}


/* Provide a countAll method so the caller doesn't need to know about a dbOperatorNotessRM.
 */
int OperatorNoteManager::countAllOperatorNotes()
{
	if (!dbAccess)
		return -1;

	edb::DBOperatorNotesRM *dbOperatorNotesRM = dbAccess->getDBOperatorNotesRM();
	if (!dbOperatorNotesRM)
		return -1;

	return dbOperatorNotesRM->countAll();
}


void OperatorNoteManager::handleMessage(SessionMessage *msg)
{
	// Valid as long as no process calls this method with a message containing
	// some other device than OperatorNote.
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	Debug::trace("OperatorNoteManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case OpReadNote:
			handleRead();
			break;

		case OpReadNoteForEditing:
			handleReadForEditing();
			break;

		case OpWriteNote:
			handleWrite(dynamic_cast<OperatorNote *> (msg->getHfcObject()));
			break;

		case OpDeleteNote:
			handleDelete();
			break;

		case OpSaveNote:
			handleSave(dynamic_cast<OperatorNote *> (msg->getHfcObject()));
			break;

		case OpRestoreNote:
			handleRestore(msg);
			break;

		default:
			Debug::trace("OperatorNoteManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}

/**************************************************************************************************
 *
 *    Operator Note methods
 *
 **************************************************************************************************/

void OperatorNoteManager::handleRead()
{
	if (!dbAccess)
		return;

	OperatorNote *opNote = OperatorNote::findOperatorNote(dbAccess);
	if (mySession && opNote)
	{
		HobString noteText = opNote->getNoteText();
		noteText.stripFontSequences();
		if (noteText.size())
		{
			HobList opNoteText;
			opNoteText.setQuotingElements(true);
			opNoteText.push_back(noteText);

			Debug::trace("OperatorNoteManager", HobString("handleReadNote() - sending opNote: ") + noteText);

			mySession->addDialogMessage(ErrOperatorDev, OperatorErrOperatorNote, HfcResponse::RESPONSESTYLE_OK, &opNoteText);

			// Now, if there is a currently-logged in operator, set the flag indicating they have seen the note.
			HobOperator *oper = 0;
			if (mgrContainer_->getOperatorManager())
				oper = mgrContainer_->getOperatorManager()->getLoggedInOperator();
			if (oper && oper->getCurrentMode() == HobOperator::CURRENTMODE_OPERATE  && !oper->hasSeenNote())
			{
				Debug::trace("OperatorNoteManager", "Updating the operator since they have seen the note.");
				oper->setSawNote(true);
				if (mgrContainer_->getOperatorManager())
					mgrContainer_->getOperatorManager()->handleSaveRecord(oper);
			}
		}
	}
	if (opNote)
	{
		delete opNote;
		opNote = (OperatorNote *)NULL;
	}
}


void OperatorNoteManager::handleReadForEditing()
{
	if (!dbAccess)
		return;

	OperatorNote *opNote = OperatorNote::findOperatorNote(dbAccess);

	if (!opNote)
		/* Still send the note if it wasn't found in the database.
		 * If it wasn't found, we will just send "", indicating to the Browser that there is no note.
		 */
		opNote = new OperatorNote();

	if (opNote)
	{
		Debug::trace("OperatorNoteManager", HobString("handleReadNoteForEditing(): ") + opNote->getNoteText());

		if (mySession)
		{
			SessionMessage noteMsg(OperatorDev, OpWriteNote);
			noteMsg.setFormatType(BrowserFormat);
			noteMsg.setHfcObject(opNote);
			mySession->send(&noteMsg, Messenger::DEST_UI);
		}

		delete opNote;
		opNote = (OperatorNote *)NULL;
	}
}


void OperatorNoteManager::handleWrite(OperatorNote *UNUSED(note))
{
	/* Write the updated note to the database.
	 * Also, go through all operator records and set the sawNote field to false.
	 */
}


/*  This method just sets the note to "".
 */
void OperatorNoteManager::handleDelete()
{
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::OperatorNotes))
	{
		// batch the delete all command; don't actually do it right now.
		SessionMessage message(getDevice(), getDeleteAllEvent());
		message.setFormatType(SynchronizationTaggedFormat);
		const char * buffer = message.serialize();

		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getDeleteAllEvent(), std::string(buffer, message.getSerializedLength())));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		if (!dbAccess)
			return;

		OperatorNote *opNote = OperatorNote::findOperatorNote(dbAccess);
		if (opNote)
		{
			Debug::trace("OperatorNoteManager", "handleDeleteNote()");
			opNote->setNoteText("");
			opNote->saveRecord();

			// Set the sawNote flag in all Operators to true.
			setAllOperatorsSawNote(true, false);  // second parameter is whether to batch it or not

			delete opNote;
		}
	}
}

void OperatorNoteManager::handleDeleteAll()
{
	handleDelete();
}


void OperatorNoteManager::handleSave(OperatorNote *modifiedOperatorNote, bool shouldBatchIt)
{
	Debug::trace("OperatorNoteManager", "handleSaveNote()");
	if (!modifiedOperatorNote || !dbAccess)
		return;

	OperatorNote *usedOpNote = 0;
	OperatorNote *opNote = OperatorNote::findOperatorNote(dbAccess);
	if (opNote)
	{
		// Only save it if the note has changed.
		if (opNote->getNoteText() != modifiedOperatorNote->getNoteText())
		{
			opNote->updateNote(modifiedOperatorNote);
			opNote->saveRecord();
			usedOpNote = opNote;
			Debug::trace("OperatorNoteManager", HobString("handleSaveNote() - saving new note: ") + opNote->getNoteText() +
														HobString(" length:") + HobString(Integer::toString((int)opNote->getNoteText().size())));

			// Reset the sawNote flag in all Operators to false,
			// unless the new note is "blank", in which case, set the sawNote flag in all Operators to true.
			setAllOperatorsSawNote( ((opNote->getNoteText().empty()) ? true : false), shouldBatchIt);
			Debug::trace("OperatorNoteManager", "finished handleSaveNote()");
		}
	}
	else
	{
		Debug::trace("OperatorNoteManager", HobString("handleSaveNote() - no currently existing OperatorNote found in the db, saving new one: ") + modifiedOperatorNote->getNoteText());

		 // If it wasn't found, just save the modifiedOperatorNote passed in.
		modifiedOperatorNote->setDBAccess(dbAccess);
		modifiedOperatorNote->saveRecord();
		usedOpNote = modifiedOperatorNote;
	}

	if ( shouldBatchIt && usedOpNote && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::OperatorNotes))
	{
		std::string synchSer = "";
		serializeForSynchronization(synchSer, usedOpNote);

		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

		if (proxy)
			proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
				hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
	}

	if (opNote)
		delete opNote;

	Debug::trace("OperatorNoteManager", "leaving handleSaveNote()");
}


void OperatorNoteManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	OperatorNote *restoreOperatorNote = dynamic_cast<OperatorNote *> (msg->getHfcObject());
	if (!restoreOperatorNote)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::OperatorNotes))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		handleSave(restoreOperatorNote, false);
	}
}


void OperatorNoteManager::setAllOperatorsSawNote(bool sawNote, bool shouldBatchIt)
{
	if (!dbAccess)
		return;

	Debug::trace("OperatorNoteManager", HobString("setAllOperatorsSawNote() - setting flag to ") + HobString(Bool::toString(sawNote)));

	DBOperatorsRM *dbOperatorsRM = dbAccess->getDBOperatorsRM();
	if (!dbOperatorsRM)
		return;

	DBQuery<DBOperator> *operatorsQuery = dbOperatorsRM->retrieveAll();
	if (operatorsQuery)
	{
		DBQuery<DBOperator>::const_iterator operatorsIter = operatorsQuery->begin();
		while (operatorsIter != operatorsQuery->end())
		{
			DBOperator *op = new DBOperator(*operatorsIter);
			if (op && op->getSawNote() != sawNote)
			{
				Debug::trace("OperatorNoteManager", HobString("setAllOperatorsSawNote() - updating operator ") +
														 HobString(op->getNumber()));
				op->setSawNote(sawNote);
				dbOperatorsRM->saveRecord(op);

				if ( shouldBatchIt && mgrContainer_->getOperatorManager() &&
					  Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone &&
					  mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Operators))
				{
					HobOperator *oper = new HobOperator(op, dbAccess);
					std::string synchSer = "";

					const char * buffer = 0;

					SessionMessage message(mgrContainer_->getOperatorManager()->getDevice(), mgrContainer_->getOperatorManager()->getWriteEvent());
					message.setFormatType(SynchronizationTaggedFormat);
					message.setHfcObject(oper);

					/* Change the current object's format to SynchronizationTaggerFormat before serializing
						it. Then restore the old format just in case. */
					int oldFormat = oper->getFormatType();
					oper->setFormatType(SynchronizationTaggedFormat);
					buffer = message.serialize();
					oper->setFormatType(oldFormat);

					synchSer = std::string(buffer, message.getSerializedLength());

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(mgrContainer_->getOperatorManager()->getDevice(), mgrContainer_->getOperatorManager()->getWriteEvent(), synchSer)));
				}
			}
			else if (op)
				Debug::trace("OperatorNoteManager", HobString("setAllOperatorsSawNote() - not updating operator ") +
														 HobString(op->getNumber()));
			++operatorsIter;
		}
		delete operatorsQuery;
	}
}
