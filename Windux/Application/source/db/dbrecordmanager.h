#ifndef DBRECORDMANAGER_H
#define DBRECORDMANAGER_H


#include "edb.h"
#include DBACCESSENTITY_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;


	class DBRecordManager : public DBAccessEntity
	{
		public:

			// --- Constructors and destructor ---
			virtual ~DBRecordManager() {}


			// --- Accessor methods ---
			DBAccessEntityType getAccessEntityType() const { return DBAccessEntityRecordManager; }
			virtual bool isEnabled() const = 0;
			virtual DBManager* getDBManager() const = 0;
			virtual DBAccessEntity::DBRecordType getRecordType() const = 0;


			// --- Processing methods for other database operations ---
			virtual int deleteAll() const = 0;
			virtual int countAll() const = 0;
			virtual int calculateAverageSpace() const = 0;
			virtual bool execute(const std::string sql) const = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBRECORDMANAGER_H
