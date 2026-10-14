#ifndef DBSOFTKEYSRMI_H
#define DBSOFTKEYSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBSoftKeyI;
	class DBSoftKeyGroupI;
	class DBProductI;
	template <class DBEntityI> class DBQueryI;


	class DBSoftKeysRMI
	{
		public:

			virtual ~DBSoftKeysRMI() {}


			virtual DBQueryI<DBSoftKeyI>* retrieveAll() = 0;
			virtual DBQueryI<DBSoftKeyI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBSoftKeyI>* retrieveGroup(const DBSoftKeyGroupI* skg) = 0;
			virtual DBQueryI<DBSoftKeyI>* retrieveGroup(const std::string& groupNumber, const std::string& deptNum) = 0;
			
			virtual DBSoftKeyI* findSoftKey(const std::string& gn, const std::string& dn, const std::string& hp, const std::string& vp) = 0;

			virtual DBSoftKeyI* createRecord(const std::string& gn, const std::string& dn, const std::string& hp, const std::string& vp) = 0;
			virtual void saveRecord(const DBSoftKeyI* rec) = 0;
			virtual int deleteRecord(const DBSoftKeyI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int deleteAllForDept(const std::string& deptNum) = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBSOFTKEYSRMI_H
