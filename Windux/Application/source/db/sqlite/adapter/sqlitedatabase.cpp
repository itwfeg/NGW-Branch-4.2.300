#include <vector>
#include <string>
#ifdef WIN32_OS
	#include <locale>
#endif

#include "PAL_platformheader.h"
//#include "MessageLogger.h"

#include "sqlitedatabase.h"
#include "sqlitefactory.h"
#include "sqliteautoincrementsrm.h"
#include "filesys/file.h"

#include DBMANAGERI_INC
#include DBEXCEPTION_INC

#include UTILSTRING_INC
#include DIRECTORY_INC
#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using edb::DBException;
	using namespace util_string;
	using namespace util_thread;
#endif



// SqliteDatabaseSingleton class will be responsible for the "singleton" aspects of the
// SqliteDatabase, because this class cannot be a singleton. [This is because some tests 
// have shown that Codebase will not work with a singleton SqliteDatabase in multi-tasking 
// mode.]
// SqliteDatabaseSingleton will then be responsible for managing the state of the database,
// including the language configuration.
// Following is the declaration of this class. The implementation of its methods is at
// the end of this file.
class SqliteDatabaseSingleton
{
	public:
	
		// --- Types and subtypes ---
		typedef std::map<sqlite3*, SqliteDatabase*> sqliteDatabaseContainerType;

		// --- Constructors and destructor ---
		static SqliteDatabaseSingleton& getInstance();
		static void destroyInstance() { Lock< Mutex<> > lock(_singletonMutex_); delete _sqliteimp_; _sqliteimp_ = 0; }

		// --- Accessor methods ---
		std::string getLanguage(unsigned int number) { Lock< Mutex<> > lock(languageMutex_); if ( number < languages_.size() ) return languages_[number]; return ""; }
		void setLanguage(unsigned int number, const std::string& language) { Lock< Mutex<> > lock(languageMutex_); if ( languages_.size() <= number ) languages_.resize(number + 1, ""); languages_[number] = language; }

		// --- Processing methods ---
		unsigned int getLanguageNumber(const std::string& languageCode);

		bool registerSqliteDatabase(SqliteDatabase* s, sqlite3* s3);   // Register SqliteDatabase instance
		bool deregisterSqliteDatabase(SqliteDatabase* s);              // Deregister SqliteDatabase instance
		SqliteDatabase* getRegisteredSqliteDatabase(sqlite3* s3);      // Retrieve SqliteDatabase instance

		void show();


	private:
	
		// --- Data members ---
		std::vector<std::string> languages_;               // Supported languages container
		Mutex<> languageMutex_;                            // Mutex to access languages container
		sqliteDatabaseContainerType registry_;             // Registry for SqliteDatabase instances
		Mutex<> registryMutex_;                            // Mutex to control access to registry

		// Singleton data members
		static SqliteDatabaseSingleton* _sqliteimp_;
		static Mutex<> _singletonMutex_;

		// --- Constructors and destructor ---
		SqliteDatabaseSingleton() : languages_(10), languageMutex_() {}
		~SqliteDatabaseSingleton() {}
};



#ifdef USE_ACCOUNTANT
int SqliteDatabase::_dataType_ = 0;
string SqliteDatabase::_dataTypeName_("SqliteDatabase");
#endif


SqliteDatabase::SqliteDatabase(const DBManagerI& mgr, bool alreadyOpen, DBDatabaseI* dbdecor)
		: manager_(mgr), config_(mgr.getProperties()), db_(0), factory_(0), checkAutoIncrementKeys_(false), autoIncrementer_(0), decorated_(dbdecor)
{
#ifndef EVOL_NDEBUG
	Debug::trace("database.construction", 20, "sqlite3 instance = " + ptrtostring(db_));
#endif

#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif


	// Read now any parameters of SqliteDatabase
	const string* property = config_.getProperty("db.SQLite.checkAutoIncrementKeys");
	if ( property )
		checkAutoIncrementKeys_ = stringtobool(*property);


	openDatabase();

	autoIncrementer_ = new SqliteAutoIncrementsRM(*this, mgr.getProperties());
	

#ifndef EVOL_NDEBUG
	Debug::trace("database.construction", 20, "Sqlite instance created");
#endif
}


SqliteDatabase::~SqliteDatabase()
{
	SqliteDatabaseSingleton::getInstance().deregisterSqliteDatabase(this);

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	// Close our database
	closeDatabase();
	//delete db_;
	
	
	if ( autoIncrementer_ )
		delete autoIncrementer_;


	// Factory is part of a decorated_ stack and it is deleted when its top is deleted.
	// Therefore we do not need to delete it here.

	// If there is a decorated_ database, destroy it too
	if ( decorated_ )
		delete decorated_;
}


DBFactoryI* SqliteDatabase::getFactory()
{
	if ( !factory_ )
	{
		if ( decorated_ )
			return factory_ = new SqliteFactory(*this, config_, decorated_->getFactory());
		else
			return factory_ = new SqliteFactory(*this, config_);
	}
	else
		return factory_;
}


void SqliteDatabase::createDatabase()
{
#ifndef EVOL_NDEBUG
	Debug::trace("database", 20, "Attempting to create the database ...");
#endif

	if ( decorated_ )
		decorated_->createDatabase();
	else
	{
		// Determine the default db directory
		const string* defaultPath = config_.getProperty("db.SQLite.defaultDB.path");
		if ( !defaultPath )
			throw DBException(DBException::SQLITE_CONFIG_MISSING, "There is no SQLite.defaultDB.path configuration parameter.");

		// Determine the current db directory
		const string* dbPath = config_.getProperty("db.SQLite.path");
		if ( !dbPath )
			throw DBException(DBException::SQLITE_CONFIG_MISSING, "There is no SQLite.path configuration parameter.");

		// Copy the contents of the default database directory over the current one
		util_filesystem::Directory defdir(*defaultPath), currdir(*dbPath);
		if ( !(currdir << defdir) )
			throw DBException(DBException::SQLITE_CREATE_DB,
					"Unable to copy the default database files (" + *defaultPath + ") into the database directory (" + *dbPath + ").");
	}
}


int handleBusy(void *sqlite3_ptr, int numberOfTimesCalled)
{
	Debug::trace("database.busy", 24, "handleBusy - busy handler called for " + ptrtostring(sqlite3_ptr) + "  number of times called: "  + inttostring(numberOfTimesCalled) + " times");
	PAL_absclass::PAL_sleep(100);
	return 1;
}


bool SqliteDatabase::returnCodeIndicatesCorruption(int result)
{
	switch (result)
	{
		case SQLITE_CORRUPT:
		case SQLITE_CANTOPEN:
		case SQLITE_NOTADB:
			return true;
			break;

		default:
			return false;
	}
}


