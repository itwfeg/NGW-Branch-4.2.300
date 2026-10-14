#ifndef DBCOOLLISTTEXTSRMI_H
#define DBCOOLLISTTEXTSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolListTextI;
	class DBCoolListI;
	template <class DBEntityI> class DBQueryI;


	class DBCoolListTextsRMI
	{
		public:

			virtual ~DBCoolListTextsRMI() {}


			virtual DBQueryI<DBCoolListTextI>* retrieveAll() = 0;
			virtual DBQueryI<DBCoolListTextI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBCoolListTextI>* retrieveLink(const std::string& link) = 0;
			virtual DBQueryI<DBCoolListTextI>* retrieveLinkedtoCoolList(const DBCoolListI* list) = 0;
			virtual DBQueryI<DBCoolListTextI>* retrievePreviousCoolListTexts(const std::string& link) = 0;

			virtual DBCoolListTextI* find(const std::string& text, const std::string& deptNum, const DBCoolListI* list) = 0;

			virtual DBCoolListTextI* createRecord(const std::string& text, const DBCoolListI* list, HobRecordNum deptNum) = 0;
			virtual DBCoolListTextI* createRecordInList(const std::string& text, const DBCoolListTextI* listText) = 0;
			virtual void saveRecord(const DBCoolListTextI* rec) = 0;
			virtual int deleteRecord(const DBCoolListTextI* rec) = 0;
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


#endif // DBCOOLLISTTEXTSRMI_H
