#ifndef SQLITEPRESENTATIONMETHODDETAILEDSTATISTIC_H
#define SQLITEPRESENTATIONMETHODDETAILEDSTATISTIC_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBPRESENTATIONMETHODDETAILEDSTATISTICI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqlitePresentationMethodDetailedStatistic : public DBPresentationMethodDetailedStatisticI
	{
		public:

			// Types and subtypes
			typedef DBPresentationMethodDetailedStatisticI interface_type;


			// Constructors and destructor
			SqlitePresentationMethodDetailedStatistic(SqliteRecord* r, DBPresentationMethodDetailedStatisticI* d = 0);
			~SqlitePresentationMethodDetailedStatistic();


			// Think about replacing this method with two friend functions from the
			// SqlitePresentationMethodDetailedStatisticsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBPresentationMethodDetailedStatisticI
			virtual DBPresentationMethodDetailedStatisticI* clone() const { return new SqlitePresentationMethodDetailedStatistic(new SqliteRecord(*rec_)); }
			virtual const DBPresentationMethodDetailedStatisticI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getPresentationMethodStatisticMethodType() const { return rec_->getField(PRESENTATIONMETHODSTATISTICMETHODTYPE_FIELD); }
			virtual void setPresentationMethodStatisticMethodType(const std::string& method) { rec_->setField(PRESENTATIONMETHODSTATISTICMETHODTYPE_FIELD, method); }
			virtual bool isPresentationMethodStatisticMethodTypeNull() const { return rec_->isFieldNull(PRESENTATIONMETHODSTATISTICMETHODTYPE_FIELD); }
			virtual bool isPresentationMethodStatisticMethodTypeWritable() const { return true; }

			// PRESENTATIONMETHODSTATISTICLANGUAGE_FIELD

			virtual std::string getPresentationMethodStatisticHourNumber() const { return rec_->getField(PRESENTATIONMETHODSTATISTICHOURNUMBER_FIELD); }
			virtual void setPresentationMethodStatisticHourNumber(const std::string& hour) { rec_->setField(PRESENTATIONMETHODSTATISTICHOURNUMBER_FIELD, hour); }
			virtual bool isPresentationMethodStatisticHourNumberNull() const { return rec_->isFieldNull(PRESENTATIONMETHODSTATISTICHOURNUMBER_FIELD); }
			virtual bool isPresentationMethodStatisticHourNumberWritable() const { return true; }

			virtual std::string getPresentationMethodType() const { return rec_->getField(PRESENTATIONMETHODTYPE_FIELD); }
			virtual void setPresentationMethodType(const std::string& number) { rec_->setField(PRESENTATIONMETHODTYPE_FIELD, number); }
			virtual bool isPresentationMethodTypeNull() const { return rec_->isFieldNull(PRESENTATIONMETHODTYPE_FIELD); }
			virtual bool isPresentationMethodTypeWritable() const { return true; }

			virtual int getSelections() const { return rec_->getField(SELECTIONS_FIELD); }
			virtual void setSelections(int selections) { rec_->setField(SELECTIONS_FIELD, selections); }
			virtual void incrementSelections() { rec_->setField(SELECTIONS_FIELD, rec_->getField(SELECTIONS_FIELD) + 1); }
			virtual bool isSelectionsNull() const { return rec_->isFieldNull(SELECTIONS_FIELD); }
			virtual void setSelectionsNull() { rec_->setFieldNull(SELECTIONS_FIELD); }
			virtual bool isSelectionsWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string PRESENTATIONMETHODSTATISTICMETHODTYPE_FIELD;
			static const std::string PRESENTATIONMETHODSTATISTICLANGUAGE_FIELD;
			static const std::string PRESENTATIONMETHODSTATISTICHOURNUMBER_FIELD;
			static const std::string PRESENTATIONMETHODTYPE_FIELD;
			static const std::string SELECTIONS_FIELD;


		private:

			SqliteRecord* rec_;
			DBPresentationMethodDetailedStatisticI* decorated_;

			// Do not allow the default copy constructor.
			SqlitePresentationMethodDetailedStatistic(const SqlitePresentationMethodDetailedStatistic& rec);
			// Do not allow the default assignment operator.
			SqlitePresentationMethodDetailedStatistic operator=(const SqlitePresentationMethodDetailedStatistic& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRESENTATIONMETHODDETAILEDSTATISTIC_H
