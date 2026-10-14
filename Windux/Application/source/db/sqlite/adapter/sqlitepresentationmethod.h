#ifndef SQLITEPRESENTATIONMETHOD_H
#define SQLITEPRESENTATIONMETHOD_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBPRESENTATIONMETHODI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif


	class SqlitePresentationMethod : public DBPresentationMethodI
	{
		public:

			// Types and subtypes
			typedef DBPresentationMethodI interface_type;

			
			// Constructors and destructor
			SqlitePresentationMethod(SqliteRecord* r, DBPresentationMethodI* d = 0);
			~SqlitePresentationMethod();


			// Think about replacing this method with two friend functions from the
			// SqlitePresentationMethodsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBPresentationMethodI
			virtual DBPresentationMethodI* clone() const { return new SqlitePresentationMethod(new SqliteRecord(*rec_)); }
			virtual const DBPresentationMethodI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getType() const { return rec_->getField(TYPE_FIELD); }
			virtual void setType(const std::string& type) { rec_->setField(TYPE_FIELD, type); }
			virtual bool isTypeNull() const { return rec_->isFieldNull(TYPE_FIELD); }
			virtual bool isTypeWritable() const { return true; }

			virtual std::string getDescription() const { return rec_->getField(DESCRIPTION1_FIELD); }
			virtual void setDescription(const std::string& desc) { rec_->setField(DESCRIPTION1_FIELD, desc); }
			virtual bool isDescriptionNull() const { return rec_->isFieldNull(DESCRIPTION1_FIELD); }
			virtual void setDescriptionNull() { rec_->setFieldNull(DESCRIPTION1_FIELD); }
			virtual bool isDescriptionWritable() const { return true; }

			// DESCRIPTION2_FIELD


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string TYPE_FIELD;
			static const std::string DESCRIPTION1_FIELD;
			static const std::string DESCRIPTION2_FIELD;


		private:

			SqliteRecord* rec_;
			DBPresentationMethodI* decorated_;

			// Do not allow the default copy constructor.
			SqlitePresentationMethod(const SqlitePresentationMethod& rec);
			// Do not allow the default assignment operator.
			SqlitePresentationMethod operator=(const SqlitePresentationMethod& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRESENTATIONMETHOD_H
