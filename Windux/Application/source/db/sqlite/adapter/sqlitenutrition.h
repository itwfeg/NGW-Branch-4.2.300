#ifndef SQLITENUTRITION_H
#define SQLITENUTRITION_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBNUTRITIONI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteNutrition : public DBNutritionI
	{
		public:

			// Types and subtypes
			typedef DBNutritionI interface_type;


			// Constructors and destructor
			SqliteNutrition(SqliteRecord* r, DBNutritionI* d = 0);
			~SqliteNutrition();


			// Think about replacing this method with two friend functions from the
			// SqliteNutritionsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBNutritionI
			virtual DBNutritionI* clone() const { return new SqliteNutrition(new SqliteRecord(*rec_)); }
			virtual const DBNutritionI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getNumber() const { return rec_->getField(NUMBER_FIELD); }
			virtual void setNumber(const std::string& number) { rec_->setField(NUMBER_FIELD, number); }
			virtual bool isNumberNull() const { return rec_->isFieldNull(NUMBER_FIELD); }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const { return rec_->getField(DEPTNUM_FIELD); }
			virtual void setDeptNumber(const std::string& number) { rec_->setField(DEPTNUM_FIELD, number); }
			virtual bool isDeptNumberNull() const { return rec_->isFieldNull(DEPTNUM_FIELD); }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual std::string getNutritionColumn1() const { return rec_->getField(NUTRITIONCOLUMN1_FIELD); }
			virtual void setNutritionColumn1(const std::string& nc1) { rec_->setField(NUTRITIONCOLUMN1_FIELD, nc1); }
			virtual bool isNutritionColumn1Null() const { return rec_->isFieldNull(NUTRITIONCOLUMN1_FIELD); }
			virtual void setNutritionColumn1Null() { rec_->setFieldNull(NUTRITIONCOLUMN1_FIELD); }
			virtual bool isNutritionColumn1Writable() const { return true; }

			virtual std::string getNutritionColumn2() const { return rec_->getField(NUTRITIONCOLUMN2_FIELD); }
			virtual void setNutritionColumn2(const std::string& nc2) { rec_->setField(NUTRITIONCOLUMN2_FIELD, nc2); }
			virtual bool isNutritionColumn2Null() const { return rec_->isFieldNull(NUTRITIONCOLUMN2_FIELD); }
			virtual void setNutritionColumn2Null() { rec_->setFieldNull(NUTRITIONCOLUMN2_FIELD); }
			virtual bool isNutritionColumn2Writable() const { return true; }

			virtual std::string getNutritionColumn3() const { return rec_->getField(NUTRITIONCOLUMN3_FIELD); }
			virtual void setNutritionColumn3(const std::string& nc3) { rec_->setField(NUTRITIONCOLUMN3_FIELD, nc3); }
			virtual bool isNutritionColumn3Null() const { return rec_->isFieldNull(NUTRITIONCOLUMN3_FIELD); }
			virtual void setNutritionColumn3Null() { rec_->setFieldNull(NUTRITIONCOLUMN3_FIELD); }
			virtual bool isNutritionColumn3Writable() const { return true; }

			virtual std::string getNutritionColumn4() const { return rec_->getField(NUTRITIONCOLUMN4_FIELD); }
			virtual void setNutritionColumn4(const std::string& nc4) { rec_->setField(NUTRITIONCOLUMN4_FIELD, nc4); }
			virtual bool isNutritionColumn4Null() const { return rec_->isFieldNull(NUTRITIONCOLUMN4_FIELD); }
			virtual void setNutritionColumn4Null() { rec_->setFieldNull(NUTRITIONCOLUMN4_FIELD); }
			virtual bool isNutritionColumn4Writable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string NUTRITIONCOLUMN1_FIELD;
			static const std::string NUTRITIONCOLUMN2_FIELD;
			static const std::string NUTRITIONCOLUMN3_FIELD;
			static const std::string NUTRITIONCOLUMN4_FIELD;


		private:

			SqliteRecord* rec_;
			DBNutritionI* decorated_;

			// Do not allow the default copy constructor.
			SqliteNutrition(const SqliteNutrition& rec);
			// Do not allow the default assignment operator.
			SqliteNutrition operator=(const SqliteNutrition& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITENUTRITION_H
