#ifndef SQLITEWRAPPERERRORHISTORY_H
#define SQLITEWRAPPERERRORHISTORY_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBWRAPPERERRORHISTORYI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteWrapperErrorHistory : public DBWrapperErrorHistoryI
	{
		public:

			// Types and subtypes
			typedef DBWrapperErrorHistoryI interface_type;


			// Constructors and destructor
			SqliteWrapperErrorHistory(SqliteRecord* r, DBWrapperErrorHistoryI* d = 0);
			~SqliteWrapperErrorHistory();


			// Think about replacing this method with two friend functions from the
			// SqliteWrapperErrorHistoryRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBWrapperErrorHistoryI
			virtual DBWrapperErrorHistoryI* clone() const { return new SqliteWrapperErrorHistory(new SqliteRecord(*rec_)); }
			virtual const DBWrapperErrorHistoryI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }

			
			// ERRORID_FIELD

			virtual int getErrorCode() const { return rec_->getField(ERROR_FIELD); }
			virtual void setErrorCode(int errorCode){ rec_->setField(ERROR_FIELD, errorCode); };
			virtual bool isErrorCodeNull() const { return rec_->isFieldNull(ERROR_FIELD); }
			virtual void setErrorCodeNull() { rec_->setFieldNull(ERROR_FIELD); }
			virtual bool isErrorCodeWritable() const { return true; }			

			virtual std::string getErrorDateTime() const { return rec_->getField(DATETIME_FIELD); }
			virtual void setErrorDateTime(const std::string& errorDateTime) { rec_->setField(DATETIME_FIELD, errorDateTime); };
			virtual bool isErrorDateTimeNull() const { return rec_->isFieldNull(DATETIME_FIELD); }
			virtual void setErrorDateTimeNull(){ rec_->setFieldNull(DATETIME_FIELD); }
			virtual bool isErrorDateTimeWritable() const { return true; }

			virtual int getErrorPkgLength() const { return rec_->getField(PKGLENGTH_FIELD); }
			virtual void setErrorPkgLength(int pkgLength) { rec_->setField(PKGLENGTH_FIELD, pkgLength); }
			virtual bool isErrorPkgLengthNull() const { return rec_->isFieldNull(PKGLENGTH_FIELD); }
			virtual void setErrorPkgLengthNull() { rec_->setFieldNull(PKGLENGTH_FIELD); }
			virtual bool isErrorPkgLengthWritable() const { return true; }

			virtual int getErrorPkgWidth() const { return rec_->getField(PKGWIDTH_FIELD); }
			virtual void setErrorPkgWidth(int pkgWidth) { rec_->setField(PKGWIDTH_FIELD, pkgWidth); }
			virtual bool isErrorPkgWidthNull() const { return rec_->isFieldNull(PKGWIDTH_FIELD); }
			virtual void setErrorPkgWidthNull() { rec_->setFieldNull(PKGWIDTH_FIELD); }
			virtual bool isErrorPkgWidthWritable() const { return true; }

			virtual int getErrorPkgHeight() const { return rec_->getField(PKGHEIGHT_FIELD); }
			virtual void setErrorPkgHeight(int pkgLength) { rec_->setField(PKGHEIGHT_FIELD, pkgLength); }
			virtual bool isErrorPkgHeightNull() const { return rec_->isFieldNull(PKGHEIGHT_FIELD); }
			virtual void setErrorPkgHeightNull() { rec_->setFieldNull(PKGHEIGHT_FIELD); }
			virtual bool isErrorPkgHeightWritable() const { return true; }

			virtual int getErrorFilmUsed() const { return rec_->getField(FILMUSED_FIELD); }
			virtual void setErrorFilmUsed(int filmUsed) { rec_->setField(FILMUSED_FIELD, filmUsed); }
			virtual bool isErrorFilmUsedNull() const { return rec_->isFieldNull(FILMUSED_FIELD); }
			virtual void setErrorFilmUsedNull() { rec_->setFieldNull(FILMUSED_FIELD); }
			virtual bool isErrorFilmUsedWritable() const { return true; }

			virtual std::string getErrorTrayName() const { return rec_->getField(TRAYNAME_FIELD); }
			virtual void setErrorTrayName(const std::string& trayName) { rec_->setField(TRAYNAME_FIELD, trayName); };
			virtual bool isErrorTrayNameNull() const { return rec_->isFieldNull(TRAYNAME_FIELD); }
			virtual void setErrorTrayNameNull(){ rec_->setFieldNull(TRAYNAME_FIELD); }
			virtual bool isErrorTrayNameWritable() const { return true; }

			virtual bool getErrorBestFit() const { return util_string::stringtobool(rec_->getField(BESTFIT_FIELD)); }
			virtual void setErrorBestFit(bool bestFit){ rec_->setField(BESTFIT_FIELD, bestFit); }
			virtual bool isErrorBestFitNull() const { return rec_->isFieldNull(BESTFIT_FIELD); }
			virtual void setErrorBestFitNull(){ rec_->setFieldNull(BESTFIT_FIELD); }
			virtual bool isErrorBestFitWritable() const { return true; }

			virtual bool getErrorPerNet() const { return util_string::stringtobool(rec_->getField(PERNET_FIELD)); }
			virtual void setErrorPerNet(bool perNet) { rec_->setField(PERNET_FIELD, perNet); }
			virtual bool isErrorPerNetNull() const { return rec_->isFieldNull(PERNET_FIELD); }
			virtual void setErrorPerNetNull() { rec_->setFieldNull(PERNET_FIELD); }
			virtual bool isErrorPerNetWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string ERRORID_FIELD;
			static const std::string ERROR_FIELD;
			static const std::string DATETIME_FIELD;
			static const std::string PKGLENGTH_FIELD;
			static const std::string PKGWIDTH_FIELD;
			static const std::string PKGHEIGHT_FIELD;
			static const std::string FILMUSED_FIELD;
			static const std::string TRAYNAME_FIELD;
			static const std::string BESTFIT_FIELD;
			static const std::string PERNET_FIELD;

 
		private:

			SqliteRecord* rec_;
			DBWrapperErrorHistoryI* decorated_;

			// Do not allow the default copy constructor.
			SqliteWrapperErrorHistory(const SqliteWrapperErrorHistory& rec);
			// Do not allow the default assignment operator.
			SqliteWrapperErrorHistory operator=(const SqliteWrapperErrorHistory& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEWRAPPERERRORHISTORY_h
