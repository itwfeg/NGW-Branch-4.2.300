#ifndef DBFLASHFILESRMI_H
#define DBFLASHFILESRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBFlashFileI;
	template <class DBEntityI> class DBQueryI;


	class DBFlashFilesRMI
	{
		public:

			virtual ~DBFlashFilesRMI() {}


			virtual DBQueryI<DBFlashFileI>* retrieveAll() = 0;

			virtual DBFlashFileI* findName(const std::string& name) = 0;

			virtual DBFlashFileI* createRecord() = 0;
			virtual void saveRecord(const DBFlashFileI* rec) = 0;
			virtual int deleteRecord(const DBFlashFileI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBFLASHFILESRMI_H
