#ifndef SQLITECONFIGURATION_H
#define SQLITECONFIGURATION_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBCONFIGURATIONI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteConfiguration : public DBConfigurationI
	{
		public:

			// Types and subtypes
			typedef DBConfigurationI interface_type;


			// Constructors and destructor
			SqliteConfiguration(SqliteRecord* r, DBConfigurationI* d = 0);
			~SqliteConfiguration();


			// Think about replacing this method with two friend functions from the
			// SqliteConfigurationsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBConfigurationI
			virtual DBConfigurationI* clone() const { return new SqliteConfiguration(new SqliteRecord(*rec_)); }
			virtual const DBConfigurationI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getName() const { return rec_->getField(NAME_FIELD); }
			virtual void setName(const std::string& name) { rec_->setField(NAME_FIELD, name); }
			virtual bool isNameNull() const { return rec_->isFieldNull(NAME_FIELD); }
			virtual bool isNameWritable() const { return true; }

			virtual std::string getValue() const { return rec_->getField(VALUE_FIELD); }
			virtual void setValue(const std::string& value) { rec_->setField(VALUE_FIELD, value); }
			virtual bool isValueNull() const { return rec_->isFieldNull(VALUE_FIELD); }
			virtual void setValueNull() { rec_->setFieldNull(VALUE_FIELD); }
			virtual bool isValueWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NAME_FIELD;
			static const std::string VALUE_FIELD;


		private:

			SqliteRecord* rec_;
			DBConfigurationI* decorated_;

			// Do not allow the default copy constructor.
			SqliteConfiguration(const SqliteConfiguration& rec);
			// Do not allow the default assignment operator.
			SqliteConfiguration operator=(const SqliteConfiguration& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECONFIGURATION_H