void SqliteDatabase::openDatabase()
{
#ifndef EVOL_NDEBUG
	Debug::trace("database", "Attempting to open the database ...");
#endif

	if ( decorated_ )
		decorated_->openDatabase();
	else
	{
		// Verify that the database exists in the right place.
		if ( !factory_ )
			getFactory();

		if (!db_)
		{
			// Determine the current db directory
			const string* dbPath = config_.getProperty("db.SQLite.path");
			if ( !dbPath )
				throw DBException(DBException::SQLITE_CONFIG_MISSING, "There is no SQLite.path configuration parameter.");

			util_filesystem::File dbFile(*dbPath); // (dbDestinationDir.getFullPath(), filename);
			if (!dbFile.exists())
				throw DBException(DBException::SQLITE_CONFIG_MISSING, "The database file does not exist: " + *dbPath);

			int openResult = sqlite3_open(dbPath->c_str(), &db_);
			Debug::trace("database", "open result: " + inttostring(openResult) + "  sqlite3*:" + ptrtostring(db_));

			// This was in the constructor for the SqliteDatabase class.  Had to move it here since
			// we didn't have the db_ attribute to register until the sqlite3_open call directly above.
			if (openResult == SQLITE_OK)
			{
				SqliteDatabaseSingleton::getInstance().registerSqliteDatabase(this, db_);

				//int result = sqlite3_busy_handler(db_, &handleBusy, db_);
				//Debug::trace("database.busy", "Called sqlite3_busy_handler to set a busy handler, result: " + inttostring(result));
			}


			// If the AUTOINC table is not present or is corrupt there will be an exception thrown.
#ifndef EVOL_NDEBUG
			Debug::trace("database", 24, "Attempting to initialize the AutoIncrement table.");
#endif
			SqliteAutoIncrementsRM::openAll();


			const string* property = config_.getProperty("db.SQLite.checkOnOpen");
			if ( property && stringtobool(*property) )
			{
				std::string resultExplanation;
#ifndef EVOL_NDEBUG
				Debug::trace("database", 24, "Going to check database.");

				bool result = 
#endif
				checkDatabase(resultExplanation);

#ifndef EVOL_NDEBUG
				Debug::trace("database", 24, "Database check " + std::string(result ? "succeeded." : "failed."));
#endif
			}
		}
		else
			Debug::trace("database", "Database already open -  sqlite3*:" + ptrtostring(db_));
	}
}


void SqliteDatabase::compressDatabase()
{
#ifndef EVOL_NDEBUG
	Debug::trace("database", 20, "Attempting to compress the database ...");
#endif

	if ( decorated_ )
		decorated_->compressDatabase();
	else
	{
		if ( factory_ )
		{
			bool result = true;



			if ( !result )
				throw DBException(DBException::SQLITE_COMPRESS_TABLE, " database compress failed.");
		}
	}
}


void SqliteDatabase::copyDatabase(bool from, const std::string& path)
{
	if ( decorated_ )
		decorated_->copyDatabase(from, path);
	else
	{
	
	
	
	}
}


void SqliteDatabase::backupDatabase(const std::string& path)
{
	if ( decorated_ )
		decorated_->backupDatabase(path);
	else
	{
		if ( factory_ )
		{
			bool result = true;
			
			

			if ( !result )
				throw DBException(DBException::SQLITE_BACKUP_TABLE, " database backup failed.");
		}
	}
}


void SqliteDatabase::closeDatabase()
{
    if( !db_ )//Trying to close a database that isn't open
	{
		Debug::trace("database", "Attempting to close a database that isn't open! ignoring...");
        return;
	}
#ifndef EVOL_NDEBUG
	Debug::trace("database", "Attempting to close the database -  sqlite3*:" + ptrtostring(db_));
#endif

	if ( decorated_ )
		decorated_->closeDatabase();
	else
	{
		// Close the AutoIncrementer tables
		SqliteAutoIncrementsRM::closeAll();
        
        int result = sqlite3_wal_checkpoint(db_, NULL);//Doesn't need the name of the db
        Debug::trace("database", "wal_checkpoint result:" + inttostring(result));

		result = sqlite3_close(db_);
		//Debug::trace("database", 1, "close result: " + inttostring(result));
        Debug::trace("database", "close result: " + inttostring(result));

		if ( result != SQLITE_OK )
		{
			// throw DBException(DBException::SQLITE_CLOSE_TABLE, " database close failed.  result=" + util_string::inttostring(result));
		}

		db_ = 0;	
	}
}


void SqliteDatabase::deleteDatabase()
{
	if ( decorated_ )
		decorated_->deleteDatabase();
	else
	{
	
	
	
	}
}


/* The string reference passed in allows this method to return a more precise explanation for the result of the integrity check.
 */
