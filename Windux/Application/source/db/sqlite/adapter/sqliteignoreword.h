#ifndef SQLITEIGNOREWORD_H
#define SQLITEIGNOREWORD_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBIGNOREWORDI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteIgnoreWord : public DBIgnoreWordI
	{
		public:

			// Types and subtypes
			typedef DBIgnoreWordI interface_type;


			// Constructors and destructor
			SqliteIgnoreWord(SqliteRecord* r, DBIgnoreWordI* d = 0);
			~SqliteIgnoreWord();


			// Think about replacing this method with two friend functions from the
			// SqliteIgnoreWordsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBIgnoreWordI
			virtual DBIgnoreWordI* clone() const { return new SqliteIgnoreWord(new SqliteRecord(*rec_)); }
			virtual const DBIgnoreWordI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getLanguage() const { return rec_->getField(LANGUAGE_FIELD); }
			virtual void setLanguage(const std::string& language) { rec_->setField(LANGUAGE_FIELD, language); }
			virtual bool isLanguageNull() const { return rec_->isFieldNull(LANGUAGE_FIELD); }
			virtual bool isLanguageWritable() const { return true; }

			virtual std::string getName() const { return rec_->getField(NAME_FIELD); }
			virtual void setName(const std::string& name) { rec_->setField(NAME_FIELD, name); }
			virtual bool isNameNull() const { return rec_->isFieldNull(NAME_FIELD); }
			virtual bool isNameWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string LANGUAGE_FIELD;
			static const std::string NAME_FIELD;


		private:

			SqliteRecord* rec_;
			DBIgnoreWordI* decorated_;

			// Do not allow the default copy constructor.
			SqliteIgnoreWord(const SqliteIgnoreWord& rec);
			// Do not allow the default assignment operator.
			SqliteIgnoreWord operator=(const SqliteIgnoreWord& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEIGNOREWORD_H
