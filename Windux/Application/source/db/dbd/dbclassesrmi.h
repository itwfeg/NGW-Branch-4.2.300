#ifndef DBCLASSESRMI_H
#define DBCLASSESRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



    class DBClassI;
    template <class DBEntityI> class DBQueryI;


    class DBClassesRMI
    {
        public:

            virtual ~DBClassesRMI() {}


            virtual DBQueryI<DBClassI>* retrieveAll() = 0;
			virtual DBQueryI<DBClassI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBClassI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum) = 0;
            virtual DBQueryI<DBClassI>* retrieveChildClasses(const std::string& number, const std::string& deptNum) = 0;

            virtual DBClassI* findNumber(const std::string& number, const std::string& deptNum) = 0;

            virtual DBClassI* createRecord() = 0;
            virtual void saveRecord(const DBClassI* rec) = 0;
            virtual int deleteRecord(const DBClassI* rec) = 0;
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


#endif // DBCLASSESRMI_H
