#ifndef SQLITECLASS_H
#define SQLITECLASS_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBCLASSI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteClass : public DBClassI
	{
		public:

			// Types and subtypes
			typedef DBClassI interface_type;

			
			// Constructors and destructor
			SqliteClass(SqliteRecord* r, DBClassI* d = 0);
			~SqliteClass();


			// Think about replacing this method with two friend functions from the
			// SqliteClasssRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBClassI
			virtual DBClassI* clone() const { return new SqliteClass(new SqliteRecord(*rec_)); }
			virtual const DBClassI* getDecorated() const { return decorated_; }

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

			virtual std::string getReportingClass() const { return rec_->getField(REPORTINGCLASS_FIELD); }
			virtual void setReportingClass(const std::string& rc) { rec_->setField(REPORTINGCLASS_FIELD, rc); }
			virtual bool isReportingClassNull() const { return rec_->isFieldNull(REPORTINGCLASS_FIELD); }
			virtual void setReportingClassNull() { rec_->setFieldNull(REPORTINGCLASS_FIELD); }
			virtual bool isReportingClassWritable() const { return true; }

			virtual std::string getDescription() const { return rec_->getField(DESCRIPTION1_FIELD); }
			virtual void setDescription(const std::string& desc) { rec_->setField(DESCRIPTION1_FIELD, desc); }
			virtual bool isDescriptionNull() const { return rec_->isFieldNull(DESCRIPTION1_FIELD); }
			virtual void setDescriptionNull() { rec_->setFieldNull(DESCRIPTION1_FIELD); }
			virtual bool isDescriptionWritable() const { return true; }

			// DESCRIPTION2_FIELD


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string REPORTINGCLASS_FIELD;
			static const std::string DESCRIPTION1_FIELD;
			static const std::string DESCRIPTION2_FIELD;


		private:

			SqliteRecord* rec_;
			DBClassI* decorated_;

			// Do not allow the default copy constructor.
			SqliteClass(const SqliteClass& rec);
			// Do not allow the default assignment operator.
			SqliteClass operator=(const SqliteClass& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECLASS_H
