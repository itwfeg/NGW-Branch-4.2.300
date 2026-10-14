#ifndef DBPRODUCTTEXTCFGI_H
#define DBPRODUCTTEXTCFGI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBProductTextCfgI
	{
		public:

			virtual ~DBProductTextCfgI() {}


			virtual DBProductTextCfgI* clone() const = 0;
			virtual const DBProductTextCfgI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getTextType() const = 0;
			virtual void setTextType(const std::string& type) = 0;
			virtual bool isTextTypeNull() const { return false; }
			virtual bool isTextTypeWritable() const { return true; }

			virtual std::string getLabelNum() const = 0;
			virtual void setLabelNum(const std::string& type) = 0;
			virtual bool isLabelNumNull() const { return false; }
			virtual void setLabelNumNull() = 0;
			virtual bool isLabelNumWritable() const { return true; }

			virtual std::string getPrinterNum() const = 0;
			virtual void setPrinterNum(const std::string& text) = 0;
			virtual bool isPrinterNumNull() const { return false; }
			virtual void setPrinterNumNull() = 0;
			virtual bool isPrinterNumWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRODUCTTEXTCFGI_H
