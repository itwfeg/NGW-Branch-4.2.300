#ifndef DBWRAPPERERRORHISTORY_H
#define DBWRAPPERERRORHISTORY_H


#include <string>


#include "edb.h"
#include DBWRAPPERERRORHISTORYI_INC
#if defined( __linux__ )
    #include DBWRAPPERERRORHISTORYRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBWrapperErrorHistory
	{
			// DBScalesRM class is a friend of DBScale because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBWrapperErrorHistoryRM;
			friend class DBQuery<DBWrapperErrorHistory>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBWrapperErrorHistoryI interface_type;
			typedef DBWrapperErrorHistoryRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBWrapperErrorHistory(const DBWrapperErrorHistory& rec);  
			~DBWrapperErrorHistory();


			// Methods associated with DBWrapperErrorHistoryI
			int getErrorCode() const { return decorated_->getErrorCode(); }
			void setErrorCode(int errorCode) {decorated_->setErrorCode(errorCode);}
			bool isErrorCodeNull() const { return decorated_->isErrorCodeNull(); }
			void setErrorCodeNull(){ decorated_->setErrorCodeNull(); }
			bool isErrorCodeWritable() const { return decorated_->isErrorCodeWritable(); }

			std::string getErrorDateTime() const { return decorated_->getErrorDateTime(); }
			void setErrorDateTime(const std::string& errorDateTime) { decorated_->setErrorDateTime(errorDateTime); }
			bool isErrorDateTimeNull() const { return decorated_->isErrorDateTimeNull(); }
			void setErrorDateTimeNull() { decorated_->setErrorDateTimeNull(); }
			bool isErrorDateTimeWritable() const { return decorated_->isErrorDateTimeWritable(); }

			int getErrorPkgLength() const { return decorated_->getErrorPkgLength(); }
			void setErrorPkgLength(int pkgLength) { decorated_->setErrorPkgLength(pkgLength); }
			bool isErrorPkgLengthNull() const { return decorated_->isErrorPkgLengthNull(); }
			void setErrorPkgLengthNull() { decorated_->setErrorPkgLengthNull(); }
			bool isErrorPkgLengthWritable() const { return decorated_->isErrorPkgLengthWritable(); }

			int getErrorPkgWidth() const { return decorated_->getErrorPkgWidth(); }
			void setErrorPkgWidth(int pkgWidth) { decorated_->setErrorPkgWidth(pkgWidth); }
			bool isErrorPkgWidthNull() const { return decorated_->isErrorPkgWidthNull(); }
			void setErrorPkgWidthNull() { decorated_->setErrorPkgWidthNull(); }
			bool isErrorPkgWidthWritable() const { return decorated_->isErrorPkgWidthWritable(); }

			int getErrorPkgHeight() const { return decorated_->getErrorPkgHeight(); }
			void setErrorPkgHeight(int pkgHeight) { decorated_->setErrorPkgHeight(pkgHeight); }
			bool isErrorPkgHeightNull() const { return decorated_->isErrorPkgHeightNull(); }
			void setErrorPkgHeightNull() { decorated_->setErrorPkgHeightNull(); }
			bool isErrorPkgHeightWritable() const { return decorated_->isErrorPkgHeightWritable(); }

			int getErrorFilmUsed() const { return decorated_->getErrorFilmUsed(); }
			void setErrorFilmUsed(int filmUsed) { decorated_->setErrorFilmUsed(filmUsed); }
			bool isErrorFilmUsedNull() const { return decorated_->isErrorFilmUsedNull(); }
			void setErrorFilmUsedNull() { decorated_->setErrorFilmUsedNull(); }
			bool isErrorFilmUsedWritable() const { return decorated_->isErrorFilmUsedWritable(); }

			std::string getErrorTrayName() const { return decorated_->getErrorTrayName(); }
			void setErrorTrayName(const std::string& trayName) { decorated_->setErrorTrayName(trayName); }
			bool isErrorTrayNameNull() const { return decorated_->isErrorTrayNameNull(); }
			void setErrorTrayNameNull() { decorated_->setErrorTrayNameNull(); }
			bool isErrorTrayNameWritable() const { return decorated_->isErrorTrayNameWritable(); }

			bool getErrorBestFit() const { return decorated_->getErrorBestFit(); }
			void setErrorBestFit(bool bestFit) { decorated_->setErrorBestFit(bestFit); }
			bool isErrorBestFitNull() const { return decorated_->isErrorBestFitNull(); }
			void setErrorBestFitNull() { decorated_->setErrorBestFitNull(); }
			bool isErrorBestFitWritable() const { return decorated_->isErrorBestFitWritable(); }

			bool getErrorPerNet() const { return decorated_->getErrorPerNet(); }
			void setErrorPerNet(bool perNet) { decorated_->setErrorPerNet(perNet); }
			bool isErrorPerNetNull() const { return decorated_->isErrorPerNetNull(); }
			void setErrorPerNetNull() { decorated_->setErrorPerNetNull(); }
			bool isErrorPerNetWritable() const { return decorated_->isErrorPerNetWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }
						

			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBWrapperErrorHistory& rec);


		private:

			edb_adapter::DBWrapperErrorHistoryI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBWrapperErrorHistory(edb_adapter::DBWrapperErrorHistoryI* d);

			// Do not allow the default assignment operator.
			DBWrapperErrorHistory operator=(const DBWrapperErrorHistory& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif
