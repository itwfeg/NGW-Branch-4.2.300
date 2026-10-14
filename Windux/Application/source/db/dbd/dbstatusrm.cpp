#include <string>


#include "edb.h"
#include DBSTATUSRM_INC
#include DBSTATUSSTORAGE_INC
#include DBSTATUS_INC
#include DBQUERY_INC

#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
	using namespace util_string;
	using namespace util_thread;
#endif



DBStatusRM::DBStatusRM(DBManager* dbm, DBRecordManager::DBRecordType recordType)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(new DBStatusStorageRM())
{
	enabled_ = true;
}


DBStatusRM::~DBStatusRM()
{
	enabled_ = false;

	if ( decorated_ )
		delete decorated_;
}


/*
DBQuery<DBStatus>* DBStatusRM::retrieveAll()
{
	if ( enabled_ )
		return new DBQuery<DBStatus>(decorated_->retrieveAll(), *this);
	else
		return 0;
}
*/


DBStatus* DBStatusRM::findName(const string& name)
{
	if ( enabled_ )
	{
		edb_adapter::DBStatusI* rec = decorated_->findName(name);

		if ( rec )
			return new DBStatus(rec);
	}

	return 0;
}


DBStatus* DBStatusRM::createRecord()
{
	if ( enabled_ )
	{
		edb_adapter::DBStatusI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBStatus* r = new DBStatus(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBStatusRM::saveRecord(const DBStatus* rec)
{
	if ( enabled_ && rec )
		decorated_->saveRecord(rec->decorated_);
}


int DBStatusRM::countAll() const
{
	if ( enabled_ )
		return decorated_->countAll();
	else
		return 0;
}


int DBStatusRM::calculateAverageSpace() const
{
	if ( enabled_ )
		return decorated_->calculateAverageSpace();
	else
		return 0;
}


bool DBStatusRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBStatusRM::showDBAccesses(const std::string& dc)
{
	std::string debuggingCategory(dc);

	if ( debuggingCategory.empty() )
		debuggingCategory.assign("database.show");

	Debug::trace(debuggingCategory, 20, "   Current accesses:");

	Lock< Mutex<> > lock(_accessContainerMutex_);
	accessContainer_type::const_iterator iter = _accessContainer_.begin();
	while ( iter != _accessContainer_.end() )
	{
		const DBAccessEntity* entity = iter->second->ae_;
		std::string accessEntity("no access entity");

		if ( entity )
		{
			DBAccessEntityType entityType = iter->second->aeType_;

			switch ( entityType )
			{
				case DBAccessEntity::DBAccessEntityRecordManager:
					accessEntity.assign("Record Manager = 0x");
					break;
				case DBAccessEntity::DBAccessEntityQuery:
					accessEntity.assign("Query = 0x");
					break;
				default:
					accessEntity.assign("Unknown Entity = 0x");
					break;
			}

			accessEntity.append(ptrtostring(entity));
		}

		Debug::trace(debuggingCategory, 20, "      (id = " + inttostring(iter->first) + ") Access Type '" +
			getAccessTypeText(iter->second->accessType_) + "' on table '" + getRecordTypeText(iter->second->record_) +
			"' (DBManager = 0x" + ptrtostring(iter->second->dbm_) + ", " + accessEntity + ")");

		++iter;
	}
}



const std::string DBStatusRM::_STATUS_STATE_                      ("State");
const std::string DBStatusRM::_STATUS_OPERATION_                  ("Operation");
const std::string DBStatusRM::_STATUS_OPERATION_STARTED_          ("OperStart");
const std::string DBStatusRM::_STATUS_ACTIVITY_                   ("Activity");
const std::string DBStatusRM::_STATUS_LAST_BACKUP_STARTED_        ("BckStart");
const std::string DBStatusRM::_STATUS_LAST_BACKUP_COMPLETED_      ("BckEnd");
const std::string DBStatusRM::_STATUS_LAST_COMPRESS_STARTED_      ("CompStart");
const std::string DBStatusRM::_STATUS_LAST_COMPRESS_COMPLETED_    ("CompEnd");

const std::string DBStatusRM::_STATUS_STATE_CLOSED_               ("Closed");
const std::string DBStatusRM::_STATUS_STATE_OPEN_                 ("Open");

const std::string DBStatusRM::_STATUS_OPERATION_NONE_             ("None");
const std::string DBStatusRM::_STATUS_OPERATION_CREATE_           ("Create");
const std::string DBStatusRM::_STATUS_OPERATION_OPEN_             ("Open");
const std::string DBStatusRM::_STATUS_OPERATION_COMPRESS_         ("Compress");
const std::string DBStatusRM::_STATUS_OPERATION_COPY_             ("Copy");
const std::string DBStatusRM::_STATUS_OPERATION_BACKUP_           ("Backup");
const std::string DBStatusRM::_STATUS_OPERATION_CLOSE_            ("Close");
const std::string DBStatusRM::_STATUS_OPERATION_DELETE_           ("Delete");
const std::string DBStatusRM::_STATUS_OPERATION_CHECK_            ("Check");
const std::string DBStatusRM::_STATUS_OPERATION_LANGUAGECHANGE_   ("LanguageChange");

const std::string DBStatusRM::_STATUS_ACTIVITY_BUSY_              ("Busy");
const std::string DBStatusRM::_STATUS_ACTIVITY_IDLE_              ("Idle");

int DBStatusRM::_busyCounter_ = 0;
bool DBStatusRM::_consecutiveBusies_ = false;
util_thread::Mutex<> DBStatusRM::_busyCounterMutex_ = util_thread::Mutex<>();

DBStatusRM::accessContainer_type DBStatusRM::_accessContainer_;
util_thread::Mutex<> DBStatusRM::_accessContainerMutex_;

const std::string DBStatusRM::_dbAccessDatabaseOperationText_        ("Database Operation");
const std::string DBStatusRM::_dbAccessOpenTableText_                ("Open Table");
const std::string DBStatusRM::_dbAccessCreateRecordText_             ("Create Record");
const std::string DBStatusRM::_dbAccessSemiautoIncrementText_        ("Semiauto Increment");
const std::string DBStatusRM::_dbAccessFindRecordText_               ("Find Record");
const std::string DBStatusRM::_dbAccessRetrieveRecordsText_          ("Retrieve Records");
const std::string DBStatusRM::_dbAccessCountAllRecordsText_          ("Count All Records");
const std::string DBStatusRM::_dbAccessCalculateAverageSpaceText_    ("Calculate Average Space");
const std::string DBStatusRM::_dbAccessSaveRecordText_               ("Save Record");
const std::string DBStatusRM::_dbAccessDeleteRecordText_             ("Delete Record");
const std::string DBStatusRM::_dbAccessDeleteAllRecordsText_         ("Delete All Records");
const std::string DBStatusRM::_dbAccessCloseTableText_               ("Close Table");
const std::string DBStatusRM::_dbAccessQueryBeginText_               ("Query Begin");
const std::string DBStatusRM::_dbAccessQueryFirstText_               ("Query First");
const std::string DBStatusRM::_dbAccessQueryReverseBeginText_        ("Query Reverse Begin");
const std::string DBStatusRM::_dbAccessQueryPreviousText_            ("Query Previous");
const std::string DBStatusRM::_dbAccessQueryNextText_                ("Query Next");
const std::string DBStatusRM::_dbAccessQueryLastText_                ("Query Last");
const std::string DBStatusRM::_dbAccessQuerySizeText_                ("Query Size");
const std::string DBStatusRM::_dbAccessQueryEmptyText_               ("Query Empty");
const std::string DBStatusRM::_dbAccessQueryDisposeText_             ("Query Dispose");
const std::string DBStatusRM::_dbAccessInvalidText_                  ("Invalid Access");

const std::string DBStatusRM::_recordTypeWholeDB_                              ("Whole DB");
const std::string DBStatusRM::_recordTypeBatches_                              ("Batches");
const std::string DBStatusRM::_recordTypeCategories_                           ("Categories");
const std::string DBStatusRM::_recordTypeCategoryDetailedStatistics_           ("Category Detailed Statistics");
const std::string DBStatusRM::_recordTypeCategoryStatistics_                   ("Category Statistics");
const std::string DBStatusRM::_recordTypeClasses_                              ("Classes");
const std::string DBStatusRM::_recordTypeConfigurations_                       ("Configurations");
const std::string DBStatusRM::_recordTypeCoolCategories_                       ("COOL Categories");
const std::string DBStatusRM::_recordTypeCoolCategoryCountries_                ("COOL Category Countries");
const std::string DBStatusRM::_recordTypeCoolCategoryPreTexts_                 ("COOL Category Pre-Texts");
const std::string DBStatusRM::_recordTypeCoolCategoryProductionSteps_          ("COOL Category Production Steps");
const std::string DBStatusRM::_recordTypeCoolCountries_                        ("COOL Countries");
const std::string DBStatusRM::_recordTypeCoolLists_                            ("COOL Lists");
const std::string DBStatusRM::_recordTypeCoolListTexts_                        ("COOL List Texts");
const std::string DBStatusRM::_recordTypeCoolPreTexts_                         ("COOL Pre-Texts");
const std::string DBStatusRM::_recordTypeCoolProductionSteps_                  ("COOL Production Steps");
const std::string DBStatusRM::_recordTypeCoolTexts_                            ("COOL Texts");
const std::string DBStatusRM::_recordTypeCuttingTests_                         ("Cutting Tests");
const std::string DBStatusRM::_recordTypeDevices_                              ("Devices");
const std::string DBStatusRM::_recordTypeEmailAlerts_                          ("Email Alerts");
const std::string DBStatusRM::_recordTypeExpirationLabels_                     ("Expiration Labels");
const std::string DBStatusRM::_recordTypeFlashFiles_                           ("Flash Files");
const std::string DBStatusRM::_recordTypeLabels_                               ("Labels");
const std::string DBStatusRM::_recordTypeLabelTexts_                           ("Label Texts");
const std::string DBStatusRM::_recordTypeMarquees_                             ("Marquees");
const std::string DBStatusRM::_recordTypeMessages_                             ("Messages");
const std::string DBStatusRM::_recordTypeNetworkErrors_                        ("Network Errors");
const std::string DBStatusRM::_recordTypeNotes_                                ("Notes");
const std::string DBStatusRM::_recordTypeNutritions_                           ("Nutritions");
const std::string DBStatusRM::_recordTypeOperators_                            ("Operators");
const std::string DBStatusRM::_recordTypeOperatorNotes_                        ("Operator Notes");
const std::string DBStatusRM::_recordTypePresentationMethods_                  ("Presentation Methods");
const std::string DBStatusRM::_recordTypePresentationMethodDetailedStatistics_ ("Presentation Method Detailed Statistics");
const std::string DBStatusRM::_recordTypePresentationMethodStatistics_         ("Presentation Method Statistics");
const std::string DBStatusRM::_recordTypeProducts_                             ("Products");
const std::string DBStatusRM::_recordTypeProductDefaults_                      ("Product Defaults");
const std::string DBStatusRM::_recordTypeProductDetailedStatistics_            ("Product Detailed Statistics");
const std::string DBStatusRM::_recordTypeProductStatistics_                    ("Product Statistics");
const std::string DBStatusRM::_recordTypeROMLabels_                            ("ROM Labels");
const std::string DBStatusRM::_recordTypeScales_                               ("Scales");
const std::string DBStatusRM::_recordTypeSoftKeys_                             ("Soft Keys");
const std::string DBStatusRM::_recordTypeSoftKeyFeatures_                      ("Soft Key Features");
const std::string DBStatusRM::_recordTypeSoftKeyGroups_                        ("Soft Key Groups");
const std::string DBStatusRM::_recordTypeSoftKeyStyles_                        ("Soft Key Styles");
const std::string DBStatusRM::_recordTypeSpecialPrograms_                      ("Special Programs");
const std::string DBStatusRM::_recordTypeStatus_                               ("Status");
const std::string DBStatusRM::_recordTypeStreamingGraphics_                    ("Streaming Graphics");
const std::string DBStatusRM::_recordTypeSynchronizationChanges_               ("Synchronization Changes");
const std::string DBStatusRM::_recordTypeTexts_                                ("Texts");
const std::string DBStatusRM::_recordTypeTransactions_                         ("Transactions");
const std::string DBStatusRM::_recordTypeTransactionTypes_                     ("TransactionTypes");
const std::string DBStatusRM::_recordTypeInvalidType_                          ("Invalid");
const std::string DBStatusRM::_recordTypeWrapperErrorHistory_                  ("WrapperErrorHistory");
const std::string DBStatusRM::_recordTypeWrapperKnife_                         ("WrapperKnifeTimes"); 	



int DBStatusRM::setBusyStatus(DBManager* dbm, DBAccessEntity::DBAccessType access, DBRecordManager::DBRecordType record, const DBAccessEntity* ae)
{
	int busyCounter = -1;

	{
		Lock< Mutex<> > lock(_busyCounterMutex_); 

		// If, after incrementing, the counter is set to 1, then we have transitioned
		// from idle to busy (change the Activity status accordingly).
		if ( !(_busyCounter_++) ) 
			setStatus(_STATUS_ACTIVITY_, _STATUS_ACTIVITY_BUSY_); 

		// Show debug trace if we set busy status more than once consecutively
		if ( _consecutiveBusies_ )
			busyCounter = _busyCounter_;
		else
			_consecutiveBusies_ = true;
	}

	if ( busyCounter != -1 )
		Debug::trace("database.status.consecutiveBusies", 28, 
				"Got another busy (" + util_string::inttostring(busyCounter) + ").");

	int accessId = 0;
	Util::CSmartPtr<DBAccess> dbaccess(new DBAccess(dbm, access, record, ae));
	{
//		int counter = 0;
		Lock< Mutex<> > lock(_accessContainerMutex_);

		do 
		{
			accessId = DBAccess::getAccessId();
//			counter ++;
//			if ( counter > 10 )
//				Debug::trace("database.infiniteLoop", "infinite loop in setBusyStatus");
		} while ( !_accessContainer_.insert(make_pair(accessId, dbaccess)).second );
	}

	return accessId;
}

void DBStatusRM::setIdleStatus(int accessId)
{
	{
		Lock< Mutex<> > lock(_busyCounterMutex_);

		// If, after decrementing, the counter is set to 0, then we have transitioned
		// from busy to idle (change the Activity status accordingly).
		if ( _busyCounter_-- <= 1 ) 
		{
			setStatus(_STATUS_ACTIVITY_, _STATUS_ACTIVITY_IDLE_); 
			_busyCounter_ = 0; 
		}

		_consecutiveBusies_ = false;
	}

	bool present = false;
	{
		Lock< Mutex<> > lock(_accessContainerMutex_);
		if ( _accessContainer_.erase(accessId) )
			present = true;
	}

	if ( !present )
		Debug::trace("database", "Failed to erase a dbAccess entry for accessId = " + util_string::inttostring(accessId));
}


const std::string& DBStatusRM::getAccessTypeText(DBAccessEntity::DBAccessType accessType)
{
	switch ( accessType )
	{
		case DBAccessEntity::DBAccessDatabaseOperation:          return _dbAccessDatabaseOperationText_;
		case DBAccessEntity::DBAccessOpenTable:                  return _dbAccessOpenTableText_;
		case DBAccessEntity::DBAccessCreateRecord:               return _dbAccessCreateRecordText_;
		case DBAccessEntity::DBAccessSemiautoIncrement:          return _dbAccessSemiautoIncrementText_;
		case DBAccessEntity::DBAccessFindRecord:                 return _dbAccessFindRecordText_;
		case DBAccessEntity::DBAccessRetrieveRecords:            return _dbAccessRetrieveRecordsText_;
		case DBAccessEntity::DBAccessCountAllRecords:            return _dbAccessCountAllRecordsText_;
		case DBAccessEntity::DBAccessCalculateAverageSpace:      return _dbAccessCalculateAverageSpaceText_;
		case DBAccessEntity::DBAccessSaveRecord:                 return _dbAccessSaveRecordText_;
		case DBAccessEntity::DBAccessDeleteRecord:               return _dbAccessDeleteRecordText_;
		case DBAccessEntity::DBAccessDeleteAllRecords:           return _dbAccessDeleteAllRecordsText_;
		case DBAccessEntity::DBAccessCloseTable:                 return _dbAccessCloseTableText_;

		case DBAccessEntity::DBAccessQueryBegin:                 return _dbAccessQueryBeginText_;
		case DBAccessEntity::DBAccessQueryFirst:                 return _dbAccessQueryFirstText_;
		case DBAccessEntity::DBAccessQueryReverseBegin:          return _dbAccessQueryReverseBeginText_;
		case DBAccessEntity::DBAccessQueryPrevious:              return _dbAccessQueryPreviousText_;
		case DBAccessEntity::DBAccessQueryNext:                  return _dbAccessQueryNextText_;
		case DBAccessEntity::DBAccessQueryLast:                  return _dbAccessQueryLastText_;
		case DBAccessEntity::DBAccessQuerySize:                  return _dbAccessQuerySizeText_;
		case DBAccessEntity::DBAccessQueryEmpty:                 return _dbAccessQueryEmptyText_;
		case DBAccessEntity::DBAccessQueryDispose:               return _dbAccessQueryDisposeText_;

		default:                                                 return _dbAccessInvalidText_;
	}
}


const std::string& DBStatusRM::getRecordTypeText(DBRecordManager::DBRecordType recordType)
{
	switch ( recordType )
	{
		case DBRecordManager::WholeDB:                               return _recordTypeWholeDB_;

		case DBRecordManager::Batches:                               return _recordTypeBatches_;
		case DBRecordManager::Categories:                            return _recordTypeCategories_;
		case DBRecordManager::CategoryDetailedStatistics:            return _recordTypeCategoryDetailedStatistics_;
		case DBRecordManager::CategoryStatistics:                    return _recordTypeCategoryStatistics_;
		case DBRecordManager::Classes:                               return _recordTypeClasses_;
		case DBRecordManager::Configurations:                        return _recordTypeConfigurations_;
		case DBRecordManager::CoolCategories:                        return _recordTypeCoolCategories_;
		case DBRecordManager::CoolCategoryCountries:                 return _recordTypeCoolCategoryCountries_;
		case DBRecordManager::CoolCategoryPreTexts:                  return _recordTypeCoolCategoryPreTexts_;
		case DBRecordManager::CoolCategoryProductionSteps:           return _recordTypeCoolCategoryProductionSteps_;
		case DBRecordManager::CoolCountries:                         return _recordTypeCoolCountries_;
		case DBRecordManager::CoolLists:                             return _recordTypeCoolLists_;
		case DBRecordManager::CoolListTexts:                         return _recordTypeCoolListTexts_;
		case DBRecordManager::CoolPreTexts:                          return _recordTypeCoolPreTexts_;
		case DBRecordManager::CoolProductionSteps:                   return _recordTypeCoolProductionSteps_;
		case DBRecordManager::CoolTexts:                             return _recordTypeCoolTexts_;
		case DBRecordManager::CuttingTests:                          return _recordTypeCuttingTests_;
		case DBRecordManager::Devices:                               return _recordTypeDevices_;
		case DBRecordManager::EmailAlerts:                           return _recordTypeEmailAlerts_;
		case DBRecordManager::ExpirationLabels:                      return _recordTypeExpirationLabels_;
		case DBRecordManager::FlashFiles:                            return _recordTypeFlashFiles_;
		case DBRecordManager::Labels:                                return _recordTypeLabels_;
		case DBRecordManager::LabelTexts:                            return _recordTypeLabelTexts_;
		case DBRecordManager::Marquees:                              return _recordTypeMarquees_;
		case DBRecordManager::Messages:                              return _recordTypeMessages_;
		case DBRecordManager::NetworkErrors:                         return _recordTypeNetworkErrors_;
		case DBRecordManager::Notes:                                 return _recordTypeNotes_;
		case DBRecordManager::Nutritions:                            return _recordTypeNutritions_;
		case DBRecordManager::Operators:                             return _recordTypeOperators_;
		case DBRecordManager::OperatorNotes:                         return _recordTypeOperatorNotes_;
		case DBRecordManager::PresentationMethods:                   return _recordTypePresentationMethods_;
		case DBRecordManager::PresentationMethodDetailedStatistics:  return _recordTypePresentationMethodDetailedStatistics_;
		case DBRecordManager::PresentationMethodStatistics:          return _recordTypePresentationMethodStatistics_;
		case DBRecordManager::Products:                              return _recordTypeProducts_;
		case DBRecordManager::ProductDefaults:                       return _recordTypeProductDefaults_;
		case DBRecordManager::ProductDetailedStatistics:             return _recordTypeProductDetailedStatistics_;
		case DBRecordManager::ProductStatistics:                     return _recordTypeProductStatistics_;
		case DBRecordManager::ROMLabels:                             return _recordTypeROMLabels_;
		case DBRecordManager::Scales:                                return _recordTypeScales_;
		case DBRecordManager::SoftKeys:                              return _recordTypeSoftKeys_;
		case DBRecordManager::SoftKeyFeatures:                       return _recordTypeSoftKeyFeatures_;
		case DBRecordManager::SoftKeyGroups:                         return _recordTypeSoftKeyGroups_;
		case DBRecordManager::SoftKeyStyles:                         return _recordTypeSoftKeyStyles_;
		case DBRecordManager::SpecialPrograms:                       return _recordTypeSpecialPrograms_;
		case DBRecordManager::Status:                                return _recordTypeStatus_;
		case DBRecordManager::StreamingGraphics:                     return _recordTypeStreamingGraphics_;
		case DBRecordManager::SynchronizationChanges:                return _recordTypeSynchronizationChanges_;
		case DBRecordManager::Texts:                                 return _recordTypeTexts_;
		case DBRecordManager::Transactions:                          return _recordTypeTransactions_;
		case DBRecordManager::TransactionTypes:                      return _recordTypeTransactionTypes_;
		case DBRecordManager::WrapperErrorHistory:                   return _recordTypeWrapperErrorHistory_;		
		case DBRecordManager::WrapperKnife:                          return _recordTypeWrapperKnife_;

		default:                                                     return _recordTypeInvalidType_;
	}
}



const std::string DBStatusRM::DESCRIPTOR = "STATUS";



bool DBStatusRM::registerQ(DBQuery<DBStatus>* query)
{
	return qsRegistry_.add(query);
}


bool DBStatusRM::deregisterQ(DBQuery<DBStatus>* query)
{
	return qsRegistry_.remove(query);
}


int DBStatusRM::DBAccess::getAccessId()
{
	static int id = 0; 
	return ++id;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
