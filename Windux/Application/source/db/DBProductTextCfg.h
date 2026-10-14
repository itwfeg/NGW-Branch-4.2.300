#ifndef DBPRODUCTTEXTCFG_H
#define DBPRODUCTTEXTCFG_H


#include <string>


#include "edb.h"
#include DBPRODUCTTEXTCFGI_INC
#if defined( __linux__ )
    #include DBPRODUCTTEXTCFGSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBProductTextCfg
	{
			// DBProductTextCfgsRM class is a friend of DBProductTextCfg because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBProductTextCfgsRM;
			friend class DBQuery<DBProductTextCfg>;
			//friend class DBProductsRM;     // Needs access to decorated_ for cross-referencing.

		public:

			// Types and subtypes
			typedef edb_adapter::DBProductTextCfgI interface_type;
			typedef DBProductTextCfgsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBProductTextCfg(const DBProductTextCfg& rec);  
			~DBProductTextCfg();


			// Methods associated with DBProductTextCfgI
			std::string getTextType() const { return decorated_->getTextType(); }
			void setTextType(const std::string& type) { decorated_->setTextType(type); }
			bool isTextTypeNull() const { return decorated_->isTextTypeNull(); }
			bool isTextTypeWritable() const { return decorated_->isTextTypeWritable(); }

			std::string getLabelNum() const { return decorated_->getLabelNum(); }
			void setLabelNum(const std::string& num) { decorated_->setLabelNum(num); }
			bool isLabelNumNull() const { return decorated_->isLabelNumNull(); }
			void setLabelNumNull() { decorated_->setLabelNumNull(); }
			bool isLabelNumWritable() const { return decorated_->isLabelNumWritable(); }

			std::string getPrinterNum() const { return decorated_->getPrinterNum(); }
			void setPrinterNum(const std::string& num) { decorated_->setPrinterNum(num); }
			bool isPrinterNumNull() const { return decorated_->isPrinterNumNull(); }
			void setPrinterNumNull() { decorated_->setPrinterNumNull(); }
			bool isPrinterNumWritable() const { return decorated_->isPrinterNumWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getTextType(); }
			void setKey(const std::string& key) { decorated_->setTextType(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBProductTextCfg& rec);


		private:

			edb_adapter::DBProductTextCfgI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBProductTextCfg(edb_adapter::DBProductTextCfgI* d);

			// Do not allow the default assignment operator.
			DBProductTextCfg operator=(const DBProductTextCfg& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRODUCTTEXTCFG_H
