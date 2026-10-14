#ifndef SQLITECOOLCATEGORYPRODUCTIONSTEP_H
#define SQLITECOOLCATEGORYPRODUCTIONSTEP_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBCOOLCATEGORYPRODUCTIONSTEPI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteCoolCategoryProductionStep : public DBCoolCategoryProductionStepI
	{
		public:

			// Types and subtypes
			typedef DBCoolCategoryProductionStepI interface_type;


			// Constructors and destructor
			SqliteCoolCategoryProductionStep(SqliteRecord* r, DBCoolCategoryProductionStepI* d = 0);
			~SqliteCoolCategoryProductionStep();


			// Think about replacing this method with two friend functions from the
			// SqliteCoolCategoryProductionStepsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBCoolCategoryProductionStepI
			virtual DBCoolCategoryProductionStepI* clone() const { return new SqliteCoolCategoryProductionStep(new SqliteRecord(*rec_)); }
			virtual const DBCoolCategoryProductionStepI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getProductionStepKey() const { return rec_->getField(PRODUCTIONSTEPKEY_FIELD); }
			virtual void setProductionStepKey(const std::string& type) { rec_->setField(PRODUCTIONSTEPKEY_FIELD, type); }
			virtual bool isProductionStepKeyNull() const { return rec_->isFieldNull(PRODUCTIONSTEPKEY_FIELD); }
			virtual bool isProductionStepKeyWritable() const { return true; }

			virtual std::string getDeptNumber() const { return rec_->getField(DEPTNUM_FIELD); }
			virtual void setDeptNumber(const std::string& number) { rec_->setField(DEPTNUM_FIELD, number); }
			virtual bool isDeptNumberNull() const { return rec_->isFieldNull(DEPTNUM_FIELD); }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual std::string getCategoryNumber() const { return rec_->getField(CATEGORYNUMBER_FIELD); }
			virtual void setCategoryNumber(const std::string& category) { rec_->setField(CATEGORYNUMBER_FIELD, category); }
			virtual bool isCategoryNumberNull() const { return rec_->isFieldNull(CATEGORYNUMBER_FIELD); }
			virtual bool isCategoryNumberWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string PRODUCTIONSTEPKEY_FIELD;
			static const std::string CATEGORYNUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;


		private:

			SqliteRecord* rec_;
			DBCoolCategoryProductionStepI* decorated_;

			// Do not allow the default copy constructor.
			SqliteCoolCategoryProductionStep(const SqliteCoolCategoryProductionStep& rec);
			// Do not allow the default assignment operator.
			SqliteCoolCategoryProductionStep operator=(const SqliteCoolCategoryProductionStep& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLCATEGORYPRODUCTIONSTEP_H
