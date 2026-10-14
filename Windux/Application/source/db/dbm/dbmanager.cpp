#include <map>



#include "edb.h"
#include DBMANAGER_INC

#include DBFACTORY_INC
#include DBSTATUSRM_INC
#include DBEXCEPTION_INC

#include UTILSTRING_INC
#include REGISTRY_INC
#include THREAD_INC
#include LOCK_INC


// Adapter layer's database classes
#ifdef MULTIPLE_DB
#  include STDDATABASE_INC
#endif
//#include CBDATABASE_INC
#include SQLITEDATABASE_INC

#ifdef VXWORKS
#  include VXWORKSTASKS_INC
#endif

#include <Configuration.h>
#include <HobartCustomerHook.h>


#ifndef NO_NAMESPACES
namespace edb
{

    using namespace util_thread;
#endif


    using namespace std;


    // DBManagerSingleton class will be responsible for the "singleton" aspects of the
    // DBManager. [DBManager cannot be a singleton because CbDatabase, indirectly an
    // integral part of it, cannot be itself a singleton because some tests have shown
    // that Codebase will not work with a singleton CbDatabase in multi-tasking mode.]
    // DBManagerSingleton will be responsible for managing the state of the database,
    // as well as the registry of DBManager instances.
    // DBManagerSingleton extends the DeleteTaskHook class in the VxWorks environment
    // in order to be notified of when tasks are deleted in order to dispose of their
    // corresponding DBManager instance, if any.
#ifdef VXWORKS
    class DBManagerSingleton : public VxWorksTasks::DeleteTaskHook
#else
                               class DBManagerSingleton
#endif
                               {
                                   public:
                                       // --- Types and subtypes ---
                                       // Database state: open or closed.
                                       typedef enum { closed, open } State;
                                       // Container of DBManager instances. DBManagers are associated with
                                       // threads so that at most one DBManager per thread will be allowed.
                                       typedef std::map<Thread<>::ThreadIDType, DBManager*> dbManagerContainerType;


                                       // --- Constructors and destructor ---
                                       static DBManagerSingleton& getInstance();
                                       static void destroyInstance()
                                       { Lock< Mutex<> > lock(_singletonMutex_); delete _dbimp_; _dbimp_ = 0; }


                                       // --- Accessor methods ---
                                       // Methods to access/manager the state of the database (open/closed)
									   void forceCloseDatabase();
                                       State getState() { Lock< Mutex<> > lock(stateMutex_); return state_; }
                                       bool isStateOpen() { Lock< Mutex<> > lock(stateMutex_); return state_ == open; }
                                       bool isStateClosed() { Lock< Mutex<> > lock(stateMutex_); return state_ == closed; }
                                       void setStateOpen() 
                                       { {Lock< Mutex<> > lock(stateMutex_); state_ = open;} 
                                           DBStatusRM::setStateStatus(DBStatusRM::_STATUS_STATE_OPEN_); }
                                           void setStateClosed() 
                                           { {Lock< Mutex<> > lock(stateMutex_); state_ = closed;} 
                                               DBStatusRM::setStateStatus(DBStatusRM::_STATUS_STATE_CLOSED_); }


                                               // --- Processing methods ---
                                               // Registry methods
                                               DBManager* getRegisteredDBM();         // Get current thread's registered DBManager, if any
                                               bool registerDBM(DBManager* dbm);      // Register DBManager instance for current thread
                                               bool deregisterDBM(DBManager* dbm);    // Deregister DBManager instance
                                               /*               void disableAllDBMs() { Lock< Mutex<> > lock(registryMutex_);
                                                                registry_->process_mem(&DBManager::disable); registry_->removeAll(); } */

                                               void showDBManagers(const std::string& dc = "");   // Print information from all DBManager instances



                                   private:
                                               // --- Data members ---
                                               State state_;                                              // Database state: open or closed
                                               Mutex<> stateMutex_;                  // Mutex to control access to state
                                               dbManagerContainerType registry_;     // Registry for DBManager instances
                                               Mutex<> registryMutex_;               // Mutex to control access to registry


                                               // --- Static data members ---
                                               static DBManagerSingleton* _dbimp_;   // Singleton instance
                                               static Mutex<> _singletonMutex_;      // Mutex to control access to singleton instance


                                               // --- Constructors and destructor ---
                                               // The contents of the following methods are commented out because the Application's
                                               // Configuration Manager is created by the Main task, which opens the database for
                                               // it, but then is used by any other task long after the Main task ceased to exist.
                                               // If we register for task deletion and delete the corresponding DBManager object
                                               // there will probably be problems.
                                               // [Reenabled to test the code since the initial problem has apparently gone away.]
#ifdef VXWORKS
                                               DBManagerSingleton() : state_(closed), registry_()
                                   { if ( !VxWorksTasks::registerDeleteTaskHook(this) ) throw; setStateClosed(); }
                                               ~DBManagerSingleton() { VxWorksTasks::unregisterDeleteTaskHook(this); }
#else
                                               DBManagerSingleton() : state_(closed), registry_() {}
                                               ~DBManagerSingleton() {}
#endif


#ifdef VXWORKS
                                               void taskDeleted(VxWorksTasks::ThreadIDType id);
#endif
                               };



    // Definition/allocation of static members.
    util_env::Properties* DBManager::_config = 0;
    ::Trace* DBManager::_debug = 0;  /* (SJJC port 6/19/06) */
    Mutex<> DBManager::dbOperationMutex_ = Mutex<>();


    DBManager::DBManager() : decorated_(0), factory(0), factoryMethods_(), instances_(0)
    {
        // Load primary database
        const string* dbname = _config->getProperty("db.primarydb");
        if ( dbname && !dbname->empty() )
        {
#ifndef EVOL_NDEBUG
            Debug::trace("database", 28, "Going to load database " + *dbname);
#endif
            decorated_ = loadDatabase(*dbname);
        }

        // Multiple database layers can be added to provide access to the data
        // Load secondary database if any
        dbname = _config->getProperty("db.secondarydb");
        if ( dbname && !dbname->empty() )
        {
#ifndef EVOL_NDEBUG
            Debug::trace("database", 28, "Going to load secondary database " + *dbname);
#endif
            edb_adapter::DBDatabaseI* d = loadDatabase(*dbname, decorated_);

            if ( d )
                decorated_ = d;
        }


#ifdef MULTIPLE_DB
        // Load secondary database if any
        dbname = _config->getProperty("db.secondarydb");
        if ( dbname && !dbname->empty() )
        {
#ifndef EVOL_NDEBUG
            Debug::trace("database", 28, "Going to load secondary database " + *dbname);
#endif
            edb_adapter::DBDatabaseI* d = loadDatabase(*dbname, decorated_);

            if ( d )
                decorated_ = d;
        }
#endif


        // Create the factory object based on the adapter layer(s) used.
#ifndef EVOL_NDEBUG
        Debug::trace("database", 28, "Creating factory object ...");
#endif
        factory = new DBFactory(this, decorated_->getFactory());


        // Make available a container with all the create methods to automate tasks.
#ifndef EVOL_NDEBUG
        Debug::trace("database", 28, "Creating factory methods ...");
#endif
        createFactoryMethods();

#ifndef EVOL_NDEBUG
        Debug::trace("database", 28, "Registering new DBManager with the DBManagerSingleton : " + util_string::ptrtostring(this));
#endif


        if ( Debug::isTracing( "database" ) )
        {
            Debug::trace("database", "DBManager() - showAll before calling DBManagerSingleton::getInstance().registerDBM(this)");
            edb::DBManager::showAll();
        }

        // Register this instance.
        DBManagerSingleton::getInstance().registerDBM(this);

        if ( Debug::isTracing( "database" ) )
        {
            Debug::trace("database", "DBManager() - showAll after calling DBManagerSingleton::getInstance().registerDBM(this)");
            edb::DBManager::showAll();
        }
    }


    DBManager::~DBManager()
    {
#ifndef EVOL_NDEBUG
        Debug::trace("database", 28, "DeRegistering DBManager with the DBManagerSingleton : " + util_string::ptrtostring(this));
#endif


        Debug::trace("database", "~DBManager() - showAll before calling DBManagerSingleton::getInstance().deregisterDBM(this)");
        edb::DBManager::showAll();
 

        // Deregister this instance.
        DBManagerSingleton::getInstance().deregisterDBM(this);


        Debug::trace("database", "~DBManager() - showAll after calling DBManagerSingleton::getInstance().deregisterDBM(this)");
        edb::DBManager::showAll();
 

        // Delete all the factory methods individually before deleting the container.
        factoryMethodContainer_type::const_iterator iter = factoryMethods_.begin();
        while ( iter != factoryMethods_.end() )
            delete *iter++;


        if ( factory )
            delete factory;
        if ( decorated_ )
            delete decorated_;
    }


    // Singleton method. This is a special singleton since it allows more than one
    // instance (although only one per thread).
    DBManager* DBManager::getInstance() throw(DBException)
    {
#ifndef EVOL_NDEBUG
        if ( Debug::isTracing( "database" ) )
        {
            debugStatus();
            Debug::trace("database", 20, "Attempting to obtain a DBManager instance ...");
        }
#endif

        // Require that there are a configuration and a debug mechanisms
        if ( _config == 0 )
        {
            Debug::trace("database.initialize", 1, "***** Configuration file not set.");
            throw DBException(DBException::DATABASE_NOCONFIG, "No configuration available");
        }

        if ( _debug == 0 )
        {
            Debug::trace("database.initialize", 1, "***** Debug mechanism not set.");
            throw DBException(DBException::DATABASE_NODEBUG, "No debugging mechanism available");
        }


		Lock< Mutex<> > lock(dbOperationMutex_);

		// If there is a DBManager for this thread, return it. Otherwise create a new one.
		DBManager* dbm = DBManagerSingleton::getInstance().getRegisteredDBM();
		if ( !dbm )
		{
#ifndef EVOL_NDEBUG
			Debug::trace("database", 28, "There was no instance for this task yet ...");
#endif

			dbm = new DBManager();
		}


		// We count the number of "users" of this DBManager instance (within the
		// current thread)
		if ( dbm )
			++dbm->instances_;

#ifndef EVOL_NDEBUG
        Debug::trace("database", 24, "DBManager instance " + util_string::ptrtostring(dbm) + " obtained successfully (currently " + 
                util_string::inttostring(dbm->instances_) + " reference(s) to this instance).");
#endif

        return dbm;
    }


