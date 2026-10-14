#ifndef DBSOFTKEYFEATUREI_H
#define DBSOFTKEYFEATUREI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBSoftKeyFeatureI
	{
		public:

			virtual ~DBSoftKeyFeatureI() {}


			virtual DBSoftKeyFeatureI* clone() const = 0;
			virtual const DBSoftKeyFeatureI* getDecorated() const = 0;


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

			virtual int getDefaultHorizontalGridSize() const = 0;
			virtual void setDefaultHorizontalGridSize(int dhgs) = 0;
			virtual bool isDefaultHorizontalGridSizeNull() const { return false; }
			virtual void setDefaultHorizontalGridSizeNull() = 0;
			virtual bool isDefaultHorizontalGridSizeWritable() const { return true; }

			virtual int getDefaultVerticalGridSize() const = 0;
			virtual void setDefaultVerticalGridSize(int dvgs) = 0;
			virtual bool isDefaultVerticalGridSizeNull() const { return false; }
			virtual void setDefaultVerticalGridSizeNull() = 0;
			virtual bool isDefaultVerticalGridSizeWritable() const { return true; }

			virtual int getDefaultHorizontalSpacing() const = 0;
			virtual void setDefaultHorizontalSpacing(int hs) = 0;
			virtual bool isDefaultHorizontalSpacingNull() const { return false; }
			virtual void setDefaultHorizontalSpacingNull() = 0;
			virtual bool isDefaultHorizontalSpacingWritable() const { return true; }

			virtual int getDefaultVerticalSpacing() const = 0;
			virtual void setDefaultVerticalSpacing(int vs) = 0;
			virtual bool isDefaultVerticalSpacingNull() const { return false; }
			virtual void setDefaultVerticalSpacingNull() = 0;
			virtual bool isDefaultVerticalSpacingWritable() const { return true; }

			virtual int getDefaultGroupNumber() const = 0;
			virtual void setDefaultGroupNumber(int dgn) = 0;
			virtual bool isDefaultGroupNumberNull() const { return false; }
			virtual void setDefaultGroupNumberNull() = 0;
			virtual bool isDefaultGroupNumberWritable() const { return true; }

			virtual int getSoftKeyStyleNumber() const = 0;
			virtual void setSoftKeyStyleNumber(int dgn) = 0;
			virtual bool isSoftKeyStyleNumberNull() const { return false; }
			virtual void setSoftKeyStyleNumberNull() = 0;
			virtual bool isSoftKeyStyleNumberWritable() const { return true; }

			virtual int getFixedRowColMethod() const = 0;
			virtual void setFixedRowColMethod(int frcm) = 0;
			virtual bool isFixedRowColMethodNull() const { return false; }
			virtual void setFixedRowColMethodNull() = 0;
			virtual bool isFixedRowColMethodWritable() const { return true; }

			virtual int getDefaultButton() const = 0;
			virtual void setDefaultButton(int frcm) = 0;
			virtual bool isDefaultButtonNull() const { return false; }
			virtual void setDefaultButtonNull() = 0;
			virtual bool isDefaultButtonWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBSOFTKEYFEATUREI_H
