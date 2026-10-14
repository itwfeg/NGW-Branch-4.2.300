#ifndef SQLITEOPERATORNOTE_H
#define SQLITEOPERATORNOTE_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBOPERATORNOTEI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteOperatorNote : public DBOperatorNoteI
	{
		public:

			// Types and subtypes
			typedef DBOperatorNoteI interface_type;


			// Constructors and destructor
			SqliteOperatorNote(SqliteRecord* r, DBOperatorNoteI* d = 0);
			~SqliteOperatorNote();


			// Think about replacing this method with two friend functions from the
			// SqliteOperatorNotesRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBOperatorNoteI
			virtual DBOperatorNoteI* clone() const { return new SqliteOperatorNote(new SqliteRecord(*rec_)); }
			virtual const DBOperatorNoteI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getText() const { return rec_->getField(TEXT_FIELD); }
			virtual void setText(const std::string& desc) { rec_->setField(TEXT_FIELD, desc); }
			virtual bool isTextNull() const { return rec_->isFieldNull(TEXT_FIELD); }
			virtual void setTextNull() { rec_->setFieldNull(TEXT_FIELD); }
			virtual bool isTextWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string TEXT_FIELD;


		private:

			SqliteRecord* rec_;
			DBOperatorNoteI* decorated_;

			// Do not allow the default copy constructor.
			SqliteOperatorNote(const SqliteOperatorNote& rec);
			// Do not allow the default assignment operator.
			SqliteOperatorNote operator=(const SqliteOperatorNote& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEOPERATORNOTE_H