    // Destroying a DBManager instance without closing the database merely ensures that the
    // db will remain open to the other current instances and that it will be open to the
    // future instances (the current state is passed down through loadDatabase during the
    // instantiation of DBManager).
    bool DBManager::destroyInstance(DBManager* dbm)
    {
        bool deleted = false;

        if ( dbm )
        {
        	{
                Lock< Mutex<> > lock(dbOperationMutex_);

#ifndef EVOL_NDEBUG
                if ( Debug::isTracing( "database" ) )
                {
                    // Lets check whether the 'passed in' DBManager corresponds to the current task
                    DBManager* thisThreadDBM = DBManagerSingleton::getInstance().getRegisteredDBM();
                    if ( thisThreadDBM != dbm )
                        Debug::trace("database", 10,
                                "Going to destroy a DBManager instance that was not created by current task.");
                    else
                        Debug::trace("database", 28, "Going to destroy DBManager instance." + util_string::ptrtostring(dbm));
                }
#endif

				if ( --dbm->instances_ == 0 )
				{
#ifndef EVOL_NDEBUG
					Debug::trace("database", 28, "Since there is only one reference we delete the object. " + util_string::ptrtostring(dbm));
#endif

					delete dbm;
					deleted = true;
				}
#ifndef EVOL_NDEBUG
				else
					Debug::trace("database", 28, "Since there are other references (" +
							util_string::inttostring(dbm->instances_) + ") the object is not deleted." + util_string::ptrtostring(dbm));
#endif
        	}

#ifndef EVOL_NDEBUG
            if ( Debug::isTracing( "database" ) )
                debugStatus();
#endif
        }

        return deleted;
    }



    void DBManager::createDatabase() throw(DBException)
    {
        // This operation can only be performed while the database is closed
        if ( DBManagerSingleton::getInstance().isStateClosed() )
        {
            Lock< Mutex<> > lock(dbOperationMutex_);

            if ( DBManagerSingleton::getInstance().isStateClosed() )
            {
#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Attempting to create the Database ...");
#endif

                {
                    DBStatusRM::OperationFunctor functor = 
                        DBStatusRM::getOperationFunctor(this, DBStatusRM::_STATUS_OPERATION_CREATE_);

                    decorated_->createDatabase();
                }

#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Database created");
#endif

                return;
            }
        }

        throw DBException(DBException::DATABASE_OPEN,
                "Cannot create the database while it is open");
    }


    void DBManager::openDatabase() throw(DBException)
    {
        // This operation can only be performed while the database is closed
        if ( DBManagerSingleton::getInstance().isStateClosed() )
        {
            Lock< Mutex<> > lock(dbOperationMutex_);

            if ( DBManagerSingleton::getInstance().isStateClosed() )
            {
#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Attempting to open the Database ...");
#endif

                {
                    DBStatusRM::OperationFunctor functor = 
                        DBStatusRM::getOperationFunctor(this, DBStatusRM::_STATUS_OPERATION_OPEN_);

                    decorated_->openDatabase();
                    DBManagerSingleton::getInstance().setStateOpen();
                }

#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Database opened");
#endif

                return;
            }
        }

        // No exception is generated since all tasks may attempt to open the database
        // after getting an instance of the DBManager.
    }


    void DBManager::compressDatabase() throw(DBException)
    {
        // This operation can only be performed while the database is closed
        if ( DBManagerSingleton::getInstance().isStateClosed() )
        {
            Lock< Mutex<> > lock(dbOperationMutex_);

            if ( DBManagerSingleton::getInstance().isStateClosed() )
            {
#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Attempting to compress the Database ...");
#endif

                {
                    DBStatusRM::OperationFunctor functor = 
                        DBStatusRM::getOperationFunctor(this, DBStatusRM::_STATUS_OPERATION_COMPRESS_);

                    decorated_->compressDatabase();

                    DBStatusRM::setNowStatus(DBStatusRM::_STATUS_LAST_COMPRESS_COMPLETED_);
                    DBStatusRM::copyStatus(DBStatusRM::_STATUS_LAST_COMPRESS_STARTED_, DBStatusRM::_STATUS_OPERATION_STARTED_);
                }

#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Completed compression of the Database");
#endif

                return;
            }
        }

        throw DBException(DBException::DATABASE_OPEN,
                "Cannot compress the database while it is open");
    }


    void DBManager::copyDatabase(bool from, const string& path) throw(DBException)
    {
        // This operation can only be performed while the database is closed
        if ( DBManagerSingleton::getInstance().isStateClosed() )
        {
            Lock< Mutex<> > lock(dbOperationMutex_);

            if ( DBManagerSingleton::getInstance().isStateClosed() )
            {
#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Attempting to copy the Database ...");
#endif

                {
                    DBStatusRM::OperationFunctor functor = 
                        DBStatusRM::getOperationFunctor(this, DBStatusRM::_STATUS_OPERATION_COPY_);

                    decorated_->copyDatabase(from, path);
                }

#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Database copied");
#endif

                return;
            }
        }

        throw DBException(DBException::DATABASE_OPEN,
                "Cannot copy the database while it is open");
    }


    void DBManager::backupDatabase(const string& path) throw(DBException)
    {
        Lock< Mutex<> > lock(dbOperationMutex_);

#ifndef EVOL_NDEBUG
        Debug::trace("database", 20, "Attempting to backup the Database ...");
#endif

        // This operation can be performed while the database is open or closed
        {
            DBStatusRM::OperationFunctor functor = 
                DBStatusRM::getOperationFunctor(this, DBStatusRM::_STATUS_OPERATION_BACKUP_);

#ifdef VXWORKS     /* (SJJC port 6/19/06) */
#ifndef EVOL_NDEBUG
            if ( Debug::isTracing("database.backup.memory") )
            { printf("**** before backing up database\n"); memShow(0); }
#endif
#endif

            decorated_->backupDatabase(path); // Beware that the db may be open

#ifdef VXWORKS     /* (SJJC port 6/19/06) */
#ifndef EVOL_NDEBUG
            if ( Debug::isTracing("database.backup.memory") )
            { printf("**** after backing up database\n"); memShow(0); }
#endif
#endif

            DBStatusRM::setNowStatus(DBStatusRM::_STATUS_LAST_BACKUP_COMPLETED_);
            DBStatusRM::copyStatus(DBStatusRM::_STATUS_LAST_BACKUP_STARTED_, DBStatusRM::_STATUS_OPERATION_STARTED_);
        }

#ifndef EVOL_NDEBUG
        Debug::trace("database", 20, "Completed backing up of the Database");
#endif
    }


    void DBManager::closeDatabase() throw(DBException)
    {
        // This operation can only be performed while the database is open
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            Lock< Mutex<> > lock(dbOperationMutex_);

            if ( DBManagerSingleton::getInstance().isStateOpen() )
            {
                DBManagerSingleton::getInstance().setStateClosed();

#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Attempting to close the Database ...");
#endif

                {
                    DBStatusRM::OperationFunctor functor = 
                        DBStatusRM::getOperationFunctor(this, DBStatusRM::_STATUS_OPERATION_CLOSE_);

                    forceCloseDatabase();
                }

#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Database closed");
#endif
            }
        }

        // No exception is generated if the database is already closed since that
        // is the intention of this API.
    }

	void DBManager::forceCloseDatabase()
	{
#ifndef EVOL_NDEBUG
		Debug::trace("database", 20, "DBManager - Attempting to force close the Database ... this:" + util_string::ptrtostring(this));
#endif

		// Before closing the database prevent disable every record manager
		if (factory)
			factory->disableDBAccess();
		if (decorated_)
			decorated_->closeDatabase();
	}



	//static
	void DBManager::forceCloseEntireDatabase()
	{
#ifndef EVOL_NDEBUG
		Debug::trace("database", 20, "static DBManager - Attempting to force close the entire Database ...");
#endif

		DBManagerSingleton::getInstance().forceCloseDatabase();
		DBManagerSingleton::getInstance().setStateClosed();
	}




    void DBManager::deleteDatabase() throw(DBException)
    {
        // This operation can only be performed while the database is closed
        if ( DBManagerSingleton::getInstance().isStateClosed() )
        {
            Lock< Mutex<> > lock(dbOperationMutex_);

            if ( DBManagerSingleton::getInstance().isStateClosed() )
            {
#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Attempting to delete the Database ...");
#endif

                {
                    DBStatusRM::OperationFunctor functor = 
                        DBStatusRM::getOperationFunctor(this, DBStatusRM::_STATUS_OPERATION_DELETE_);

                    decorated_->deleteDatabase();
                }

#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Database deleted");
#endif

                return;
            }
        }

        throw DBException(DBException::DATABASE_OPEN,
                "Cannot delete the database while it is open");
    }


    bool DBManager::checkDatabase(std::string &resultExplanation) throw(DBException)
    {
        // This operation can only be performed while the database is closed - NOT TRUE FOR SQLITE! - so I'm commenting that part out
        //if ( DBManagerSingleton::getInstance().isStateClosed() )
        //{
            Lock< Mutex<> > lock(dbOperationMutex_);

            //if ( DBManagerSingleton::getInstance().isStateClosed() )
            //{
                bool result;

#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Attempting to check the Database ...");
#endif

                {
                    DBStatusRM::OperationFunctor functor = DBStatusRM::getOperationFunctor(this, DBStatusRM::_STATUS_OPERATION_CHECK_);

                    result = decorated_->checkDatabase(resultExplanation);
                }

#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Database checked");
#endif

                return result;
            //}
        //}

        //throw DBException(DBException::DATABASE_OPEN, "Cannot check the database while it is open");
    }


    bool DBManager::checkDatabaseIndices(bool rebuildIndicesEnabled) throw(DBException)
    {
        // This operation can only be performed while the database is closed
        if ( DBManagerSingleton::getInstance().isStateClosed() )
        {
            Lock< Mutex<> > lock(dbOperationMutex_);

            if ( DBManagerSingleton::getInstance().isStateClosed() )
            {
                bool result;

#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Attempting to check the Database Indices ...");
#endif

                {
                    DBStatusRM::OperationFunctor functor = 
                        DBStatusRM::getOperationFunctor(this, DBStatusRM::_STATUS_OPERATION_CHECK_);

                    result = decorated_->checkDatabaseIndices(rebuildIndicesEnabled);
                }

#ifndef EVOL_NDEBUG
                Debug::trace("database", 20, "Database indices checked");
#endif

                return result;
            }
        }

        throw DBException(DBException::DATABASE_OPEN,
                "Cannot check the database indices while it is open");
    }


    void DBManager::setDatabaseLanguage(unsigned int languageNumber, const std::string& languageCode)
    {
#ifndef EVOL_NDEBUG
        Debug::trace("database", 20, "Changing language " + util_string::uinttostring(languageNumber) +
                " to language code " + languageCode);
#endif

        // This operation can be performed while the database is open or closed
        {
            Lock< Mutex<> > lock(dbOperationMutex_);
            DBStatusRM::OperationFunctor functor = 
                DBStatusRM::getOperationFunctor(this, DBStatusRM::_STATUS_OPERATION_LANGUAGECHANGE_);

            decorated_->setDatabaseLanguage(languageNumber, languageCode);
        }

#ifndef EVOL_NDEBUG
        Debug::trace("database", 20, "Database language changed");
#endif
    }


    void DBManager::show()
    {
        decorated_->show();
    }


    // Factory methods
    edb::DBBatchesRM* DBManager::createBatches() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::Batches);

            return factory->createBatches();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (batches) when the database is closed");
    }

    edb::DBBarcodeScannersRM* DBManager::createBarcodeScanners() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::BarcodeScanners);        // jhc - This could be the table name?

            return factory->createBarcodeScanners();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (barcode scanners data) when the database is closed");
    }

    edb::DBCategoriesRM* DBManager::createCategories() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::Categories);

            return factory->createCategories();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (categories) when the database is closed");
    }


    DBCategoryDetailedStatisticsRM* DBManager::createCategoryDetailedStatistics() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::CategoryDetailedStatistics);

            return factory->createCategoryDetailedStatistics();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (category detailed statistics) when the database is closed");
    }


    DBCategoryStatisticsRM* DBManager::createCategoryStatistics() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::CategoryStatistics);

            return factory->createCategoryStatistics();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (category statistics) when the database is closed");
    }


    edb::DBClassesRM* DBManager::createClasses() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::Classes);

            return factory->createClasses();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (classes) when the database is closed");
    }


    edb::DBConfigurationsRM* DBManager::createConfigurations() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::Configurations);

            return factory->createConfigurations();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (configurations) when the database is closed");
    }


    edb::DBCoolCategoriesRM* DBManager::createCoolCategories() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::CoolCategories);

            return factory->createCoolCategories();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (coolcountries) when the database is closed");
    }


    edb::DBCoolCategoryCountriesRM* DBManager::createCoolCategoryCountries() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::CoolCategoryCountries);

            return factory->createCoolCategoryCountries();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (coolcategorycountries) when the database is closed");
    }


    edb::DBCoolCategoryPreTextsRM* DBManager::createCoolCategoryPreTexts() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::CoolCategoryPreTexts);

            return factory->createCoolCategoryPreTexts();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (coolcategorypretexts) when the database is closed");
    }


    edb::DBCoolCategoryProductionStepsRM* DBManager::createCoolCategoryProductionSteps() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::CoolCategoryProductionSteps);

            return factory->createCoolCategoryProductionSteps();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (coolcategoryproductionsteps) when the database is closed");
    }


    edb::DBCoolCountriesRM* DBManager::createCoolCountries() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::CoolCountries);

            return factory->createCoolCountries();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (coolcountries) when the database is closed");
    }


    edb::DBCoolListsRM* DBManager::createCoolLists() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::CoolLists);

            return factory->createCoolLists();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (coollists) when the database is closed");
    }


    edb::DBCoolListTextsRM* DBManager::createCoolListTexts() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::CoolListTexts);

            return factory->createCoolListTexts();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (coollists) when the database is closed");
    }


    edb::DBCoolPreTextsRM* DBManager::createCoolPreTexts() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::CoolPreTexts);

            return factory->createCoolPreTexts();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (coolpretexts) when the database is closed");
    }


    edb::DBCoolProductionStepsRM* DBManager::createCoolProductionSteps() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::CoolProductionSteps);

            return factory->createCoolProductionSteps();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (coolproductionsteps) when the database is closed");
    }


    edb::DBCoolTextsRM* DBManager::createCoolTexts() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::CoolTexts);

            return factory->createCoolTexts();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (cooltexts) when the database is closed");
    }


