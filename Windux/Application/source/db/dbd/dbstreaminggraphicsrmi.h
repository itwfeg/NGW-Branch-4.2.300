#ifndef DBSTREAMINGGRAPHICSRMI_H
#define DBSTREAMINGGRAPHICSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBStreamingGraphicI;
	template <class DBEntityI> class DBQueryI;


	class DBStreamingGraphicsRMI
	{
		public:

			virtual ~DBStreamingGraphicsRMI() {}


			virtual DBQueryI<DBStreamingGraphicI>* retrieveAll() = 0;
			virtual DBQueryI<DBStreamingGraphicI>* retrieveRangeNumber(const std::string& first, const std::string& last) = 0;

			virtual DBStreamingGraphicI* findNumber(const std::string& number) = 0;
			virtual DBStreamingGraphicI* findNext(const DBStreamingGraphicI* current) = 0;

			virtual DBStreamingGraphicI* createRecord() = 0;
			virtual void saveRecord(const DBStreamingGraphicI* rec) = 0;
			virtual int deleteRecord(const DBStreamingGraphicI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBSTREAMINGGRAPHICSRMI_H
