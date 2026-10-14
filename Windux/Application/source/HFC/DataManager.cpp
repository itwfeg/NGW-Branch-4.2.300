/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "DataManager.h"
#include "DBAccess.h"
#include "SessionMessage.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int DataManager::_dataType_ = 0;
std::string DataManager::_dataTypeName_("DataManager"); 
#endif

DataManager::DataManager()
	: dbAccess((DBAccess *)NULL)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

}

DataManager::~DataManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	/* At this level, do not delete the DBAccess database; the DataManager does not own the
	 * database pointer.  It is owned by the task (Session for example) to which this DBAccess
	 * belongs.  If the task is also a DataManager, as is the case with BackupRestoreManager,
	 * then delete the DBAccess instance in that class' destructor. 
	 */
}

void DataManager::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


// This method is used to synchronize a record received that will be
// used for synchronization.  The handleSave() methods use it to then pass to processUserChanges().
// When comm or restore/merge bring in a record, the handleRestore() methods use it to then store
// the record in a batch.
// The 'event' parameter should either be the getWriteEvent() or the getDeleteEvent()
void DataManager::serializeForSynchronization(std::string &resultString, HfcObject *hfcObject, UINT8 event)
{
	resultString = "";
	if (!hfcObject)
		return;

	const char * buffer = 0;

	if ( event == InvalidEvent )
		event = getWriteEvent();

	SessionMessage message(getDevice(), event);
	message.setFormatType(SynchronizationTaggedFormat);
	message.setHfcObject(hfcObject);

	/* Change the current object's format to SynchronizationTaggerFormat before serializing
		it. Then restore the old format just in case. */
	int oldFormat = hfcObject->getFormatType();
	hfcObject->setFormatType(SynchronizationTaggedFormat);
	buffer = message.serialize();
	resultString = std::string(buffer, message.getSerializedLength());
	hfcObject->setFormatType(oldFormat);
}
