#ifndef DBIGNOREWORDSRMI_H
#define DBIGNOREWORDSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBIgnoreWordI;
	template <class DBEntityI> class DBQueryI;


	class DBIgnoreWordsRMI
	{
		public:

			virtual ~DBIgnoreWordsRMI() {}


			virtual DBQueryI<DBIgnoreWordI>* retrieveAll() = 0;

			virtual DBIgnoreWordI* find(const std::string& language, const std::string& name) = 0;

			virtual DBIgnoreWordI* createRecord() = 0;
			virtual void saveRecord(const DBIgnoreWordI* rec) = 0;
			virtual int deleteRecord(const DBIgnoreWordI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBIGNOREWORDSRMI_H
