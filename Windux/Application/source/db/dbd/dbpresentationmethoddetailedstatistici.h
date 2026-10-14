#ifndef DBPRESENTATIONMETHODDETAILEDSTATISTICI_H
#define DBPRESENTATIONMETHODDETAILEDSTATISTICI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBPresentationMethodDetailedStatisticI
	{
		public:

			virtual ~DBPresentationMethodDetailedStatisticI() {}


			virtual DBPresentationMethodDetailedStatisticI* clone() const = 0;
			virtual const DBPresentationMethodDetailedStatisticI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getPresentationMethodType() const = 0;
			virtual void setPresentationMethodType(const std::string& type) = 0;
			virtual bool isPresentationMethodTypeNull() const { return false; }
			virtual bool isPresentationMethodTypeWritable() const { return true; }

			virtual std::string getPresentationMethodStatisticHourNumber() const = 0;
			virtual void setPresentationMethodStatisticHourNumber(const std::string& type) = 0;
			virtual bool isPresentationMethodStatisticHourNumberNull() const { return false; }
			virtual bool isPresentationMethodStatisticHourNumberWritable() const { return true; }

			virtual std::string getPresentationMethodStatisticMethodType() const = 0;
			virtual void setPresentationMethodStatisticMethodType(const std::string& type) = 0;
			virtual bool isPresentationMethodStatisticMethodTypeNull() const { return false; }
			virtual bool isPresentationMethodStatisticMethodTypeWritable() const { return true; }

			virtual int getSelections() const = 0;
			virtual void setSelections(int selections) = 0;
			virtual void incrementSelections() = 0;
			virtual bool isSelectionsNull() const { return false; }
			virtual void setSelectionsNull() = 0;
			virtual bool isSelectionsWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRESENTATIONMETHODDETAILEDSTATISTICI_H
