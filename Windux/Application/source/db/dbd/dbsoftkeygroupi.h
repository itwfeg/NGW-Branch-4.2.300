#ifndef DBSOFTKEYGROUPI_H
#define DBSOFTKEYGROUPI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBSoftKeyGroupI
	{
		public:

			virtual ~DBSoftKeyGroupI() {}


			virtual DBSoftKeyGroupI* clone() const = 0;
			virtual const DBSoftKeyGroupI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getNumber() const = 0;
			virtual void setNumber(const std::string& n) = 0;
			virtual bool isNumberNull() const { return false; }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const = 0;
			virtual void setDeptNumber(const std::string& number) = 0;
			virtual bool isDeptNumberNull() const { return false; }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual int getHorizontalGridSize() const = 0;
			virtual void setHorizontalGridSize(int hs) = 0;
			virtual bool isHorizontalGridSizeNull() const { return false; }
			virtual void setHorizontalGridSizeNull() = 0;
			virtual bool isHorizontalGridSizeWritable() const { return true; }

			virtual int getVerticalGridSize() const = 0;
			virtual void setVerticalGridSize(int vs) = 0;
			virtual bool isVerticalGridSizeNull() const { return false; }
			virtual void setVerticalGridSizeNull() = 0;
			virtual bool isVerticalGridSizeWritable() const { return true; }

			virtual int getHorizontalSpacing() const = 0;
			virtual void setHorizontalSpacing(int hs) = 0;
			virtual bool isHorizontalSpacingNull() const { return false; }
			virtual void setHorizontalSpacingNull() = 0;
			virtual bool isHorizontalSpacingWritable() const { return true; }

			virtual int getVerticalSpacing() const = 0;
			virtual void setVerticalSpacing(int vs) = 0;
			virtual bool isVerticalSpacingNull() const { return false; }
			virtual void setVerticalSpacingNull() = 0;
			virtual bool isVerticalSpacingWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBSOFTKEYGROUPI_H
