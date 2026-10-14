#ifndef DBBATCHSRMI_H
#define DBBATCHSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBBatchI;
	template <class DBEntityI> class DBQueryI;


	class DBBatchesRMI
	{
		public:

			virtual ~DBBatchesRMI() {}


			virtual DBQueryI<DBBatchI>* retrieveAll() = 0;
			virtual DBQueryI<DBBatchI>* retrieveCompleteBatchesOfOrigin(int origin) = 0;
			virtual DBQueryI<DBBatchI>* retrieveCompleteBatches(int version) = 0;
			virtual DBQueryI<DBBatchI>* retrieveIncompleteBatches() = 0;
			virtual DBQueryI<DBBatchI>* retrievePendingSynchronizationBatches() = 0;
			virtual DBQueryI<DBBatchI>* retrievePendingDeleteBatches() = 0;
			virtual DBQueryI<DBBatchI>* retrieveSameSignatureBatches(const std::string& signature) = 0;
			virtual DBQueryI<DBBatchI>* retrieveRangeNumber(const std::string& first, const std::string& last) = 0;

			virtual DBBatchI* findNumber(const std::string& number) = 0;
			virtual DBBatchI* findVersion(const std::string& version) = 0;

			virtual DBBatchI* createRecord() = 0;
			virtual void saveRecord(const DBBatchI* rec) = 0;
			virtual int deleteRecord(const DBBatchI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;

			virtual void ensureAutoIncrementValue(int value) = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBBATCHSRMI_H
