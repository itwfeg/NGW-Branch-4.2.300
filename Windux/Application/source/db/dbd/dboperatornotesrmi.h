#ifndef DBOPERATORNOTESRMI_H
#define DBOPERATORNOTESRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBOperatorNoteI;
	template <class DBEntityI> class DBQueryI;


	class DBOperatorNotesRMI
	{
		public:

			virtual ~DBOperatorNotesRMI() {}


			virtual DBQueryI<DBOperatorNoteI>* retrieveAll() = 0;
//			virtual DBQueryI<DBOperatorNoteI>* retrieveRange(const std::string& first, const std::string& last) = 0;

			virtual DBOperatorNoteI* findRecord() = 0;

			virtual DBOperatorNoteI* createRecord() = 0;
			virtual void saveRecord(const DBOperatorNoteI* rec) = 0;
			virtual int deleteRecord(const DBOperatorNoteI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBOPERATORNOTESRMI_H
