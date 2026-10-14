#ifndef DBEXCEPTION_H
#define DBEXCEPTION_H


#include "edb.h"
#include EXCEPTION_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBException : public util_exception::Exception
	{
		public:

			// Types, subtypes, etc.
			enum Type
			{
				CB_OPEN_TABLE,             // Unable to open table
				CB_BACKUP_TABLE,           // Unable to backup table
				CB_COMPRESS_TABLE,         // Unable to compress table
				CB_SELECT_TAG,             // Unable to select tag
				CB_RETRIEVE_TAG,           // Unable to retrieve tag
				CB_UNLOCK,                 // Unable to unlock a table
				CB_QUERY_NOT_INITIALIZED,  // Unable to initialize query

				CB_NAVIGATE_FIRST,         // Unable to navigate to first record of query
				CB_NAVIGATE_NEXT,          // Unable to navigate to next record of query
				CB_NAVIGATE_PREVIOUS,      // Unable to navigate to previous record of query
				CB_NAVIGATE_LAST,          // Unable to navigate to last record of query
				CB_NAVIGATE_FIND,          // Unable to navigate to a record of query
				CB_FIND_SINGLE_RECORD,     // Unable to find single record
				CB_FIND_RECORD_KEY,        // Unable to find record using key value
				CB_FIND_RECORD_TAG,        // Unable to find record using tag value
				CB_SAVE_RECORD,            // Unable to save record
				CB_DELETE_RECORD,          // Unable to delete record

				CB_BAD_VALUE_TYPE,         // Value type inconsistent with field type
				CB_UNKNOWN_FIELD_TYPE,     // Unknown/Not-used field type
				CB_NOT_NULL_FIELD,         // Attempting to set to null a not-null field
				CB_NOT_STORED,             // Unable to store value in database
				
				CB_CONFIG_MISSING,         // Configuration parameter missing
				CB_CREATE_DB,              // Unable to create the database
				CB_DELETE_DB,              // Unable to delete the database
				CB_AUTOINC_REGISTRY,       // Unable to register AutoIncrementer
				CB_NO_AUTOINC_REC,         // Could not navigate to the AutoInc table's record
				CB_AUTOINC_FAILED,         // Failed to generate an autoincrement key value
				CB_CHECK_INDEX,            // An index file checked invalid
				CB_CHECK_RECORD,           // A data file checked invalid
				CB_MISSED_LOCKS,           // Locks not removed and left behind locking some data

				CB_LAYERING_ERROR,         // Record arriving at a layer it does not belong to
				DB_LAYERING_ERROR,         // Record arriving at a layer it does not belong to

				NO_AUTOINCREMENT_TAG,      // Table with autoincrement field, but no autoinc tag
				DATABASE_NOCONFIG,         // Unable to initialize database: no configuration available
				DATABASE_NODEBUG,          // Unable to initialize database: no debugging available
				DATABASE_OPEN,             // Unable to perform operation: the database is open
				DATABASE_CLOSED,           // Unable to perform operation: the database is closed


				SQLITE_OPEN_TABLE,             // Unable to open table
				SQLITE_BACKUP_TABLE,           // Unable to backup table
				SQLITE_COMPRESS_TABLE,         // Unable to compress table
				SQLITE_SELECT_TAG,             // Unable to select tag
				SQLITE_RETRIEVE_TAG,           // Unable to retrieve tag
				SQLITE_UNLOCK,                 // Unable to unlock a table
				SQLITE_QUERY_NOT_INITIALIZED,  // Unable to initialize query

				SQLITE_NAVIGATE_FIRST,         // Unable to navigate to first record of query
				SQLITE_NAVIGATE_NEXT,          // Unable to navigate to next record of query
				SQLITE_NAVIGATE_PREVIOUS,      // Unable to navigate to previous record of query
				SQLITE_NAVIGATE_LAST,          // Unable to navigate to last record of query
				SQLITE_NAVIGATE_FIND,          // Unable to navigate to a record of query
				SQLITE_FIND_SINGLE_RECORD,     // Unable to find single record
				SQLITE_FIND_RECORD_KEY,        // Unable to find record using key value
				SQLITE_FIND_RECORD_TAG,        // Unable to find record using tag value
				SQLITE_SAVE_RECORD,            // Unable to save record
				SQLITE_DELETE_RECORD,          // Unable to delete record

				SQLITE_BAD_VALUE_TYPE,         // Value type inconsistent with field type
				SQLITE_UNKNOWN_FIELD_TYPE,     // Unknown/Not-used field type
				SQLITE_NOT_NULL_FIELD,         // Attempting to set to null a not-null field
				SQLITE_NOT_STORED,             // Unable to store value in database
				
				SQLITE_CONFIG_MISSING,         // Configuration parameter missing
				SQLITE_CREATE_DB,              // Unable to create the database
				SQLITE_DELETE_DB,              // Unable to delete the database
				SQLITE_AUTOINC_REGISTRY,       // Unable to register AutoIncrementer
				SQLITE_NO_AUTOINC_REC,         // Could not navigate to the AutoInc table's record
				SQLITE_AUTOINC_FAILED,         // Failed to generate an autoincrement key value
				SQLITE_CHECK_INDEX,            // An index file checked invalid
				SQLITE_CHECK_RECORD,           // A data file checked invalid
				SQLITE_MISSED_LOCKS,           // Locks not removed and left behind locking some data

				SQLITE_LAYERING_ERROR,         // Record arriving at a layer it does not belong to


				NO_EXCEPTION               //
			};


			// Constructors and destructor
			DBException(Type t, const std::string& what, const std::exception* e = 0);
			virtual ~DBException() throw();

			// Accessor methods
			Type getType() const { return type; }
			

		private:

			Type type;

			// Do not allow the default copy constructor.
//			DBException(const DBException& e);
			// Do not allow the default assignment operator.
			DBException operator=(const DBException& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBEXCEPTION_H