edb::DBCuttingTestsRM* DBManager::createCuttingTests() throw(DBException)
{
	if ( DBManagerSingleton::getInstance().isStateOpen() )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::CuttingTests);
		
		return factory->createCuttingTests();
	}
	else
		throw DBException(DBException::DATABASE_CLOSED,
				"Cannot create record manager (CuttingTests) when the database is closed");
}


DBDeptsRM* DBManager::createDepts() throw(DBException)
{
	if (DBManagerSingleton::getInstance().isStateOpen())
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::Depts);

		return factory->createDepts();
	}
	else
		throw DBException(DBException::DATABASE_CLOSED,
			"Cannot create record manager (Depts) when the database is closed");
}


	DBDevicesRM* DBManager::createDevices() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::Devices);

            return factory->createDevices();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (Devices) when the database is closed");
    }


    DBEmailAlertsRM* DBManager::createEmailAlerts() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::EmailAlerts);

            return factory->createEmailAlerts();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (email alerts) when the database is closed");
    }


    DBExpirationLabelsRM* DBManager::createExpirationLabels() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::ExpirationLabels);

            return factory->createExpirationLabels();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (email alerts) when the database is closed");
    }


    edb::DBFlashFilesRM* DBManager::createFlashFiles() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::FlashFiles);

            return factory->createFlashFiles();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (flash files) when the database is closed");
    }


    edb::DBLabelsRM* DBManager::createLabels() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::Labels);

            return factory->createLabels();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (labels) when the database is closed");
    }


    edb::DBLabelTextsRM* DBManager::createLabelTexts() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::LabelTexts);

            return factory->createLabelTexts();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (label texts) when the database is closed");
    }


    edb::DBNetworkErrorsRM* DBManager::createNetworkErrors() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::NetworkErrors);

            return factory->createNetworkErrors();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (network errors) when the database is closed");
    }


    edb::DBNutritionsRM* DBManager::createNutritions() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::Nutritions);

            return factory->createNutritions();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (nutritions) when the database is closed");
    }


    edb::DBOperatorsRM* DBManager::createOperators() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::Operators);

            return factory->createOperators();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (operators) when the database is closed");
    }


    edb::DBOperatorNotesRM* DBManager::createOperatorNotes() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::OperatorNotes);

            return factory->createOperatorNotes();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (operator notes) when the database is closed");
    }


    edb::DBPresentationMethodsRM* DBManager::createPresentationMethods() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::PresentationMethods);

            return factory->createPresentationMethods();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (presentation methods) when the database is closed");
    }


    DBPresentationMethodDetailedStatisticsRM* DBManager::createPresentationMethodDetailedStatistics() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::PresentationMethodDetailedStatistics);

            return factory->createPresentationMethodDetailedStatistics();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (presentation method detailed statistics) when the database is closed");
    }


    DBPresentationMethodStatisticsRM* DBManager::createPresentationMethodStatistics() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::PresentationMethodStatistics);

            return factory->createPresentationMethodStatistics();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (presentation method statistics) when the database is closed");
    }


    edb::DBProductsRM* DBManager::createProducts() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::Products);

            return factory->createProducts();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (products) when the database is closed");
    }


    edb::DBProductDefaultsRM* DBManager::createProductDefaults() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::ProductDefaults);

            return factory->createProductDefaults();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (product defaults) when the database is closed");
    }


    edb::DBProductDetailedStatisticsRM* DBManager::createProductDetailedStatistics() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::ProductDetailedStatistics);

            return factory->createProductDetailedStatistics();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (product detailed statistics) when the database is closed");
    }


    edb::DBProductStatisticsRM* DBManager::createProductStatistics() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::ProductStatistics);

            return factory->createProductStatistics();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (product statistics) when the database is closed");
    }


    edb::DBProductTextsRM* DBManager::createProductTexts() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::ProductTexts);

            return factory->createProductTexts();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (product texts) when the database is closed");
    }


    edb::DBProductTextCfgsRM* DBManager::createProductTextCfgs() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::ProductTextCfgs);

            return factory->createProductTextCfgs();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (product text cfgs) when the database is closed");
    }

    edb::DBRecallsRM* DBManager::createRecalls() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::Recalls);

            return factory->createRecalls();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (recalls) when the database is closed");
    }


    edb::DBROMLabelsRM* DBManager::createROMLabels() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::ROMLabels);

            return factory->createROMLabels();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (ROM labels) when the database is closed");
    }


    edb::DBScalesRM* DBManager::createScales() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::Scales);

            return factory->createScales();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (scales) when the database is closed");
    }


    edb::DBSoftKeysRM* DBManager::createSoftKeys() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::SoftKeys);

            return factory->createSoftKeys();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (soft keys) when the database is closed");
    }


    edb::DBSoftKeyFeaturesRM* DBManager::createSoftKeyFeatures() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::SoftKeyFeatures);

            return factory->createSoftKeyFeatures();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (soft key features) when the database is closed");
    }


    edb::DBSoftKeyGroupsRM* DBManager::createSoftKeyGroups() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::SoftKeyGroups);

            return factory->createSoftKeyGroups();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (soft key groups) when the database is closed");
    }


    edb::DBSoftKeyStylesRM* DBManager::createSoftKeyStyles() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::SoftKeyStyles);

            return factory->createSoftKeyStyles();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (soft key styles) when the database is closed");
    }


    DBSpecialProgramsRM* DBManager::createSpecialPrograms() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::SpecialPrograms);

            return factory->createSpecialPrograms();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (special programs) when the database is closed");
    }


    edb::DBStatusRM* DBManager::createStatus() throw(DBException)
    {
        return factory->createStatus();
    }


    edb::DBStreamingGraphicsRM* DBManager::createStreamingGraphics() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::StreamingGraphics);

            return factory->createStreamingGraphics();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (streaming graphics) when the database is closed");
    }


    edb::DBSynchronizationChangesRM* DBManager::createSynchronizationChanges() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::SynchronizationChanges);

            return factory->createSynchronizationChanges();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (synchronization changes) when the database is closed");
    }


    edb::DBTransactionsRM* DBManager::createTransactions() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::Transactions);

            return factory->createTransactions();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (transactions) when the database is closed");
    }


    edb::DBTransactionTypesRM* DBManager::createTransactionTypes() throw(DBException)
    {
        if ( DBManagerSingleton::getInstance().isStateOpen() )
        {
            DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::TransactionTypes);

            return factory->createTransactionTypes();
        }
        else
            throw DBException(DBException::DATABASE_CLOSED,
                    "Cannot create record manager (transaction types) when the database is closed");
    }


