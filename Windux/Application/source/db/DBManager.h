#ifndef DBMANAGER_H
#define DBMANAGER_H


#include "edb.h"
#include DBMANAGERI_INC
#include DBEXCEPTION_INC
#include DBDATABASEI_INC
#include DBFACTORY_INC
#include DBRECORDMANAGER_INC
#include PROPERTIES_INC
#include MUTEX_INC

#if defined(PAL_WIN32_RUN)
#pragma warning( disable : 4290 )
#endif

class Trace;

#ifndef NO_NAMESPACES
namespace edb
{
#endif


	class DBBatchesRM;
	class DBBarcodeScannersRM;
	class DBCategoriesRM;
	class DBCategoryDetailedStatisticsRM;
	class DBCategoryStatisticsRM;
	class DBClassesRM;
	class DBConfigurationsRM;
	class DBCoolCategoriesRM;
	class DBCoolCategoryCountriesRM;
	class DBCoolCategoryPreTextsRM;
	class DBCoolCategoryProductionStepsRM;
	class DBCoolCountriesRM;
	class DBCoolListsRM;
	class DBCoolListTextsRM;
	class DBCoolPreTextsRM;
	class DBCoolProductionStepsRM;
	class DBCoolTextsRM;
	class DBCuttingTestsRM;
	class DBDeptsRM;
	class DBDevicesRM;
	class DBEmailAlertsRM;
	class DBExpirationLabelsRM;
	class DBFlashFilesRM;
	class DBKnifeTimeHistoryRM;
	class DBLabelsRM;
	class DBLabelTextsRM;
	class DBNetworkErrorsRM;
	class DBNutritionsRM;
	class DBOperatorsRM;
	class DBOperatorNotesRM;
	class DBPresentationMethodsRM;
	class DBPresentationMethodDetailedStatisticsRM;
	class DBPresentationMethodStatisticsRM;
	class DBProductsRM;
	class DBProductDefaultsRM;
	class DBProductDetailedStatisticsRM;
	class DBProductStatisticsRM;
	class DBProductTextsRM;
	class DBProductTextCfgsRM;
	class DBRecallsRM;
	class DBROMLabelsRM;
	class DBScalesRM;
	class DBSoftKeysRM;
	class DBSoftKeyFeaturesRM;
	class DBSoftKeyGroupsRM;
	class DBSoftKeyStylesRM;
	class DBSpecialProgramsRM;
	class DBStatusRM;
	class DBStreamingGraphicsRM;
	class DBSynchronizationChangesRM;
	class DBTransactionsRM;
	class DBTransactionTypesRM;
	class DBWrapperErrorHistoryRM;


	// DBManager is the facade class to the database component as a whole. Use
	// an object of this class to open/close the database or to perform other
	// operations like create, compress, backup, copy, check, etc. Also use an
	// object of this class to 'obtain' access to individual tables of the db,
	// like products, soft keys, etc.
	// DBManager implements DBManagerI so other classes can rely on the
	// implementation of its methods. On the other hand, it does not implement
	// the DBDatabaseI interface (even when it has almost identical APIs), but
	// it does rely on each database layer's manager implementing that interface
	class DBManager : public edb_adapter::DBManagerI
	{
			friend class DBManagerSingleton;

		public:

			// --- Types and subtypes ---
			// factoryMethod_type describes any of DBManager's factory methods
			// (methods that return a record manager every time they are called)
			typedef DBRecordManager* (DBManager::* factoryMethod_type)(); // throw(DBException);


			// Wrapper around factoryMethod to simplify the management of record managers
			// for operations where we iterate through all the record managers
			class FactoryFunctor
			{
				public:
					FactoryFunctor(DBManager* dbm, DBManager::factoryMethod_type m)
							: dbm_(dbm), factoryMethod_(m) {}
					DBRecordManager* operator()() { return (dbm_->*factoryMethod_)(); }
				private:
					DBManager* dbm_;
					DBManager::factoryMethod_type factoryMethod_;
			};


			// FactoryMethod container type and its iterator type
			typedef std::vector<FactoryFunctor*> factoryMethodContainer_type;
			typedef factoryMethodContainer_type::const_iterator factoryMethodIterator;


