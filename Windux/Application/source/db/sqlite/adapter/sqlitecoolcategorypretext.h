#ifndef SQLITECOOLCATEGORYPRETEXT_H
#define SQLITECOOLCATEGORYPRETEXT_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBCOOLCATEGORYPRETEXTI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteCoolCategoryPreText : public DBCoolCategoryPreTextI
	{
		public:

			// Types and subtypes
			typedef DBCoolCategoryPreTextI interface_type;


			// Constructors and destructor
			SqliteCoolCategoryPreText(SqliteRecord* r, DBCoolCategoryPreTextI* d = 0);
			~SqliteCoolCategoryPreText();


			// Think about replacing this method with two friend functions from the
			// SqliteCoolCategoryPreTextsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBCoolCategoryPreTextI
			virtual DBCoolCategoryPreTextI* clone() const { return new SqliteCoolCategoryPreText(new SqliteRecord(*rec_)); }
			virtual const DBCoolCategoryPreTextI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getPreTextNumber() const { return rec_->getField(PRETEXTNUMBER_FIELD); }
			virtual void setPreTextNumber(const std::string& step) { rec_->setField(PRETEXTNUMBER_FIELD, step); }
			virtual bool isPreTextNumberNull() const { return rec_->isFieldNull(PRETEXTNUMBER_FIELD); }
			virtual bool isPreTextNumberWritable() const { return true; }

			virtual std::string getCategoryNumber() const { return rec_->getField(CATEGORYNUMBER_FIELD); }
			virtual void setCategoryNumber(const std::string& category) { rec_->setField(CATEGORYNUMBER_FIELD, category); }
			virtual bool isCategoryNumberNull() const { return rec_->isFieldNull(CATEGORYNUMBER_FIELD); }
			virtual bool isCategoryNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const { return rec_->getField(DEPTNUM_FIELD); }
			virtual void setDeptNumber(const std::string& number) { rec_->setField(DEPTNUM_FIELD, number); }
			virtual bool isDeptNumberNull() const { return rec_->isFieldNull(DEPTNUM_FIELD); }
			virtual bool isDeptNumberWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string PRETEXTNUMBER_FIELD;
			static const std::string CATEGORYNUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;

		private:

			SqliteRecord* rec_;
			DBCoolCategoryPreTextI* decorated_;

			// Do not allow the default copy constructor.
			SqliteCoolCategoryPreText(const SqliteCoolCategoryPreText& rec);
			// Do not allow the default assignment operator.
			SqliteCoolCategoryPreText operator=(const SqliteCoolCategoryPreText& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLCATEGORYPRETEXT_H
