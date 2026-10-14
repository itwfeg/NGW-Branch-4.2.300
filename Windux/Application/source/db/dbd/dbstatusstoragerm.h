#ifndef DBSTATUSSTORAGERM_H
#define DBSTATUSSTORAGERM_H


#include <string>
#include <map>


#include "edb.h"
#include DBSTATUSRMI_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBStatusStorageRM : public edb_adapter::DBStatusRMI
	{
			friend class DBStatusQuery;
			friend class DBStatusRM;

		public:

			// --- Types and subtypes ---
			typedef std::map<std::string, std::string> statusContainerType;
			typedef statusContainerType::size_type size_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			virtual ~DBStatusStorageRM();


			// --- Accessor methods ---


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			edb_adapter::DBQueryI<edb_adapter::DBStatusI>* retrieveAll();

			// findName: a null return means "unable to find the Status record".
			edb_adapter::DBStatusI* findName(const std::string& name);

			edb_adapter::DBStatusI* createRecord();
			void saveRecord(const edb_adapter::DBStatusI* st);
			int countAll();
			int calculateAverageSpace() { return 0; }
			bool execute(const std::string sql);


			static std::string* getStatus(const std::string& field);
			static void setStatus(const std::string& field, const std::string& value);
			static void setStatusNull(const std::string& field);
			static void copyStatus(const std::string& destField, const std::string& srcField);


		private:

			// --- Data members ---
			static statusContainerType _statusContainer_;
			static util_thread::Mutex<> _statusContainerMutex_;


			// --- Constructors and destructor ---
			DBStatusStorageRM();
			// Not implemented methods (linker will complain if called)
			DBStatusStorageRM(const DBStatusStorageRM& trm);           // Copy constructor
			DBStatusStorageRM operator=(const DBStatusStorageRM& rhs); // Assignment operator


			// Methods used by the friend classes
			static edb_adapter::DBStatusI* first();
			static edb_adapter::DBStatusI* next(const edb_adapter::DBStatusI* current);
			static edb_adapter::DBStatusI* previous(const edb_adapter::DBStatusI* current);
			static edb_adapter::DBStatusI* last();
			static edb_adapter::DBStatusI* find(const edb_adapter::DBStatusI* current);
			static size_type size();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSTATUSSTORAGERM_H