			// --- Constructors and destructor ---
			static DBManager* getInstance() throw(DBException);    // Singleton pattern's getInstance
			static bool destroyInstance(DBManager* dbm);      // returns true if it actually destroys it


			// --- Accessor methods ---
			// Returns a container will all factory methods
			const factoryMethodContainer_type& getCreateMethods() const { return factoryMethods_; }
			// Provides access to 'defaults.ini' configuration/properties file (part of DBManagerI)
			const util_env::Properties& getProperties() const { return *_config; }
			::Trace& getDebug() const { return *_debug; }  /* (SJJC port 6/19/06) */
			void reportErrorContext() const;


			// --- Processing methods ---
			// Database operations: create, open, compress, copy, backup, close,
			// delete, and check. setDatabaseLanguage is also considered operation.
			// They all are governed by the by the dbOperationMutex_ that prevents
			// two operations from executing simultaneously.
			void createDatabase() throw(DBException);
			void openDatabase() throw(DBException);
			void compressDatabase() throw(DBException);
			// Parameter "from": true means copy from db directory to path,
			// false means copy from path to db directory (not very intuitive!?)
			void copyDatabase(bool from, const std::string& path) throw(DBException);
			void backupDatabase(const std::string& path) throw(DBException);
			void closeDatabase() throw(DBException);
			void forceCloseDatabase();

			void deleteDatabase() throw(DBException);
			bool checkDatabase(std::string &resultExplanation) throw(DBException);
			bool checkDatabaseIndices(bool rebuildIndicesEnabled) throw(DBException);

			static void forceCloseEntireDatabase();

			// languageNumber is 1, 2, etc. associated with Name1, Name2, etc. fields in the db
			void setDatabaseLanguage(unsigned int languageNumber, const std::string& languageCode);

			// Prints information pertaining to the current DBManager instance
			void show();


			// Factory methods
			DBBatchesRM*                               createBatches()                               throw(DBException);
			DBBarcodeScannersRM*                       createBarcodeScanners()                       throw(DBException);
			DBCategoriesRM*                            createCategories()                            throw(DBException);
			DBCategoryDetailedStatisticsRM*            createCategoryDetailedStatistics()            throw(DBException);
			DBCategoryStatisticsRM*                    createCategoryStatistics()                    throw(DBException);
			DBClassesRM*                               createClasses()                               throw(DBException);
			DBConfigurationsRM*                        createConfigurations()                        throw(DBException);
			DBCoolCategoriesRM*                        createCoolCategories()                        throw(DBException);
			DBCoolCategoryCountriesRM*                 createCoolCategoryCountries()                 throw(DBException);
			DBCoolCategoryPreTextsRM*                  createCoolCategoryPreTexts()                  throw(DBException);
			DBCoolCategoryProductionStepsRM*           createCoolCategoryProductionSteps()           throw(DBException);
			DBCoolCountriesRM*                         createCoolCountries()                         throw(DBException);
			DBCoolListsRM*                             createCoolLists()                             throw(DBException);
			DBCoolListTextsRM*                         createCoolListTexts()                         throw(DBException);
			DBCoolPreTextsRM*                          createCoolPreTexts()                          throw(DBException);
			DBCoolProductionStepsRM*                   createCoolProductionSteps()                   throw(DBException);
			DBCoolTextsRM*                             createCoolTexts()                             throw(DBException);
			DBCuttingTestsRM*                          createCuttingTests()                          throw(DBException);
			DBDeptsRM*                                 createDepts()                                 throw(DBException);
			DBDevicesRM*                               createDevices()                               throw(DBException);
			DBEmailAlertsRM*                           createEmailAlerts()                           throw(DBException);
			DBExpirationLabelsRM*                      createExpirationLabels()                      throw(DBException);
			DBFlashFilesRM*                            createFlashFiles()                            throw(DBException);
			DBKnifeTimeHistoryRM*                      createWrapperKnife()                          throw(DBException);
			DBLabelsRM*                                createLabels()                                throw(DBException);
			DBLabelTextsRM*                            createLabelTexts()                            throw(DBException);
			DBNetworkErrorsRM*                         createNetworkErrors()                         throw(DBException);
			DBNutritionsRM*                            createNutritions()                            throw(DBException);
			DBOperatorsRM*                             createOperators()                             throw(DBException);
			DBOperatorNotesRM*                         createOperatorNotes()                         throw(DBException);
			DBPresentationMethodsRM*                   createPresentationMethods()                   throw(DBException);
			DBPresentationMethodDetailedStatisticsRM*  createPresentationMethodDetailedStatistics()  throw(DBException);
			DBPresentationMethodStatisticsRM*          createPresentationMethodStatistics()          throw(DBException);
			DBProductsRM*                              createProducts()                              throw(DBException);
			DBProductDefaultsRM*                       createProductDefaults()                       throw(DBException);
			DBProductDetailedStatisticsRM*             createProductDetailedStatistics()             throw(DBException);
			DBProductStatisticsRM*                     createProductStatistics()                     throw(DBException);
			DBProductTextsRM*                          createProductTexts()                          throw(DBException);
			DBProductTextCfgsRM*                       createProductTextCfgs()                       throw(DBException);
			DBRecallsRM*                               createRecalls()                               throw(DBException);
			DBROMLabelsRM*                             createROMLabels()                             throw(DBException);
			DBScalesRM*                                createScales()                                throw(DBException);
			DBSoftKeysRM*                              createSoftKeys()                              throw(DBException);
			DBSoftKeyFeaturesRM*                       createSoftKeyFeatures()                       throw(DBException);
			DBSoftKeyGroupsRM*                         createSoftKeyGroups()                         throw(DBException);
			DBSoftKeyStylesRM*                         createSoftKeyStyles()                         throw(DBException);
			DBSpecialProgramsRM*                       createSpecialPrograms()                       throw(DBException);
			DBStatusRM*                                createStatus()                                throw(DBException);
			DBStreamingGraphicsRM*                     createStreamingGraphics()                     throw(DBException);
			DBSynchronizationChangesRM*                createSynchronizationChanges()                throw(DBException);
			DBTransactionsRM*                          createTransactions()                          throw(DBException);
			DBTransactionTypesRM*                      createTransactionTypes()                      throw(DBException);
			DBWrapperErrorHistoryRM*                   createWrapperErrorHistory()                   throw(DBException);	 