bool SqliteDatabase::checkDatabase(std::string &resultExplanation)
{
	bool retVal = true;

	if ( decorated_ )
		retVal = decorated_->checkDatabase(resultExplanation);
	else
	{
		resultExplanation = "";

		if ( !db_ )
			openDatabase();

		if ( !db_ )
		{
			resultExplanation = "Database Integrity Check - No database - FAILED";
			//MessageLogger::logMessage(MessageLogger::InfoLog, "Database Integrity Check - No database - FAILED");
			return false;
		}

#ifndef EVOL_NDEBUG
		util_time::Time t0 = util_time::Time();
#endif

		bool isTracingSql = Debug::isTracing("database.sql");
		bool isTracingSqlSuccess = Debug::isTracing("database.sql.success");

		string dbgHeader = "SDb::checkDatabase - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

		std::string sql = "pragma integrity_check";

		const string* maxMsgs = config_.getProperty("db.SQLite.integrityCheckMaxMsgs");
		if ( maxMsgs )
			sql.append("(").append(*maxMsgs).append(")");

		if (isTracingSql)
			Debug::trace("database.sql", dbgHeader + sql);

		string queryResult;
		int result;
		bool looping = true;
		int numberOfTimeslocked = 0;
		while (looping)
		{
			// Assume we're not going to need to loop
			looping = false;

			sqlite3_stmt *stmt = 0;

			result = sqlite3_prepare_v2(db_, sql.c_str(), (int)sql.length()+1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				if (isTracingSqlSuccess)
					Debug::trace("database.sql.success", dbgHeader + "sqlite3_prepare_v2 succeeded - " + sql);
			}
			else
			{
				if (isTracingSql)
					Debug::trace("database.sql", dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);

				resultExplanation = "Database Integrity Check - Could not perform integrity check - code: " + inttostring(result);

				if ( returnCodeIndicatesCorruption(result) )
				{
					resultExplanation.append("  Error: database disk image is malformed");
					//MessageLogger::logMessage(MessageLogger::InfoLog, resultExplanation);

					//throw edb::DBException(edb::DBException::SQLITE_FIND_RECORD_KEY, "SQLite Error preparing integrity check query - result:" + inttostring(result) + " - " + sql);
					// For now, assume integrity check has failed
					return false;
				}
				else
				{
					resultExplanation.append(" - OK");
					//MessageLogger::logMessage(MessageLogger::InfoLog, resultExplanation);

					//throw edb::DBException(edb::DBException::SQLITE_FIND_RECORD_KEY, "SQLite Error preparing integrity check query - result:" + inttostring(result) + " - " + sql);
					// For now, assume integrity check is OK
					return true;
				}
			}

			result = sqlite3_step(stmt);
			if ( result == SQLITE_ROW )
			{
				if (isTracingSqlSuccess)
					Debug::trace("database.sql.success", dbgHeader + "succeeded - " + sql);

				// Since we got at least one row, initialize return value to false.
				retVal = false;

				bool moreRows = true;
				while (moreRows)
				{
					char *rowData = (char *)sqlite3_column_text(stmt, 0);
					if (rowData)  // shouldn't be null, but check anyway to be safe.
					{
						queryResult.assign(rowData);
						if ( queryResult == "ok" || queryResult == "OK" )
						{
							retVal = true;
							moreRows = false;
							queryResult.insert(0, "Database Integrity Check - ");
							//MessageLogger::logMessage(MessageLogger::InfoLog, queryResult);
							Debug::trace("database", 24, queryResult);
						}
						else
						{
							queryResult.insert(0, "Database Integrity Check - ");
							resultExplanation.append(queryResult);
							//MessageLogger::logMessage(MessageLogger::InfoLog, queryResult);
							Debug::trace("database", 24, queryResult);
						}
					}
					else
					{
						moreRows = false;
						// If didn't get a response to the integrity_check query, assume db is OK, not corrupt.
						retVal = true;
						resultExplanation = "Database Integrity Check - No response - OK";
						//queryResult = "Database Integrity Check - No response - OK";
						//MessageLogger::logMessage(MessageLogger::InfoLog, queryResult);
					}

					if (moreRows)
					{
						result = sqlite3_step(stmt);
						if ( result != SQLITE_ROW )
							moreRows = false;
					}
				}
			}
			else if ( result == SQLITE_BUSY || result == SQLITE_IOERR_BLOCKED )
			{
				if (numberOfTimeslocked++ < MAX_LOCK_COUNT)
					looping = true;
				else
				{
					retVal = true;
					resultExplanation = "Database Integrity Check busy - result:" + inttostring(result) + " - assuming OK";
					//MessageLogger::logMessage(MessageLogger::InfoLog, "Database Integrity Check busy - result:" + inttostring(result) + " - assuming OK");
				}

				Debug::trace("database.busy", dbgHeader + "busy - lockedCount:" + inttostring(numberOfTimeslocked) + " - " + sql);
				PAL_absclass::PAL_sleep(100);
			}
			else
			{
				retVal = true;
				resultExplanation = "Database Integrity Check not determined - result:" + inttostring(result) + " - assuming OK";
				//MessageLogger::logMessage(MessageLogger::InfoLog, "Database Integrity Check not determined - result:" + inttostring(result) + " - assuming OK");
				if (isTracingSql)
					Debug::trace("database.sql", dbgHeader + "failed - result:" + inttostring(result) + " - " + sql);
			}
		
			result = sqlite3_finalize(stmt);
			if (Debug::isTracing("database.sql.finalize"))
				Debug::trace("database.sql.finalize", dbgHeader + "finalize - result:" + inttostring(result) + " - " + sql);
		}
		
#ifndef EVOL_NDEBUG
		util_time::Time t1 = util_time::Time();
		Debug::trace("database.timing.findRecord", dbgHeader + "integrity check took " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif
	}

	return retVal;
}


bool SqliteDatabase::checkDatabaseIndices(bool rebuildIndicesEnabled)
{
	bool retVal = true;

	if ( decorated_ )
		retVal = decorated_->checkDatabaseIndices(rebuildIndicesEnabled);
	else
	{
	
	
	
	}
	return retVal;
}


void SqliteDatabase::setDatabaseLanguage(unsigned int languageNumber, const std::string& languageCode)
{
	SqliteDatabaseSingleton::getInstance().setLanguage(languageNumber, languageCode);
}


void SqliteDatabase::show()
{
#ifndef EVOL_NDEBUG
	Debug::trace("database.show", 20, "   SQLite pointer is " + ptrtostring(getDB()));

	Debug::trace("database.show", 20, "   Auto-Increment values are " + std::string(isCheckAutoIncrementValues() ? "CHECKED" : "NOT CHECKED"));
#endif
}


std::string SqliteDatabase::getLanguageCode(unsigned int language) const
{
	return SqliteDatabaseSingleton::getInstance().getLanguage(language);
}


unsigned int SqliteDatabase::getCurrentLocaleLanguageNumber() const
{
	std::string reverseLocale;
	util_platform::Platform::getInstance().reverseLocaleString(reverseLocale, locale().name());
	unsigned int number = SqliteDatabaseSingleton::getInstance().getLanguageNumber(reverseLocale);

	return (number == 0 ? 1 : number);
}


SqliteProductsRM* SqliteDatabase::getProductsRM() const
{
	return (const_cast<SqliteFactory*>(factory_))->createProducts(); 
}


string SqliteDatabase::getAutoIncrementValue(const string& field) const
{
	if (!autoIncrementer_)
		return "";
	return (*autoIncrementer_)(field);
}


void SqliteDatabase::ensureAutoIncrementValue(const std::string& field, int value) const
{
	if (!autoIncrementer_)
		return;
	autoIncrementer_->ensureAutoIncrementValue(field, value);
}


void SqliteDatabase::showAll()
{
	SqliteDatabaseSingleton::getInstance().show();
}


int SqliteDatabase::sendSqliteError(int errorCode1, long errorCode2, const std::string& desc1,
		const std::string& desc2, const std::string& desc3) const
{
	//lap todo?
	return 0;  // db_->error(errorCode1, errorCode2, desc1.c_str(), desc2.c_str(), desc3.c_str());
}



// --- SqliteDatabaseSingleton class ---
// Definition/allocation of static members.
Mutex<> SqliteDatabaseSingleton::_singletonMutex_ = Mutex<>();
SqliteDatabaseSingleton* SqliteDatabaseSingleton::_sqliteimp_ = &SqliteDatabaseSingleton::getInstance();


// Singleton method
SqliteDatabaseSingleton& SqliteDatabaseSingleton::getInstance()
{
	if ( _sqliteimp_ == 0 )
	{
		Lock< Mutex<> > lock(_singletonMutex_);
		if ( _sqliteimp_ == 0 )
		{
			_sqliteimp_ = new SqliteDatabaseSingleton();
			_sqliteimp_->setLanguage(1, "en");
			_sqliteimp_->setLanguage(2, "es");
		}
	}

	return *_sqliteimp_;
}


unsigned int SqliteDatabaseSingleton::getLanguageNumber(const std::string& languageCode)
{
	Lock< Mutex<> > lock(languageMutex_); 
	for ( unsigned int i = 1; i < languages_.size(); ++i )
	{	
		if ( languages_[i] == languageCode )
			return i;
	}

	return 0;
}


bool SqliteDatabaseSingleton::registerSqliteDatabase(SqliteDatabase* s, sqlite3 *s3)
{
	Lock< Mutex<> > lock(registryMutex_);

	if ( s && s3 )
	{
		pair<sqliteDatabaseContainerType::iterator, bool> result = registry_.insert(make_pair(s3, s));

		Debug::trace("database", std::string(result.second ? "Registered" : "Unable to register") +
			" SqliteDatabase " + util_string::ptrtostring(s) + " with sqlite* " + util_string::ptrtostring(s3));

		return result.second;
	}
	else
	{
		Debug::trace("database", "Did not register SqliteDatabase " + util_string::ptrtostring(s) +
				" with sqlite* " + util_string::ptrtostring(s3) + " (at least one is null)");

		return false;
	}
}


bool SqliteDatabaseSingleton::deregisterSqliteDatabase(SqliteDatabase* s)
{
	bool result = false;

	{
		Lock< Mutex<> > lock(registryMutex_);

		sqliteDatabaseContainerType::iterator iter = registry_.begin();
		while ( iter != registry_.end() )
		{
			if ( iter->second == s )
			{
				registry_.erase(iter);
				result = true;

				break;
			}

			++iter;
		}
	}

	if ( result )
		Debug::trace("database", "Deregistered SqliteDatabase " + util_string::ptrtostring(s));
	else
		Debug::trace("database", 11, "Failed to deregister SqliteDatabase " +
				util_string::ptrtostring(s) + " (not found)");

	return result;
}


SqliteDatabase* SqliteDatabaseSingleton::getRegisteredSqliteDatabase(sqlite3* s3)
{
	Lock< Mutex<> > lock(registryMutex_);

	sqliteDatabaseContainerType::iterator iter = registry_.find(s3);
	if ( iter != registry_.end() )
		return iter->second;

	return 0;
}


void SqliteDatabaseSingleton::show()
{
#ifndef EVOL_NDEBUG
	Debug::trace("database.show", 20, "");

	// Show the database languages as they currently are set
	{
		Lock< Mutex<> > lock(languageMutex_);

		for ( unsigned int i = 0; i < languages_.size(); ++i )
			Debug::trace("database.show", 20, "Database language " + uinttostring(i) + " is set to '" + languages_[i] + "'");
	}

	Debug::trace("database.show", 20, "");
#endif
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
