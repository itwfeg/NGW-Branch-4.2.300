#ifndef DBPRODUCTSEQUENCESRMI_H
#define DBPRODUCTSEQUENCESRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBProductSequenceI;
	class DBProductI;
	template <class DBEntityI> class DBQueryI;


	class DBProductSequencesRMI
	{
		public:

			virtual ~DBProductSequencesRMI() {}


			virtual DBQueryI<DBProductSequenceI>* retrieveAll() = 0;
			virtual DBQueryI<DBProductSequenceI>* retrieveLinkedToProduct(const DBProductI* prod) = 0;
			virtual DBQueryI<DBProductSequenceI>* retrieveCharacterSequence(const std::string& sequence, const std::string& language) = 0;

			virtual DBProductSequenceI* findProductSequence(const DBProductI* prod, const std::string& sequence, const std::string& language) = 0;

			virtual DBProductSequenceI* createRecord(const DBProductI* prod, const std::string& sequence, const std::string& language) = 0;
			virtual void saveRecord(const DBProductSequenceI* rec) = 0;
			virtual int deleteRecord(const DBProductSequenceI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRODUCTSEQUENCESRMI_H
