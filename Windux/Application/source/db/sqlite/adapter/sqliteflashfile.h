#ifndef SQLITEFLASHFILE_H
#define SQLITEFLASHFILE_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBFLASHFILEI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteFlashFile : public DBFlashFileI
	{
		public:

			// Types and subtypes
			typedef DBFlashFileI interface_type;


			// Constructors and destructor
			SqliteFlashFile(SqliteRecord* r, DBFlashFileI* d = 0);
			~SqliteFlashFile();


			// Think about replacing this method with two friend functions from the
			// SqliteFlashFilesRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBFlashFileI
			virtual DBFlashFileI* clone() const { return new SqliteFlashFile(new SqliteRecord(*rec_)); }
			virtual const DBFlashFileI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getName() const { return rec_->getField(NAME_FIELD); }
			virtual void setName(const std::string& name) { rec_->setField(NAME_FIELD, name); }
			virtual bool isNameNull() const { return rec_->isFieldNull(NAME_FIELD); }
			virtual bool isNameWritable() const { return true; }

			virtual std::string getDate() const { return rec_->getField(DATE_FIELD); }
			virtual void setDate(const std::string& date) { rec_->setField(DATE_FIELD, date); }
			virtual bool isDateNull() const { return rec_->isFieldNull(DATE_FIELD); }
			virtual void setDateNull() { rec_->setFieldNull(DATE_FIELD); }
			virtual bool isDateWritable() const { return true; }

			// COMPLETE_FIELD


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NAME_FIELD;
			static const std::string DATE_FIELD;
			static const std::string COMPLETE_FIELD;


		private:

			SqliteRecord* rec_;
			DBFlashFileI* decorated_;

			// Do not allow the default copy constructor.
			SqliteFlashFile(const SqliteFlashFile& rec);
			// Do not allow the default assignment operator.
			SqliteFlashFile operator=(const SqliteFlashFile& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEFLASHFILE_H
