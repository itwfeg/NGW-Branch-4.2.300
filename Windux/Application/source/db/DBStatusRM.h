#ifndef DBSTATUSRM_H
#define DBSTATUSRM_H


#include <string>
#include <map>


#include "edb.h"
#include DBRECORDMANAGER_INC
#include DBSTATUSSTORAGERM_INC
#include REGISTRY_INC
#include MUTEX_INC
#include DATETIME_INC

#include "CSmartPtr.h"


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBStatus;
	template <class DBEntity> class DBQuery;


	// DBStatusRM manages the database status (state) under the interface of a
	// DBRecordManager. This class keeps track of several aspects of the database
	// as a whole:
	// State (Open/Closed): when the database is first initialized (by calling
	//    DBManager::getInstance) its state is set to closed; from then on, the
	//    database can be opened and closed iteratively and its state is tracked
	//    here.
	// Activity (Busy/Idle): any database operation or access makes the database
	//    busy; otherwise the database is idle.
	// Busy Counter (counter): the database component is multithreaded (multiple
	//    threads can access it at the same time); this counter reflects the number
	//    of simultaneous threads that make the database busy at any given time.
	class DBStatusRM : public DBRecordManager
	{
		public:

			// --- Types and subtypes ---
			typedef DBStatus record_type;
			typedef DBQuery<DBStatus> query_type;
			typedef std::map<std::string, std::string> statusContainerType;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBStatusRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
//			DBQuery<DBStatus>* retrieveAll();

			// findName: a null return means "unable to find the Status record".
			DBStatus* findName(const std::string& name);
			DBStatus* find(const std::string& key) { return findName(key); }

			DBStatus* createRecord();
			void saveRecord(const DBStatus* rec);
			int deleteAll() const { return 0; }
			int countAll() const;
			int calculateAverageSpace() const;
			bool execute(const std::string sql) const;

			static void showDBAccesses(const std::string& debuggingCategory);


			// --- Constants ---
			// Variables for which we keep status
			static const std::string _STATUS_STATE_;
			static const std::string _STATUS_OPERATION_;
			static const std::string _STATUS_OPERATION_STARTED_;
			static const std::string _STATUS_ACTIVITY_;
			static const std::string _STATUS_LAST_BACKUP_STARTED_;
			static const std::string _STATUS_LAST_BACKUP_COMPLETED_;
			static const std::string _STATUS_LAST_COMPRESS_STARTED_;
			static const std::string _STATUS_LAST_COMPRESS_COMPLETED_;

			// Possible values for variable STATE
			static const std::string _STATUS_STATE_CLOSED_;
			static const std::string _STATUS_STATE_OPEN_;

			// Possible values for variable OPERATION
			static const std::string _STATUS_OPERATION_NONE_;
			static const std::string _STATUS_OPERATION_CREATE_;
			static const std::string _STATUS_OPERATION_OPEN_;
			static const std::string _STATUS_OPERATION_COMPRESS_;
			static const std::string _STATUS_OPERATION_COPY_;
			static const std::string _STATUS_OPERATION_BACKUP_;
			static const std::string _STATUS_OPERATION_CLOSE_;
			static const std::string _STATUS_OPERATION_DELETE_;
			static const std::string _STATUS_OPERATION_CHECK_;
			static const std::string _STATUS_OPERATION_LANGUAGECHANGE_;

			// Possible values for variable ACTIVITY
			static const std::string _STATUS_ACTIVITY_BUSY_;
			static const std::string _STATUS_ACTIVITY_IDLE_;


			static const std::string DESCRIPTOR;


		private:

			// --- Types and subtypes ---
			// Functor used to automatically set busy and idle status
			friend class BusyIdleFunctor;  /* (SJJC port 6/19/06) */
			class BusyIdleFunctor
			{
				public: 
					BusyIdleFunctor(DBManager* dbm, DBAccessEntity::DBAccessType access, DBRecordManager::DBRecordType record)
						{ accessId_ = DBStatusRM::setBusyStatus(dbm, access, record); }
					BusyIdleFunctor(const DBAccessEntity* ae, DBAccessEntity::DBAccessType access)
						{ accessId_ = DBStatusRM::setBusyStatus(ae, access); }
					~BusyIdleFunctor() { DBStatusRM::setIdleStatus(accessId_); }

				private:
					int accessId_;
			};


			// Functor used to automatically set current operation and move from
			// busy to idle
			friend class OperationFunctor;  /* (SJJC port 6/19/06) */
			class OperationFunctor
			{
				public:
					OperationFunctor(DBManager* dbm, const std::string& operation) { accessId_ = DBStatusRM::setOperationStatus(dbm, operation); }
					~OperationFunctor() { DBStatusRM::setOperationNoneStatus(accessId_); }

				private:
					int accessId_;
			};


			class DBAccess
			{
				public:
					DBAccess(DBManager* dbm, DBAccessEntity::DBAccessType access, DBAccessEntity::DBRecordType record, const DBAccessEntity* ae)
						: dbm_(dbm), accessType_(access), record_(record), ae_(ae), aeType_(ae ? ae->getAccessEntityType() : DBAccessEntityInvalid) {}
					DBAccess(const DBAccess& rhs) {copy(rhs);}

					DBAccess& operator=(const DBAccess& rhs) {copy(rhs); return *this;}

					static int getAccessId();

					DBManager* dbm_;
					DBAccessEntity::DBAccessType accessType_;
					DBAccessEntity::DBRecordType record_;

					const DBAccessEntity* ae_;
					DBAccessEntityType aeType_;

				private:
					void copy(const DBAccess& rhs) {
						dbm_ = rhs.dbm_;
						accessType_ = rhs.accessType_;
						record_ = rhs.record_;
						ae_ = rhs.ae_;
						aeType_ = rhs.aeType_;
					}
			};


			typedef std::map<int, Util::CSmartPtr<DBAccess> > accessContainer_type;


			// --- Data members ---
			DBManager* dbm_;
			DBRecordManager::DBRecordType recordType_;
			bool enabled_;
			util_registry::Registry<DBQuery<DBStatus> > qsRegistry_;
			edb_adapter::DBStatusRMI* decorated_;


			// --- Class data members ---
			static int _busyCounter_;
			static bool _consecutiveBusies_;
			static util_thread::Mutex<> _busyCounterMutex_;

			static accessContainer_type _accessContainer_;
			static util_thread::Mutex<> _accessContainerMutex_;


			// --- Constructors and destructor ---
			DBStatusRM(DBManager* dbm, DBRecordManager::DBRecordType recordType);
			// Not implemented methods (linker will complain if called)
			DBStatusRM(const DBStatusRM& rm);            // Copy constructor
			DBStatusRM operator=(const DBStatusRM& rhs);  // Assignment operator


			// Registry methods used by queries
			bool registerQ(DBQuery<DBStatus>* query);
			bool deregisterQ(DBQuery<DBStatus>* query);

			// Methods to access status
			static std::string* getStatus(const std::string& field) { return DBStatusStorageRM::getStatus(field); }
			static void setStatus(const std::string& field, const std::string& value) { DBStatusStorageRM::setStatus(field, value); }
			static void setStatusNull(const std::string& field) { DBStatusStorageRM::setStatusNull(field); }
			static void copyStatus(const std::string& destField, const std::string& srcField) { DBStatusStorageRM::copyStatus(destField, srcField); }
			static void setNowStatus(const std::string& field) { setStatus(field, util_time::DateTime().operator std::string()); }

			// Methods for the State status
			static void setStateStatus(const std::string& value) { setStatus(_STATUS_STATE_, value); }

			// Methods for the Activity status
			static int setBusyStatus(const DBAccessEntity* ae, DBAccessEntity::DBAccessType access) { return setBusyStatus(ae->getDBManager(), access, ae->getRecordType(), ae); }
			static int setBusyStatus(DBManager* dbm, DBAccessEntity::DBAccessType access, DBRecordManager::DBRecordType record = DBRecordManager::WholeDB, const DBAccessEntity* ae = 0);
			static void setIdleStatus(int accessId);
			static int getBusyCounter()
					{ util_thread::Lock< util_thread::Mutex<> > lock(_busyCounterMutex_);
					  return _busyCounter_; } /* (SJJC port 6/19/06) */

			// Methods for the Operation status
			static int setOperationStatus(DBManager* dbm, const std::string& value)
					{ int accessId = setBusyStatus(dbm, DBAccessEntity::DBAccessDatabaseOperation);
					  setStatus(_STATUS_OPERATION_, value); 
					  setStatus(_STATUS_OPERATION_STARTED_, util_time::DateTime().operator std::string()); return accessId; }
			static void setOperationNoneStatus(int accessId) { setStatus(_STATUS_OPERATION_, _STATUS_OPERATION_NONE_); setIdleStatus(accessId); }

			// Convenience Methods to access functors
			static BusyIdleFunctor getBusyIdleFunctor(DBManager* dbm, DBAccessEntity::DBAccessType access, DBRecordManager::DBRecordType record) { return BusyIdleFunctor(dbm, access, record); }
			static BusyIdleFunctor getBusyIdleFunctor(const DBAccessEntity* ae, DBAccessEntity::DBAccessType access) { return BusyIdleFunctor(ae, access); }
			static OperationFunctor getOperationFunctor(DBManager* dbm, const std::string& operation) { return OperationFunctor(dbm, operation); }


			static const std::string _dbAccessDatabaseOperationText_;
			static const std::string _dbAccessOpenTableText_;
			static const std::string _dbAccessCreateRecordText_;
			static const std::string _dbAccessSemiautoIncrementText_;
			static const std::string _dbAccessFindRecordText_;
			static const std::string _dbAccessRetrieveRecordsText_;
			static const std::string _dbAccessCountAllRecordsText_;
			static const std::string _dbAccessCalculateAverageSpaceText_;
			static const std::string _dbAccessSaveRecordText_;
			static const std::string _dbAccessDeleteRecordText_;
			static const std::string _dbAccessDeleteAllRecordsText_;
			static const std::string _dbAccessCloseTableText_;
			static const std::string _dbAccessQueryBeginText_;
			static const std::string _dbAccessQueryFirstText_;
			static const std::string _dbAccessQueryReverseBeginText_;
			static const std::string _dbAccessQueryPreviousText_;
			static const std::string _dbAccessQueryNextText_;
			static const std::string _dbAccessQueryLastText_;
			static const std::string _dbAccessQuerySizeText_;
			static const std::string _dbAccessQueryEmptyText_;
			static const std::string _dbAccessQueryDisposeText_;
			static const std::string _dbAccessInvalidText_;

			static const std::string& getAccessTypeText(DBAccessEntity::DBAccessType accessType);

			static const std::string _recordTypeWholeDB_;
			static const std::string _recordTypeBatches_;
			static const std::string _recordTypeBarcodeScanners_;
			static const std::string _recordTypeCategories_;
			static const std::string _recordTypeCategoryDetailedStatistics_;
			static const std::string _recordTypeCategoryStatistics_;
			static const std::string _recordTypeClasses_;
			static const std::string _recordTypeConfigurations_;
			static const std::string _recordTypeCoolCategories_;
			static const std::string _recordTypeCoolCategoryCountries_;
			static const std::string _recordTypeCoolCategoryPreTexts_;
			static const std::string _recordTypeCoolCategoryProductionSteps_;
			static const std::string _recordTypeCoolCountries_;
			static const std::string _recordTypeCoolLists_;
			static const std::string _recordTypeCoolListTexts_;
			static const std::string _recordTypeCoolPreTexts_;
			static const std::string _recordTypeCoolProductionSteps_;
			static const std::string _recordTypeCoolTexts_;
			static const std::string _recordTypeCuttingTests_;
			static const std::string _recordTypeDepts_;
			static const std::string _recordTypeDevices_;
			static const std::string _recordTypeEmailAlerts_;
			static const std::string _recordTypeExpirationLabels_;
			static const std::string _recordTypeFlashFiles_;
			static const std::string _recordTypeLabels_;
			static const std::string _recordTypeLabelTexts_;
			static const std::string _recordTypeMarquees_;
			static const std::string _recordTypeMessages_;
			static const std::string _recordTypeNetworkErrors_;
			static const std::string _recordTypeNotes_;
			static const std::string _recordTypeNutritions_;
			static const std::string _recordTypeOperators_;
			static const std::string _recordTypeOperatorNotes_;
			static const std::string _recordTypePresentationMethods_;
			static const std::string _recordTypePresentationMethodDetailedStatistics_;
			static const std::string _recordTypePresentationMethodStatistics_;
			static const std::string _recordTypeProducts_;
			static const std::string _recordTypeProductDefaults_;
			static const std::string _recordTypeProductDetailedStatistics_;
			static const std::string _recordTypeProductStatistics_;
			static const std::string _recordTypeProductTexts_;
			static const std::string _recordTypeProductTextCfgs_;
			static const std::string _recordTypeRecalls_;
			static const std::string _recordTypeROMLabels_;
			static const std::string _recordTypeScales_;
			static const std::string _recordTypeSoftKeys_;
			static const std::string _recordTypeSoftKeyFeatures_;
			static const std::string _recordTypeSoftKeyGroups_;
			static const std::string _recordTypeSoftKeyStyles_;
			static const std::string _recordTypeSpecialPrograms_;
			static const std::string _recordTypeStatus_;
			static const std::string _recordTypeStreamingGraphics_;
			static const std::string _recordTypeSynchronizationChanges_;
			static const std::string _recordTypeTexts_;
			static const std::string _recordTypeTransactions_;
			static const std::string _recordTypeTransactionTypes_;
			static const std::string _recordTypeWrapperErrorHistory_;
			static const std::string _recordTypeWrapperKnife_;
			static const std::string _recordTypeInvalidType_;

			static const std::string& getRecordTypeText(DBRecordManager::DBRecordType recordType);


			// --- Friend classes ---
			friend class DBManager;
			friend class DBManagerSingleton;
			friend class DBFactory;

			friend class DBBatchesRM;
			friend class DBBarcodeScannersRM;
			friend class DBCategoriesRM;
			friend class DBCategoryStatisticsRM;
			friend class DBCategoryDetailedStatisticsRM;
			friend class DBClassesRM;
			friend class DBConfigurationsRM;
			friend class DBCoolCategoriesRM;
			friend class DBCoolCategoryCountriesRM;
			friend class DBCoolCategoryPreTextsRM;
			friend class DBCoolCategoryProductionStepsRM;
			friend class DBCoolCountriesRM;
			friend class DBCoolListsRM;
			friend class DBCoolListTextsRM;
			friend class DBCoolPreTextsRM;
			friend class DBCoolProductionStepsRM;
			friend class DBCoolTextsRM;
			friend class DBCuttingTestsRM;
			friend class DBDeptsRM;
			friend class DBDevicesRM;
			friend class DBEmailAlertsRM;
			friend class DBExpirationLabelsRM;
			friend class DBFlashFilesRM;
			friend class DBKnifeTimeHistoryRM;
			friend class DBLabelsRM;
			friend class DBLabelTextsRM;
			friend class DBNetworkErrorsRM;
			friend class DBNutritionsRM;
			friend class DBOperatorsRM;
			friend class DBOperatorNotesRM;
			friend class DBPresentationMethodsRM;
			friend class DBPresentationMethodDetailedStatisticsRM;
			friend class DBPresentationMethodStatisticsRM;
			friend class DBProductsRM;
			friend class DBProductDefaultsRM;
			friend class DBProductDetailedStatisticsRM;
			friend class DBProductStatisticsRM;
			friend class DBProductTextsRM;
			friend class DBProductTextCfgsRM;
			friend class DBRecallsRM;
			friend class DBROMLabelsRM;
			friend class DBScalesRM;
			friend class DBSoftKeysRM;
			friend class DBSoftKeyFeaturesRM;
			friend class DBSoftKeyGroupsRM;
			friend class DBSoftKeyStylesRM;
			friend class DBSpecialProgramsRM;
			friend class DBStreamingGraphicsRM;
			friend class DBSynchronizationChangesRM;
			friend class DBTransactionsRM;
			friend class DBTransactionTypesRM;
			friend class DBWrapperErrorHistoryRM;

			friend class DBQuery<class DBBatch>;
			friend class DBQuery<class DBBarcodeScanner>;
			friend class DBQuery<class DBCategory>;
			friend class DBQuery<class DBCategoryDetailedStatistic>;
			friend class DBQuery<class DBCategoryStatistic>;
			friend class DBQuery<class DBClass>;
			friend class DBQuery<class DBConfiguration>;
			friend class DBQuery<class DBCoolCategory>;
			friend class DBQuery<class DBCoolCategoryCountry>;
			friend class DBQuery<class DBCoolCategoryPreText>;
			friend class DBQuery<class DBCoolCategoryProductionStep>;
			friend class DBQuery<class DBCoolCountry>;
			friend class DBQuery<class DBCoolList>;
			friend class DBQuery<class DBCoolListText>;
			friend class DBQuery<class DBCoolPreText>;
			friend class DBQuery<class DBCoolProductionStep>;
			friend class DBQuery<class DBCoolText>;
			friend class DBQuery<class DBCuttingTest>;
			friend class DBQuery<class DBDept>;
			friend class DBQuery<class DBDevice>;
			friend class DBQuery<class DBEmailAlert>;
			friend class DBQuery<class DBExpirationLabel>;
			friend class DBQuery<class DBFlashFile>;
			friend class DBQuery<class DBKnifeTimeHistory>;
			friend class DBQuery<class DBLabel>;
			friend class DBQuery<class DBLabelText>;
			friend class DBQuery<class DBNetworkError>;
			friend class DBQuery<class DBNutrition>;
			friend class DBQuery<class DBOperator>;
			friend class DBQuery<class DBOperatorNote>;
			friend class DBQuery<class DBPresentationMethod>;
			friend class DBQuery<class DBPresentationMethodDetailedStatistic>;
			friend class DBQuery<class DBPresentationMethodStatistic>;
			friend class DBQuery<class DBProduct>;
			friend class DBQuery<class DBProductDefault>;
			friend class DBQuery<class DBProductDetailedStatistic>;
			friend class DBQuery<class DBProductStatistic>;
			friend class DBQuery<class DBProductText>;
			friend class DBQuery<class DBProductTextCfg>;
			friend class DBQuery<class DBRecall>;
			friend class DBQuery<class DBROMLabel>;
			friend class DBQuery<class DBScale>;
			friend class DBQuery<class DBSoftKey>;
			friend class DBQuery<class DBSoftKeyFeature>;
			friend class DBQuery<class DBSoftKeyGroup>;
			friend class DBQuery<class DBSoftKeyStyle>;
			friend class DBQuery<class DBSpecialProgram>;
			friend class DBQuery<class DBStatus>;
			friend class DBQuery<class DBStreamingGraphic>;
			friend class DBQuery<class DBSynchronizationChange>;
			friend class DBQuery<class DBTransaction>;
			friend class DBQuery<class DBTransactionType>;
			friend class DBQuery<class DBWrapperErrorHistory>;
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSTATUSRM_H
