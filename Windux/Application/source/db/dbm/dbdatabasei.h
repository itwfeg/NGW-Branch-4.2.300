#ifndef DBDATABASEI_H
#define DBDATABASEI_H



#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif


	class DBFactoryI;

	
	// Interface that each database layer needs to implement in a kind of 'database
	// manager' class in order to have all the database layers interacting smoothly
	class DBDatabaseI
	{
		public:
			virtual ~DBDatabaseI() {}


			// Returns a pointer to the current database layer's factory class (the
			// class that implements the DBFactoryI interface).
			virtual DBFactoryI* getFactory() = 0;


			// --- Database operations methods ---
			virtual void createDatabase() = 0;

			// Upon returning from this method, the database will be open (it may
			// have already been open before) so that the application can access
			// the data (through record managers) and perform database operations
			// (through the database manager) allowed while the database is open
			// (backup, close, and language).
			virtual void openDatabase() = 0;

			// Compresses the physical database files to remove deleted records.
			virtual void compressDatabase() = 0;

			// Copies all the database files.
			// Parameter "from": true means copy from db directory to path,
			// false means copy from path to db directory (not very intuitive!?)
			virtual void copyDatabase(bool from, const std::string& path) = 0;

			// Backs up the database file into 'path' directory. It only backs
			// up those files that have changed since the last backup.
			virtual void backupDatabase(const std::string& path) = 0;

			// Upon returning from this method, the database will be closed (it may
			// have already been closed before) so that the application cannot
			// access the data any longer, and only perform database operations
			// allowed while the database is closed (create, open, compress, copy,
			// backup, delete, check, and language).
			virtual void closeDatabase() = 0;

			// Deletes the database files.
			virtual void deleteDatabase() = 0;

			// Verifies the integrity of the database, both indices and data.
			virtual bool checkDatabase(std::string &resultExplanation) = 0;
			virtual bool checkDatabaseIndices(bool rebuildIndicesEnabled) = 0;

			// Sets the database languages to be supported in the database. This
			// is done so that queries return data sorted according to the locale
			// corresponding to the associated languages.
			virtual void setDatabaseLanguage(unsigned int languageNumber, const std::string& languageCode) = 0;

			// Prints information associated with the corresponding database
			// manager instance.
			virtual void show() = 0;
	};



#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif



#endif // DBDATABASEI_H

