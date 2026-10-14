#ifndef SQLITEPRODUCTTEXTCFG_H
#define SQLITEPRODUCTTEXTCFG_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBPRODUCTTEXTCFGI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteProductTextCfg : public DBProductTextCfgI
	{
		public:

			// Types and subtypes
			typedef DBProductTextCfgI interface_type;


			// Constructors and destructor
			SqliteProductTextCfg(SqliteRecord* r, DBProductTextCfgI* d = 0);
			~SqliteProductTextCfg();


			// Think about replacing this method with two friend functions from the
			// SqliteProductTextsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBProductTextI
			virtual DBProductTextCfgI* clone() const { return new SqliteProductTextCfg(new SqliteRecord(*rec_)); }
			virtual const DBProductTextCfgI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getTextType() const { return rec_->getField(TEXTTYPE_FIELD); }
			virtual void setTextType(const std::string& type) { rec_->setField(TEXTTYPE_FIELD, type); }
			virtual bool isTextTypeNull() const { return rec_->isFieldNull(TEXTTYPE_FIELD); }
			virtual bool isTextTypeWritable() const { return true; }

			virtual std::string getLabelNum() const { return rec_->getField(LABELNUM_FIELD); }
			virtual void setLabelNum(const std::string& num) { rec_->setField(LABELNUM_FIELD, num); }
			virtual bool isLabelNumNull() const { return rec_->isFieldNull(LABELNUM_FIELD); }
			virtual void setLabelNumNull() { rec_->setFieldNull(LABELNUM_FIELD); }
			virtual bool isLabelNumWritable() const { return true; }

			virtual std::string getPrinterNum() const { return rec_->getField(PRINTERNUM_FIELD); }
			virtual void setPrinterNum(const std::string& num) { rec_->setField(PRINTERNUM_FIELD, num); }
			virtual bool isPrinterNumNull() const { return rec_->isFieldNull(PRINTERNUM_FIELD); }
			virtual void setPrinterNumNull() { rec_->setFieldNull(PRINTERNUM_FIELD); }
			virtual bool isPrinterNumWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string TEXTTYPE_FIELD;
			static const std::string LABELNUM_FIELD;
			static const std::string PRINTERNUM_FIELD;


		private:

			SqliteRecord* rec_;
			DBProductTextCfgI* decorated_;

			// Do not allow the default copy constructor.
			SqliteProductTextCfg(const SqliteProductTextCfg& rec);
			// Do not allow the default assignment operator.
			SqliteProductTextCfg operator=(const SqliteProductTextCfg& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRODUCTTEXTCFG_H
