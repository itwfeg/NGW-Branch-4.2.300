#ifndef DBSOFTKEYSTYLEI_H
#define DBSOFTKEYSTYLEI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBSoftKeyStyleI
	{
		public:

			virtual ~DBSoftKeyStyleI() {}


			virtual DBSoftKeyStyleI* clone() const = 0;
			virtual const DBSoftKeyStyleI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getNumber() const = 0;
			virtual void setNumber(const std::string& n) = 0;
			virtual bool isNumberNull() const { return false; }
			virtual bool isNumberWritable() const { return true; }

			virtual int getDefaultTextLocation() const = 0;
			virtual void setDefaultTextLocation(int hs) = 0;
			virtual bool isDefaultTextLocationNull() const { return false; }
			virtual void setDefaultTextLocationNull() = 0;
			virtual bool isDefaultTextLocationWritable() const { return true; }

			virtual int getDefaultTextColor() const = 0;
			virtual void setDefaultTextColor(int hs) = 0;
			virtual bool isDefaultTextColorNull() const { return false; }
			virtual void setDefaultTextColorNull() = 0;
			virtual bool isDefaultTextColorWritable() const { return true; }

			virtual int getDefaultGrayedOutTextColor() const = 0;
			virtual void setDefaultGrayedOutTextColor(int hs) = 0;
			virtual bool isDefaultGrayedOutTextColorNull() const { return false; }
			virtual void setDefaultGrayedOutTextColorNull() = 0;
			virtual bool isDefaultGrayedOutTextColorWritable() const { return true; }

			virtual int getDefaultForegroundImageLocation() const = 0;
			virtual void setDefaultForegroundImageLocation(int hs) = 0;
			virtual bool isDefaultForegroundImageLocationNull() const { return false; }
			virtual void setDefaultForegroundImageLocationNull() = 0;
			virtual bool isDefaultForegroundImageLocationWritable() const { return true; }

			virtual int getDefaultBackgroundColor() const = 0;
			virtual void setDefaultBackgroundColor(int hs) = 0;
			virtual bool isDefaultBackgroundColorNull() const { return false; }
			virtual void setDefaultBackgroundColorNull() = 0;
			virtual bool isDefaultBackgroundColorWritable() const { return true; }

			virtual std::string getDefaultBackgroundImage() const = 0;
			virtual void setDefaultBackgroundImage(const std::string& dbi) = 0;
			virtual bool isDefaultBackgroundImageNull() const { return false; }
			virtual void setDefaultBackgroundImageNull() = 0;
			virtual bool isDefaultBackgroundImageWritable() const { return true; }

			virtual std::string getImagesDirectory() const = 0;
			virtual void setImagesDirectory(const std::string& id) = 0;
			virtual bool isImagesDirectoryNull() const { return false; }
			virtual void setImagesDirectoryNull() = 0;
			virtual bool isImagesDirectoryWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBSOFTKEYSTYLEI_H
