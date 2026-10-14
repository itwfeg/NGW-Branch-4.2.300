#ifndef SQLITEOPERATOR_H
#define SQLITEOPERATOR_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"
#include "Bool.h"

#include DBOPERATORI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteOperator : public DBOperatorI
	{
		public:

			// Types and subtypes
			typedef DBOperatorI interface_type;

			
			// Constructors and destructor
			SqliteOperator(SqliteRecord* r, DBOperatorI* d = 0);
			~SqliteOperator();


			// Think about replacing this method with two friend functions from the
			// SqliteOperatorsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBOperatorI
			virtual DBOperatorI* clone() const { return new SqliteOperator(new SqliteRecord(*rec_)); }
			virtual const DBOperatorI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getNumber() const { return rec_->getField(NUMBER_FIELD); }
			virtual void setNumber(const std::string& oper) { rec_->setField(NUMBER_FIELD, oper); }
			virtual bool isNumberNull() const { return rec_->isFieldNull(NUMBER_FIELD); }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getRFID() const { return rec_->getField(RFID_FIELD); }
			virtual void setRFID(const std::string& id) { rec_->setField(RFID_FIELD, id); }
			virtual bool isRFIDNull() const { return rec_->isFieldNull(RFID_FIELD); }
			virtual void setRFIDNull() { rec_->setFieldNull(RFID_FIELD); }
			virtual bool isRFIDWritable() const { return true; }

			virtual int getClassification() const { return rec_->getField(CLASSIFICATION_FIELD); }
			virtual void setClassification(int cl) { rec_->setField(CLASSIFICATION_FIELD, cl); }
			virtual bool isClassificationNull() const { return rec_->isFieldNull(CLASSIFICATION_FIELD); }
			virtual void setClassificationNull() { rec_->setFieldNull(CLASSIFICATION_FIELD); }
			virtual bool isClassificationWritable() const { return true; }

			virtual std::string getLanguage() const { return rec_->getField(LANGUAGE_FIELD); }
			virtual void setLanguage(const std::string& lang) { rec_->setField(LANGUAGE_FIELD, lang); }
			virtual bool isLanguageNull() const { return rec_->isFieldNull(LANGUAGE_FIELD); }
			virtual void setLanguageNull() { rec_->setFieldNull(LANGUAGE_FIELD); }
			virtual bool isLanguageWritable() const { return true; }
			
			virtual bool getSawNote() const { return util_string::stringtobool(rec_->getField(SAWNOTE_FIELD)); }
			virtual void setSawNote(bool saw) { rec_->setField(SAWNOTE_FIELD, Bool::toString(saw)); }
			virtual bool isSawNoteNull() const { return rec_->isFieldNull(SAWNOTE_FIELD); }
			virtual void setSawNoteNull() { rec_->setFieldNull(SAWNOTE_FIELD); }
			virtual bool isSawNoteWritable() const { return true; }

			virtual std::string getName() const { return rec_->getField(NAME_FIELD); }
			virtual void setName(const std::string& name) { rec_->setField(NAME_FIELD, name); }
			virtual bool isNameNull() const { return rec_->isFieldNull(NAME_FIELD); }
			virtual void setNameNull() { rec_->setFieldNull(NAME_FIELD); }
			virtual bool isNameWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string RFID_FIELD;
			static const std::string CLASSIFICATION_FIELD;
			static const std::string LANGUAGE_FIELD;
			static const std::string SAWNOTE_FIELD;
			static const std::string NAME_FIELD;


		private:

			SqliteRecord* rec_;
			DBOperatorI* decorated_;

			// Do not allow the default copy constructor.
			SqliteOperator(const SqliteOperator& rec);
			// Do not allow the default assignment operator.
			SqliteOperator operator=(const SqliteOperator& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEOPERATOR_H
