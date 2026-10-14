#ifndef SQLITEEXPIRATIONLABEL_H
#define SQLITEEXPIRATIONLABEL_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBEXPIRATIONLABELI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteExpirationLabel : public DBExpirationLabelI
	{
		public:

			// Types and subtypes
			typedef DBExpirationLabelI interface_type;

			
			// Constructors and destructor
			SqliteExpirationLabel(SqliteRecord* r, DBExpirationLabelI* d = 0);
			~SqliteExpirationLabel();


			// Think about replacing this method with two friend functions from the
			// SqliteExpirationLabelsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBExpirationLabelI
			virtual DBExpirationLabelI* clone() const { return new SqliteExpirationLabel(new SqliteRecord(*rec_)); }
			virtual const DBExpirationLabelI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getSequenceNumber() const { return rec_->getField(SEQUENCENUMBER_FIELD); }
			virtual bool isSequenceNumberNull() const { return rec_->isFieldNull(SEQUENCENUMBER_FIELD); }
			virtual bool isSequenceNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const { return rec_->getField(DEPTNUM_FIELD); }
			virtual void setDeptNumber(const std::string& number) { rec_->setField(DEPTNUM_FIELD, number); }
			virtual bool isDeptNumberNull() const { return rec_->isFieldNull(DEPTNUM_FIELD); }
			virtual bool isDeptNumberWritable() const { return true; }


			virtual std::string getPLU() const { return rec_->getField(PLU_FIELD); }
			virtual void setPLU(const std::string& plu) { rec_->setField(PLU_FIELD, plu); }
			virtual bool isPLUNull() const { return rec_->isFieldNull(PLU_FIELD); }
			virtual void setPLUNull() { rec_->setFieldNull(PLU_FIELD); }
			virtual bool isPLUWritable() const { return true; }

			virtual std::string getProductDescription() const { return rec_->getField(PRODUCTDESCRIPTION_FIELD); }
			virtual void setProductDescription(const std::string& desc) { rec_->setField(PRODUCTDESCRIPTION_FIELD, desc); }
			virtual bool isProductDescriptionNull() const { return rec_->isFieldNull(PRODUCTDESCRIPTION_FIELD); }
			virtual void setProductDescriptionNull() { rec_->setFieldNull(PRODUCTDESCRIPTION_FIELD); }
			virtual bool isProductDescriptionWritable() const { return true; }

			virtual std::string getExpiration() const { return rec_->getField(EXPIRATION_FIELD); }
			virtual void setExpiration(const std::string& exp) { rec_->setField(EXPIRATION_FIELD, exp); }
			virtual bool isExpirationNull() const { return rec_->isFieldNull(EXPIRATION_FIELD); }
			virtual void setExpirationNull() { rec_->setFieldNull(EXPIRATION_FIELD); }
			virtual bool isExpirationWritable() const { return true; }

			virtual std::string getExpirationLabelKey() const { return rec_->getField(SEQUENCENUMBER_FIELD); }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string SEQUENCENUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string PLU_FIELD;
			static const std::string PRODUCTDESCRIPTION_FIELD;
			static const std::string EXPIRATION_FIELD;


		private:

			SqliteRecord* rec_;
			DBExpirationLabelI* decorated_;

			// Do not allow the default copy constructor.
			SqliteExpirationLabel(const SqliteExpirationLabel& rec);
			// Do not allow the default assignment operator.
			SqliteExpirationLabel operator=(const SqliteExpirationLabel& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEEXPIRATIONLABEL_H
