#ifndef SQLITEPRODUCTDETAILEDSTATISTIC_H
#define SQLITEPRODUCTDETAILEDSTATISTIC_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBPRODUCTDETAILEDSTATISTICI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteProductDetailedStatistic : public DBProductDetailedStatisticI
	{
		public:

			// Types and subtypes
			typedef DBProductDetailedStatisticI interface_type;


			// Constructors and destructor
			SqliteProductDetailedStatistic(SqliteRecord* r, DBProductDetailedStatisticI* d = 0);
			~SqliteProductDetailedStatistic();


			// Think about replacing this method with two friend functions from the
			// SqliteProductDetailedStatisticsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBProductDetailedStatisticI
			virtual DBProductDetailedStatisticI* clone() const { return new SqliteProductDetailedStatistic(new SqliteRecord(*rec_)); }
			virtual const DBProductDetailedStatisticI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getProductKey() const { return rec_->getField(PRODUCTKEY_FIELD); }
			virtual void setProductKey(const std::string& plu) { rec_->setField(PRODUCTKEY_FIELD, plu); }
			virtual bool isProductKeyNull() const { return rec_->isFieldNull(PRODUCTKEY_FIELD); }
			virtual bool isProductKeyWritable() const { return true; }

			virtual std::string getProductStatisticLanguage() const { return rec_->getField(PRODUCTSTATISTICLANGUAGE_FIELD); }
			virtual void setProductStatisticLanguage(const std::string& language) { rec_->setField(PRODUCTSTATISTICLANGUAGE_FIELD, language); }
			virtual bool isProductStatisticLanguageNull() const { return rec_->isFieldNull(PRODUCTSTATISTICLANGUAGE_FIELD); }
			virtual bool isProductStatisticLanguageWritable() const { return true; }

			virtual std::string getProductStatisticHourNumber() const { return rec_->getField(PRODUCTSTATISTICHOURNUMBER_FIELD); }
			virtual void setProductStatisticHourNumber(const std::string& hour) { rec_->setField(PRODUCTSTATISTICHOURNUMBER_FIELD, hour); }
			virtual bool isProductStatisticHourNumberNull() const { return rec_->isFieldNull(PRODUCTSTATISTICHOURNUMBER_FIELD); }
			virtual bool isProductStatisticHourNumberWritable() const { return true; }

			virtual std::string getPresentationMethodNumber() const { return rec_->getField(PRESENTATIONMETHODTYPE_FIELD); }
			virtual void setPresentationMethodNumber(const std::string& number) { rec_->setField(PRESENTATIONMETHODTYPE_FIELD, number); }
			virtual bool isPresentationMethodNumberNull() const { return rec_->isFieldNull(PRESENTATIONMETHODTYPE_FIELD); }
			virtual bool isPresentationMethodNumberWritable() const { return true; }

			virtual int getSelections() const { return rec_->getField(SELECTIONS_FIELD); }
			virtual void setSelections(int selections) { rec_->setField(SELECTIONS_FIELD, selections); }
			virtual void incrementSelections() { rec_->setField(SELECTIONS_FIELD, rec_->getField(SELECTIONS_FIELD) + 1); }
			virtual bool isSelectionsNull() const { return rec_->isFieldNull(SELECTIONS_FIELD); }
			virtual void setSelectionsNull() { rec_->setFieldNull(SELECTIONS_FIELD); }
			virtual bool isSelectionsWritable() const { return true; }

			virtual int getLabelsPrinted() const { return rec_->getField(LABELSPRINTED_FIELD); }
			virtual void setLabelsPrinted(int printed) { rec_->setField(LABELSPRINTED_FIELD, printed); }
			virtual void incrementLabelsPrinted() { rec_->setField(LABELSPRINTED_FIELD, rec_->getField(LABELSPRINTED_FIELD) + 1); }
			virtual bool isLabelsPrintedNull() const { return rec_->isFieldNull(LABELSPRINTED_FIELD); }
			virtual void setLabelsPrintedNull() { rec_->setFieldNull(LABELSPRINTED_FIELD); }
			virtual bool isLabelsPrintedWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string PRODUCTKEY_FIELD;
			static const std::string PRODUCTSTATISTICLANGUAGE_FIELD;
			static const std::string PRODUCTSTATISTICHOURNUMBER_FIELD;
			static const std::string PRESENTATIONMETHODTYPE_FIELD;
			static const std::string SELECTIONS_FIELD;
			static const std::string LABELSPRINTED_FIELD;


		private:

			SqliteRecord* rec_;
			DBProductDetailedStatisticI* decorated_;

			// Do not allow the default copy constructor.
			SqliteProductDetailedStatistic(const SqliteProductDetailedStatistic& rec);
			// Do not allow the default assignment operator.
			SqliteProductDetailedStatistic operator=(const SqliteProductDetailedStatistic& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRODUCTDETAILEDSTATISTIC_H
