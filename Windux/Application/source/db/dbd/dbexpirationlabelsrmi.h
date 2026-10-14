#ifndef DBEXPIRATIONLABELSRMI_H
#define DBEXPIRATIONLABELSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



    class DBExpirationLabelI;
    template <class DBEntityI> class DBQueryI;


    class DBExpirationLabelsRMI
    {
        public:

            virtual ~DBExpirationLabelsRMI() {}


            virtual DBQueryI<DBExpirationLabelI>* retrieveAll() = 0;
			virtual DBQueryI<DBExpirationLabelI>* retrieveAllForDept(const std::string& deptNum) = 0;
            virtual DBQueryI<DBExpirationLabelI>* retrieveLinkedToPLU(const std::string& plu, const std::string& deptNum) = 0;

            virtual DBExpirationLabelI* find(const std::string& key, const std::string& deptNum) = 0;

            virtual DBExpirationLabelI* createRecord() = 0;
            virtual void saveRecord(const DBExpirationLabelI* rec) = 0;
            virtual int deleteRecord(const DBExpirationLabelI* rec) = 0;
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


#endif // DBEXPIRATIONLABELSRMI_H
