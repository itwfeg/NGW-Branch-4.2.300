#ifndef SQLITEPRODUCTSEQUENCE_H
#define SQLITEPRODUCTSEQUENCE_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBPRODUCTSEQUENCEI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteProductSequence : public DBProductSequenceI
	{
		public:

			// Types and subtypes
			typedef DBProductSequenceI interface_type;

			
			// Constructors and destructor
			SqliteProductSequence(SqliteRecord* r, DBProductSequenceI* d = 0);
			~SqliteProductSequence();


			// Think about replacing this method with two friend functions from the
			// SqliteProductSequencesRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBProductSequenceI
			virtual DBProductSequenceI* clone() const { return new SqliteProductSequence(new SqliteRecord(*rec_)); }
			virtual const DBProductSequenceI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getProductKey() const { return rec_->getField(PRODUCTKEY_FIELD); }
			virtual void setProductKey(const std::string& id) { rec_->setField(PRODUCTKEY_FIELD, id); }
			virtual bool isProductKeyNull() const { return rec_->isFieldNull(PRODUCTKEY_FIELD); }
			virtual bool isProductKeyWritable() const { return true; }

			virtual std::string getCharacterSequenceLanguage() const { return rec_->getField(CHARACTERSEQUENCELANGUAGE_FIELD); }
			virtual void setCharacterSequenceLanguage(const std::string& language) { rec_->setField(CHARACTERSEQUENCELANGUAGE_FIELD, language); }
			virtual bool isCharacterSequenceLanguageNull() const { return rec_->isFieldNull(CHARACTERSEQUENCELANGUAGE_FIELD); }
			virtual bool isCharacterSequenceLanguageWritable() const { return true; }

			virtual std::string getCharacterSequence() const { return rec_->getField(CHARACTERSEQUENCE_FIELD); }
			virtual void setCharacterSequence(const std::string& sequence) { rec_->setField(CHARACTERSEQUENCE_FIELD, sequence); }
			virtual bool isCharacterSequenceNull() const { return rec_->isFieldNull(CHARACTERSEQUENCE_FIELD); }
			virtual bool isCharacterSequenceWritable() const { return true; }

			virtual std::string getProductName() const { return rec_->getField(PRODUCTNAME_FIELD); } 
			virtual void setProductName(const std::string& name) { rec_->setField(PRODUCTNAME_FIELD, name); }
			virtual bool isProductNameNull() const { return rec_->isFieldNull(PRODUCTNAME_FIELD); }
			virtual void setProductNameNull() { rec_->setFieldNull(PRODUCTNAME_FIELD); }
			virtual bool isProductNameWritable() const { return true; }

			virtual std::string getProductPrice() const { return rec_->getField(PRODUCTPRICE_FIELD); }
			virtual void setProductPrice(const std::string& price) { rec_->setField(PRODUCTPRICE_FIELD, price); }
			virtual bool isProductPriceNull() const { return rec_->isFieldNull(PRODUCTPRICE_FIELD); }
			virtual void setProductPriceNull() { rec_->setFieldNull(PRODUCTPRICE_FIELD); }
			virtual bool isProductPriceWritable() const { return true; }

			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string PRODUCTKEY_FIELD;
			static const std::string CHARACTERSEQUENCELANGUAGE_FIELD;
			static const std::string CHARACTERSEQUENCE_FIELD;
			static const std::string PRODUCTNAME_FIELD;
			static const std::string PRODUCTPRICE_FIELD;


		private:

			SqliteRecord* rec_;
			DBProductSequenceI* decorated_;

			// Do not allow the default copy constructor.
			SqliteProductSequence(const SqliteProductSequence& rec);
			// Do not allow the default assignment ProductSequence.
			SqliteProductSequence operator=(const SqliteProductSequence& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRODUCTSEQUENCE_H
