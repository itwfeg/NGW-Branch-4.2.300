#ifndef SQLITESPECIALPROGRAM_H
#define SQLITESPECIALPROGRAM_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBSPECIALPROGRAMI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteSpecialProgram : public DBSpecialProgramI
	{
		public:

			// Types and subtypes
			typedef DBSpecialProgramI interface_type;

			
			// Constructors and destructor
			SqliteSpecialProgram(SqliteRecord* r, DBSpecialProgramI* d = 0);
			~SqliteSpecialProgram();


			// Think about replacing this method with two friend functions from the
			// SqliteSpecialProgramsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBSpecialProgramI
			virtual DBSpecialProgramI* clone() const { return new SqliteSpecialProgram(new SqliteRecord(*rec_)); }
			virtual const DBSpecialProgramI* getDecorated() const { return decorated_; }

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

			virtual std::string getDescription() const { return rec_->getField(DESCRIPTION1_FIELD); }
			virtual void setDescription(const std::string& desc) { rec_->setField(DESCRIPTION1_FIELD, desc); }
			virtual bool isDescriptionNull() const { return rec_->isFieldNull(DESCRIPTION1_FIELD); }
			virtual void setDescriptionNull() { rec_->setFieldNull(DESCRIPTION1_FIELD); }
			virtual bool isDescriptionWritable() const { return true; }

			// DESCRIPTION2_FIELD

			virtual int getDefaultLength() const { return rec_->getField(DEFAULTLENGTH_FIELD); }
			virtual void setDefaultLength(int length) { rec_->setField(DEFAULTLENGTH_FIELD, length); }
			virtual bool isDefaultLengthNull() const { return rec_->isFieldNull(DEFAULTLENGTH_FIELD); }
			virtual void setDefaultLengthNull() { rec_->setFieldNull(DEFAULTLENGTH_FIELD); }
			virtual bool isDefaultLengthWritable() const { return true; }

			virtual std::string getImage1() const { return rec_->getField(IMAGE1_FIELD); }
			virtual void setImage1(const std::string& desc) { rec_->setField(IMAGE1_FIELD, desc); }
			virtual bool isImage1Null() const { return rec_->isFieldNull(IMAGE1_FIELD); }
			virtual void setImage1Null() { rec_->setFieldNull(IMAGE1_FIELD); }
			virtual bool isImage1Writable() const { return true; }

			virtual std::string getImage2() const { return rec_->getField(IMAGE2_FIELD); }
			virtual void setImage2(const std::string& desc) { rec_->setField(IMAGE2_FIELD, desc); }
			virtual bool isImage2Null() const { return rec_->isFieldNull(IMAGE2_FIELD); }
			virtual void setImage2Null() { rec_->setFieldNull(IMAGE2_FIELD); }
			virtual bool isImage2Writable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string DESCRIPTION1_FIELD;
			static const std::string DESCRIPTION2_FIELD;
			static const std::string DEFAULTLENGTH_FIELD;
			static const std::string IMAGE1_FIELD;
			static const std::string IMAGE2_FIELD;


		private:

			SqliteRecord* rec_;
			DBSpecialProgramI* decorated_;

			// Do not allow the default copy constructor.
			SqliteSpecialProgram(const SqliteSpecialProgram& rec);
			// Do not allow the default assignment operator.
			SqliteSpecialProgram operator=(const SqliteSpecialProgram& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESPECIALPROGRAM_H
