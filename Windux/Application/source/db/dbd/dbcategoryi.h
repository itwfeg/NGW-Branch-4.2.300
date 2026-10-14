#ifndef DBCATEGORYI_H
#define DBCATEGORYI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCategoryI
	{
		public:

			virtual ~DBCategoryI() {}


			virtual DBCategoryI* clone() const = 0;
			virtual const DBCategoryI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getNumber() const = 0;
			virtual void setNumber(const std::string& number) = 0;
			virtual bool isNumberNull() const { return false; }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const = 0;
			virtual void setDeptNumber(const std::string& number) = 0;
			virtual bool isDeptNumberNull() const { return false; }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual std::string getParentCategory() const = 0;
			virtual void setParentCategory(const std::string& pc) = 0;
			virtual bool isParentCategoryNull() const { return false; }
			virtual void setParentCategoryNull() = 0;
			virtual bool isParentCategoryWritable() const { return true; }

			virtual std::string getName1() const = 0;
			virtual void setName1(const std::string& name) = 0;
			virtual bool isName1Null() const { return false; }
			virtual void setName1Null() = 0;
			virtual bool isName1Writable() const { return true; }

			virtual std::string getName2() const = 0;
			virtual void setName2(const std::string& name) = 0;
			virtual bool isName2Null() const { return false; }
			virtual void setName2Null() = 0;
			virtual bool isName2Writable() const { return true; }

			virtual int getBackgroundColor() const = 0;
			virtual void setBackgroundColor(int vs) = 0;
			virtual bool isBackgroundColorNull() const { return false; }
			virtual void setBackgroundColorNull() = 0;
			virtual bool isBackgroundColorWritable() const { return true; }

			virtual int getTextColor() const = 0;
			virtual void setTextColor(int tc) = 0;
			virtual bool isTextColorNull() const { return false; }
			virtual void setTextColorNull() = 0;
			virtual bool isTextColorWritable() const { return true; }

			virtual std::string getImage() const = 0;
			virtual void setImage(const std::string& graphic) = 0;
			virtual bool isImageNull() const { return false; }
			virtual void setImageNull() = 0;
			virtual bool isImageWritable() const { return true; }

			virtual std::string getPlaylist() const = 0;
			virtual void setPlaylist(const std::string& list) = 0;
			virtual bool isPlaylistNull() const { return false; }
			virtual void setPlaylistNull() = 0;
			virtual bool isPlaylistWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBCATEGORYI_H
