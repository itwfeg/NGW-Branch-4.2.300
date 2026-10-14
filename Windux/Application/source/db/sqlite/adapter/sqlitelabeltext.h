#ifndef SQLITELABELTEXT_H
#define SQLITELABELTEXT_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBLABELTEXTI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteLabelText : public DBLabelTextI
	{
		public:

			// Types and subtypes
			typedef DBLabelTextI interface_type;


			// Constructors and destructor
			SqliteLabelText(SqliteRecord* r, DBLabelTextI* d = 0);
			~SqliteLabelText();


			// Think about replacing this method with two friend functions from the
			// SqliteLabelTextsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBLabelTextI
			virtual DBLabelTextI* clone() const { return new SqliteLabelText(new SqliteRecord(*rec_)); }
			virtual const DBLabelTextI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getNumber() const { return rec_->getField(NUMBER_FIELD); }
			virtual void setNumber(const std::string& number) { rec_->setField(NUMBER_FIELD, number); }
			virtual bool isNumberNull() const { return rec_->isFieldNull(NUMBER_FIELD); }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getText() const { return rec_->getField(TEXT_FIELD); }
			virtual void setText(const std::string& text) { rec_->setField(TEXT_FIELD, text); }
			virtual bool isTextNull() const { return rec_->isFieldNull(TEXT_FIELD); }
			virtual void setTextNull() { rec_->setFieldNull(TEXT_FIELD); }
			virtual bool isTextWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string TEXT_FIELD;


		private:

			SqliteRecord* rec_;
			DBLabelTextI* decorated_;

			// Do not allow the default copy constructor.
			SqliteLabelText(const SqliteLabelText& rec);
			// Do not allow the default assignment operator.
			SqliteLabelText operator=(const SqliteLabelText& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITELABELTEXT_H