			// Template method used for testing purposes
			template <class DBEntityRM> DBEntityRM*    create()                                      throw(DBException);


			// --- Static methods ---
			// The Application is required to provided both Configuration and Debug
			// mechanisms before any DBManager is instanciated.
			static void setConfiguration(util_env::Properties* c) { _config = c; }
			static void setDebug(::Trace* d) { _debug = d; }  /* (SJJC port 6/19/06) */

			// Prints information about all currently alive DBManager instances
			static void showAll(const std::string& dc = "");
			// Prints Database Status Report
			static void debugStatus(const std::string& dc = "");


		protected:

			// Constructor is protected because of the Singleton pattern
			DBManager();
			virtual ~DBManager();


		private:

			// --- Data members ---
			edb_adapter::DBDatabaseI* decorated_;         // Decorated Database class
			DBFactory* factory;                          // Factory object: provides the factory methods
			factoryMethodContainer_type factoryMethods_; // Factory methods container
			int instances_;                              // Counter of DBManager users within a thread

			// --- Static data members ---
			// The following mutex is used to prevent two database managers from
			// performing database operations simultaneously (i.e., backing up
			// the database at the same time the database is being deleted or
			// compressed).
			static util_thread::Mutex<> dbOperationMutex_;
			static util_env::Properties* _config;     // Configuration parameters
			static ::Trace* _debug;  /* (SJJC port 6/19/06) */ // Debug/Logging mechanism


			// --- Private utility methods ---
			edb_adapter::DBDatabaseI* loadDatabase(const std::string& dbname, edb_adapter::DBDatabaseI* decorated_ = 0);
			void createFactoryMethods();


			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBManager(const DBManager& db);
			// Do not allow the default assignment operator.
			DBManager operator=(const DBManager& rhs);
	};


	// Template method used for testing purposes
	template <class DBEntityRM> DBEntityRM* DBManager::create() throw(DBException)
	{
		return factory->create<DBEntityRM>();
	}


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBMANAGER_H