edb::DBWrapperErrorHistoryRM* DBManager::createWrapperErrorHistory() throw(DBException)
{
	if ( DBManagerSingleton::getInstance().isStateOpen() )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::WrapperErrorHistory);

		return factory->createWrapperErrorHistory();
	}
	else
		throw DBException(DBException::DATABASE_CLOSED,
				"Cannot create record manager (Wrapper Error History) when the database is closed");
}


edb::DBKnifeTimeHistoryRM* DBManager::createWrapperKnife() throw(DBException)
{
	if ( DBManagerSingleton::getInstance().isStateOpen() )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessOpenTable, DBRecordManager::WrapperKnife);

		return factory->createWrapperKnife();
	}
	else
		throw DBException(DBException::DATABASE_CLOSED,
				"Cannot create record manager (Wrapper Knife) when the database is closed");
}


    void DBManager::reportErrorContext() const
    {
        Debug::trace("database.error", 20, "---------------------------------------------------------");
        Debug::trace("database.error", 20, "Reporting Database Error Context for DBManager 0x" + util_string::ptrtostring(this));
        Debug::trace("database.error", 20, "---------------------------------------------------------");

        showAll("database.error");
        debugStatus("database.error");

        Debug::trace("database.error", 20, "---------------------------------------------------------");
    }


    void DBManager::showAll(const std::string& dc)
    {
        DBManagerSingleton::getInstance().showDBManagers(dc);

        /* Need "database.show" debugging category turned on for this report. */
        DBFactory::showDBRecordManagers(dc);
    }


    void DBManager::debugStatus(const std::string& dc)
    {
        string* status = 0;
        string debuggingCategory(dc);

        if ( debuggingCategory.empty() )
            debuggingCategory.assign("database.status.report");

        Debug::trace(debuggingCategory, 20, "--- DATABASE STATUS REPORT ---");


        // Show the state: open or closed
        status = DBStatusRM::getStatus(DBStatusRM::_STATUS_STATE_);
        if ( status )
        {
            // Number of simultaneous database operations/accesses currently going on
            int busyCounter = DBStatusRM::getBusyCounter();
            // Show activity: busy or idle
            string* busy = DBStatusRM::getStatus(DBStatusRM::_STATUS_ACTIVITY_);
            if ( busy )
            {
                Debug::trace(debuggingCategory, 20, "   Database state        : " + *status +
                        " (database is currently " + *busy + " {" + util_string::inttostring(busyCounter) + "})");  /* (SJJC port 6/19/06) */

                delete busy;
            }
            else
                Debug::trace(debuggingCategory, 20, "   Database state        : " + *status);

            delete status;
        }
        else
            Debug::trace(debuggingCategory, 20,
                    "   Database state        : Closed (has not been open yet)");


        // Current database accesses
        DBStatusRM::showDBAccesses(debuggingCategory);


        // Current operation the database is undertaking
        status = DBStatusRM::getStatus(DBStatusRM::_STATUS_OPERATION_);
        if ( status )
        {
            if ( status->compare(DBStatusRM::_STATUS_OPERATION_NONE_) )
            {
                // When the operation started
                string* started = DBStatusRM::getStatus(DBStatusRM::_STATUS_OPERATION_STARTED_);
                if ( started )
                {
                    Debug::trace(debuggingCategory, 20, "   Current operation     : '" + 
                            *status + "' (started at " + *started + ")");

                    delete started;
                }
                else
                    Debug::trace(debuggingCategory, 20, "   Current operation     : '" + 
                            *status + "'.");
            }
            else
                Debug::trace(debuggingCategory, 20, "   Current operation     : None");

            delete status;
        }
        else
            Debug::trace(debuggingCategory, 20, 
                    "   Current operation     : None (none has been performed yet)");


        // Last backup performed
        status = DBStatusRM::getStatus(DBStatusRM::_STATUS_LAST_BACKUP_STARTED_);
        if ( status )
        {
            string* completed = DBStatusRM::getStatus(DBStatusRM::_STATUS_LAST_BACKUP_COMPLETED_);
            if ( completed )
            {
                Debug::trace(debuggingCategory, 20, "   Last backup           : started at " + *status +
                        " and completed at " + *completed);

                delete completed;
            }
            else
                Debug::trace(debuggingCategory, 20, "   Backup in progress    : started at " + *status);

            delete status;
        }
        else
            Debug::trace(debuggingCategory, 20, "   Last backup           : no backup has occurred");


        // Last compression performed
        status = DBStatusRM::getStatus(DBStatusRM::_STATUS_LAST_COMPRESS_STARTED_);
        if ( status )
        {
            string* completed = DBStatusRM::getStatus(DBStatusRM::_STATUS_LAST_COMPRESS_COMPLETED_);
            if ( completed )
            {
                Debug::trace(debuggingCategory, 20, "   Last compress         : started at " + *status +
                        " and completed at " + *completed);

                delete completed;
            }
            else
                Debug::trace(debuggingCategory, 20, "   Compress in progress  : started at " + *status);

            delete status;
        }
        else
            Debug::trace(debuggingCategory, 20, "   Last compress         : no compress has occurred");


        Debug::trace(debuggingCategory, 20, "--- DATABASE STATUS COMPLETED ---");
    }



    // This is a private method that attempts to load a database as if it were
    // a dynamically linked library. This method can eventually be made more
    // sophisticated so it actually loads a dynamically linked library or uses a
    // form of registry to locate the desired database.
    edb_adapter::DBDatabaseI* DBManager::loadDatabase(const std::string& dbname,
            edb_adapter::DBDatabaseI* decorated_)
    {
/*
        if ( dbname == "Codebase" )
        {
#ifndef EVOL_NDEBUG
            Debug::trace("database", 28, "Creating Codebase instance ...");
#endif

            return new edb_adapter::CbDatabase(*this, DBManagerSingleton::getInstance().isStateOpen(), decorated_);
        }
*/
//        else
		if ( dbname == "SQLite" )
        {
#ifndef EVOL_NDEBUG
            Debug::trace("database", 28, "Creating SQLite instance ...");
#endif
            const string *dbname( this->getProperties().getProperty("db.SQLite.path") );

#if defined( __linux__ )
			if (( Configuration::getInstance()->getCustomerHooksEnabled() ) && ( dbname ))
                hobart_cust_hook_before_open_database( dbname->c_str() );
#endif

            return new edb_adapter::SqliteDatabase(*this, DBManagerSingleton::getInstance().isStateOpen(), decorated_);
        }
#ifdef MULTIPLE_DB
        else if ( dbname == "STDDatabase" )
        {
            return new edb_adapter::STDDatabase(*this,
                    DBManagerSingleton::getInstance().isStateOpen(), decorated_);
        }
#endif

        return 0;
    }


	// Creates the factory methods container
	void DBManager::createFactoryMethods()
	{
		if ( factoryMethods_.size() )
			factoryMethods_.clear();

		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createBatches));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createCategories));
		//      factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createCategoryDetailStatistics));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createCategoryStatistics));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createClasses));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createConfigurations));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createCoolCategories));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createCoolCategoryCountries));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createCoolCategoryPreTexts));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createCoolCategoryProductionSteps));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createCoolCountries));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createCoolLists));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createCoolListTexts));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createCoolPreTexts));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createCoolProductionSteps));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createCoolTexts));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createCuttingTests));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createDepts));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createDevices));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createEmailAlerts));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createExpirationLabels));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createFlashFiles));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createLabels));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createLabelTexts));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createNetworkErrors));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createNutritions));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createOperators));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createOperatorNotes));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createPresentationMethods));
		//      factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createPresentationMethodDetailedStatistics));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createPresentationMethodStatistics));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createProducts));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createProductDefaults));
		//      factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createProductDetailedStatistics));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createProductStatistics));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createRecalls));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createROMLabels));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createScales));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createSoftKeys));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createSoftKeyFeatures));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createSoftKeyGroups));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createSoftKeyStyles));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createSpecialPrograms));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createStreamingGraphics));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createSynchronizationChanges));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createTransactions));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createTransactionTypes));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createWrapperErrorHistory));
		factoryMethods_.push_back(new FactoryFunctor(this, (factoryMethod_type)&DBManager::createWrapperKnife));
	}





    // Definition/allocation of static members.
    Mutex<> DBManagerSingleton::_singletonMutex_ = Mutex<>();
    DBManagerSingleton* DBManagerSingleton::_dbimp_ = 0;


    // Singleton method
    DBManagerSingleton& DBManagerSingleton::getInstance()
    {
        if ( _dbimp_ == 0 )
        {
            Lock< Mutex<> > lock(_singletonMutex_);
            if ( _dbimp_ == 0 )
                _dbimp_ = new DBManagerSingleton();
        }

        return *_dbimp_;
    }


    DBManager* DBManagerSingleton::getRegisteredDBM()
    {
        Thread<>::ThreadIDType tid = Thread<>::getCurrentThreadID();

        Lock< Mutex<> > lock(registryMutex_);

        dbManagerContainerType::iterator iter = registry_.find(tid);
        if ( iter != registry_.end() )
            return iter->second;
        else
            return 0;
    }


    bool DBManagerSingleton::registerDBM(DBManager* dbm)
    {
        Thread<>::ThreadIDType tid = Thread<>::getCurrentThreadID();

        Lock< Mutex<> > lock(registryMutex_);

        pair<dbManagerContainerType::iterator, bool> result = registry_.insert(make_pair(tid,dbm));

        return result.second;
    }


    bool DBManagerSingleton::deregisterDBM(DBManager* dbm)
    {
        bool result = false;
        Thread<>::ThreadIDType tid = Thread<>::getCurrentThreadID();

        Lock< Mutex<> > lock(registryMutex_);

        dbManagerContainerType::iterator iter = registry_.find(tid);
        if ( iter != registry_.end() )
        {
            // If the DBManager passed in corresponds to the current thread's
            // DBManager, deregister it
            if ( iter->second == dbm )
            {
                registry_.erase(iter);
                result = true;
            }
        }

        if ( !result )
        {
            // Otherwise, iterate through the DBManagers container to find the
            // desired one and deregister it
            iter = registry_.begin();
            while ( iter != registry_.end() && !result )
            {
                if ( iter->second == dbm )
                {
                    registry_.erase(iter);
                    result = true;

                    break;  /* (SJJC port 6/19/06) */
                }

                ++iter;
            }
        }

#ifndef EVOL_NDEBUG
        if ( !result )
            Debug::trace("database", 11, "Failed to deregister DBManager " +
                    util_string::ptrtostring(dbm) + " (not found)");
#endif

        return result;
    }


	void DBManagerSingleton::forceCloseDatabase()
	{
#ifndef EVOL_NDEBUG
		Debug::trace("database", 20, "DBManagerSingleton - Attempting to force close the Database ...");
#endif

        Lock< Mutex<> > lock(registryMutex_);

        dbManagerContainerType::iterator iter = registry_.begin();
        while ( iter != registry_.end() )
        {
            DBManager* dbm = iter->second;
			if (dbm)
				dbm->forceCloseDatabase();

            registry_.erase(iter);
            iter = registry_.begin();
        }

#ifndef EVOL_NDEBUG
        Debug::trace("database", 11, "DBManagerSingleton - Finished force closing the Database.");
#endif
	}


    void DBManagerSingleton::showDBManagers(const std::string& dc)
    {
        std::string debuggingCategory(dc);

        if ( debuggingCategory.empty() )
            debuggingCategory.assign("database.show");

        Debug::trace(debuggingCategory, 20, string("The database is currently ") + 
                (isStateOpen() ? "OPEN." : "CLOSED."));

        // Show all CbDatabase information
        edb_adapter::SqliteDatabase::showAll();  /* (SJJC port 6/19/06) */

        // Show each of the currently registered DBManagers
        {
            Lock< Mutex<> > lock(registryMutex_);

            dbManagerContainerType::iterator iter = registry_.begin();
            while ( iter != registry_.end() )
            {
                DBManager* dbm = iter->second;
                unsigned long long dbgPointer = iter->first; /* (SJJC port 6/19/06) */

                Debug::trace(debuggingCategory, 20, "DBManager 0x" + util_string::ptrtostring(dbm) + " associated with task " +
                        util_string::ptrtostring((void*)dbgPointer));

                dbm->show();

                Debug::trace(debuggingCategory, 20, "");

                ++iter;
            }
        }
    }


#ifdef VXWORKS
    void DBManagerSingleton::taskDeleted(VxWorksTasks::ThreadIDType id)
    {
        DBManager* db = 0;

        {
            Lock< Mutex<> > lock(registryMutex_);

            dbManagerContainerType::iterator iter = registry_.find(id);
            if ( iter != registry_.end() )
                db = iter->second;
        }

        // Deleting the instance will deregister it
        if ( db )
            delete db;
    }
#endif



#ifndef NO_NAMESPACES
} // namespace edb
#endif
