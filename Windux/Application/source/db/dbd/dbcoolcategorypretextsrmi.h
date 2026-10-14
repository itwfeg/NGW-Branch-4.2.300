#ifndef DBCOOLCATEGORYPRETEXTSRMI_H
#define DBCOOLCATEGORYPRETEXTSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolCategoryPreTextI;
	template <class DBEntityI> class DBQueryI;


	class DBCoolCategoryPreTextsRMI
	{
		public:

			virtual ~DBCoolCategoryPreTextsRMI() {}


			virtual DBQueryI<DBCoolCategoryPreTextI>* retrieveAll() = 0;
			virtual DBQueryI<DBCoolCategoryPreTextI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBCoolCategoryPreTextI>* retrieveLinkedtoCoolCategory(const std::string& category, const std::string& deptNum) = 0;
			virtual DBQueryI<DBCoolCategoryPreTextI>* retrieveLinkedtoCoolPreText(const std::string& pretext, const std::string& deptNum) = 0;

			virtual DBCoolCategoryPreTextI* find(const std::string& category, const std::string& pretext, const std::string& deptNum) = 0;

			virtual DBCoolCategoryPreTextI* createRecord(const std::string& category, const std::string& pretext, const std::string& deptNum) = 0;
			virtual void saveRecord(const DBCoolCategoryPreTextI* rec) = 0;
			virtual int deleteRecord(const DBCoolCategoryPreTextI* rec) = 0;
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


#endif // DBCOOLCATEGORYPRETEXTSRMI_H
