#ifndef DBSYNCHRONIZATIONCHANGESRMI_H
#define DBSYNCHRONIZATIONCHANGESRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBSynchronizationChangeI;
	class DBBatchI;
	template <class DBEntityI> class DBQueryI;


	class DBSynchronizationChangesRMI
	{
		public:

			virtual ~DBSynchronizationChangesRMI() {}


			virtual DBQueryI<DBSynchronizationChangeI>* retrieveAll() = 0;
//			virtual DBQueryI<DBSynchronizationChangeI>* retrieveRangeNumber(const std::string& first, const std::string& last) = 0;
			virtual DBQueryI<DBSynchronizationChangeI>* retrieveLinkedToBatch(const DBBatchI* batch) = 0;
			virtual DBQueryI<DBSynchronizationChangeI>* retrieveLinkedToBatch(const std::string& batchNumber) = 0;

			virtual DBSynchronizationChangeI* findIdentifier(const std::string& id) = 0;

			virtual DBSynchronizationChangeI* createRecord(const DBBatchI* batch) = 0;
			virtual DBSynchronizationChangeI* createRecord(const std::string& batchNumber) = 0;
			virtual void saveRecord(const DBSynchronizationChangeI* rec) = 0;
			virtual int deleteRecord(const DBSynchronizationChangeI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBSYNCHRONIZATIONCHANGESRMI_H
