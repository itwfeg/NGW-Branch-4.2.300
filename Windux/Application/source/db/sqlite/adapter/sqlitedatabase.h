#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H


#include "edb.h"
#include <sqlite3.h>

#include DBDATABASEI_INC
#include PROPERTIES_INC
#include MUTEX_INC
#include FILENAME_INC


#define MAX_LOCK_COUNT 100


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBManagerI;
	class DBFactoryI;
	class SqliteFactory;
	class SqliteProductsRM;
	class SqliteAutoIncrementsRM;


	// SqliteDatabase implements the DBDatabaseI interface around the SQLite library
	// providing a complete database layer in our database component.
	class SqliteDatabase : public DBDatabaseI
	{
		public:

			// --- Constructors and destructor ---
			SqliteDatabase(const DBManagerI& manager, bool alreadyOpen, DBDatabaseI* db = 0);
			virtual ~SqliteDatabase();


			// --- Getter and setter methods ---
			sqlite3* getDB() const { return db_; }
			bool isCheckAutoIncrementValues() const { return checkAutoIncrementKeys_; }


			// --- Processing methods ---
			// Methods from DBDatabaseI
			DBFactoryI* getFactory();

			bool returnCodeIndicatesCorruption(int result);

			void createDatabase();
			void openDatabase();
			//int handleBusy(void *, int numberOfTimesCalled);
			void compressDatabase();
			void copyDatabase(bool from, const std::string& path);
			void backupDatabase(const std::string& path);
			void closeDatabase();
			void deleteDatabase();
			bool checkDatabase(std::string &resultExplanation);
			bool checkDatabaseIndices(bool rebuildIndicesEnabled);

			void setDatabaseLanguage(unsigned int languageNumber, const std::string& languageCode);

			int sendSqliteError(int errorCode1, long errorCode2, const std::string& desc1,
					const std::string& desc2, const std::string& desc3) const;

			void show();


			// Other methods

			// Returns the language code (in order to instantiate the corresponding
			// locale) associated with a given ordinal
			std::string getLanguageCode(unsigned int language) const;
			// Returns the ordinal number associated with the current locale so
			// as to decide which name (typically) attribute to use for sorting.
			unsigned int getCurrentLocaleLanguageNumber() const;

			// Gets the new auto-increment value for the corresponding auto-increment field
			std::string getAutoIncrementValue(const std::string& field) const;
			// Ensure auto-increment value is consistent with new value
			void ensureAutoIncrementValue(const std::string& field, int value) const;

			// Factory method for Products Record Manager
			SqliteProductsRM* getProductsRM() const;


			// --- Class methods ---
			static void showAll();


		private:

			// --- Instance data members ---
			const DBManagerI& manager_;                 // This is the associated DBManager object
			const util_env::Properties& config_;        // Access to database properties/configuration
			sqlite3* db_;
			SqliteFactory* factory_;                    // Factory object
			bool checkAutoIncrementKeys_;               // Enables checking for repeated autoincrement values
			SqliteAutoIncrementsRM* autoIncrementer_;   // Database table that tracks autoincrement values
			DBDatabaseI* decorated_;                    // Pointer to next layers database manager


			// --- Instance function members ---
			// Do not allow the default copy constructor.
			SqliteDatabase(const SqliteDatabase& db);
			// Do not allow the default assignment operator.
			SqliteDatabase operator=(const SqliteDatabase& rhs);


			// --- Class data members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEDATABASE_H
