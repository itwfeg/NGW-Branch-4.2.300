#ifndef DBSOFTKEYI_H
#define DBSOFTKEYI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBProductI;


	class DBSoftKeyI
	{
		public:

			virtual ~DBSoftKeyI() {}


			virtual DBSoftKeyI* clone() const = 0;
			virtual const DBSoftKeyI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getGroupNumber() const = 0;
			virtual void setGroupNumber(const std::string& gn) = 0;
			virtual bool isGroupNumberNull() const { return false; }
			virtual bool isGroupNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const = 0;
			virtual void setDeptNumber(const std::string& number) = 0;
			virtual bool isDeptNumberNull() const { return false; }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual std::string getHorizontalPosition() const = 0;
			virtual void setHorizontalPosition(const std::string& hp) = 0;
			virtual bool isHorizontalPositionNull() const { return false; }
			virtual bool isHorizontalPositionWritable() const { return true; }

			virtual std::string getVerticalPosition() const = 0;
			virtual void setVerticalPosition(const std::string& vp) = 0;
			virtual bool isVerticalPositionNull() const { return false; }
			virtual bool isVerticalPositionWritable() const { return true; }

			virtual int getHorizontalSize() const = 0;
			virtual void setHorizontalSize(int hs) = 0;
			virtual bool isHorizontalSizeNull() const { return false; }
			virtual void setHorizontalSizeNull() = 0;
			virtual bool isHorizontalSizeWritable() const { return true; }

			virtual int getVerticalSize() const = 0;
			virtual void setVerticalSize(int vs) = 0;
			virtual bool isVerticalSizeNull() const { return false; }
			virtual void setVerticalSizeNull() = 0;
			virtual bool isVerticalSizeWritable() const { return true; }

			virtual int getTextColor() const = 0;
			virtual void setTextColor(int tc) = 0;
			virtual bool isTextColorNull() const { return false; }
			virtual void setTextColorNull() = 0;
			virtual bool isTextColorWritable() const { return true; }

			virtual int getBackgroundColor() const = 0;
			virtual void setBackgroundColor(int vs) = 0;
			virtual bool isBackgroundColorNull() const { return false; }
			virtual void setBackgroundColorNull() = 0;
			virtual bool isBackgroundColorWritable() const { return true; }

			virtual int getToggledBackgroundColor() const = 0;
			virtual void setToggledBackgroundColor(int vs) = 0;
			virtual bool isToggledBackgroundColorNull() const { return false; }
			virtual void setToggledBackgroundColorNull() = 0;
			virtual bool isToggledBackgroundColorWritable() const { return true; }

			virtual std::string getBackgroundImage() const = 0;
			virtual void setBackgroundImage(const std::string& image) = 0;
			virtual bool isBackgroundImageNull() const { return false; }
			virtual void setBackgroundImageNull() = 0;
			virtual bool isBackgroundImageWritable() const { return true; }

			virtual std::string getToggledBackgroundImage() const = 0;
			virtual void setToggledBackgroundImage(const std::string& image) = 0;
			virtual bool isToggledBackgroundImageNull() const { return false; }
			virtual void setToggledBackgroundImageNull() = 0;
			virtual bool isToggledBackgroundImageWritable() const { return true; }

			virtual std::string getName1() const = 0;
			virtual void setName1(const std::string& name1) = 0;
			virtual bool isName1Null() const { return false; }
			virtual void setName1Null() = 0;
			virtual bool isName1Writable() const { return true; }

			virtual std::string getName2() const = 0;
			virtual void setName2(const std::string& name2) = 0;
			virtual bool isName2Null() const { return false; }
			virtual void setName2Null() = 0;
			virtual bool isName2Writable() const { return true; }

			virtual bool getForceNoBackgroundColor() const = 0;
			virtual void setForceNoBackgroundColor(bool force) = 0;
			virtual bool isForceNoBackgroundColorNull() const { return false; }
			virtual void setForceNoBackgroundColorNull() = 0;
			virtual bool isForceNoBackgroundColorWritable() const { return true; }

			virtual bool getForceNoBackgroundImage() const = 0;
			virtual void setForceNoBackgroundImage(bool force) = 0;
			virtual bool isForceNoBackgroundImageNull() const { return false; }
			virtual void setForceNoBackgroundImageNull() = 0;
			virtual bool isForceNoBackgroundImageWritable() const { return true; }

			virtual bool getForceNoText() const = 0;
			virtual void setForceNoText(bool force) = 0;
			virtual bool isForceNoTextNull() const { return false; }
			virtual void setForceNoTextNull() = 0;
			virtual bool isForceNoTextWritable() const { return true; }

			virtual bool getLinkedToProductionPlanning() const = 0;
			virtual void setLinkedToProductionPlanning(bool force) = 0;
			virtual bool isLinkedToProductionPlanningNull() const { return false; }
			virtual void setLinkedToProductionPlanningNull() = 0;
			virtual bool isLinkedToProductionPlanningWritable() const { return true; }

			virtual int getAction() const = 0;
			virtual void setAction(int type) = 0;
			virtual bool isActionNull() const { return false; }
			virtual void setActionNull() = 0;
			virtual bool isActionWritable() const { return true; }

			virtual int getActionSubtype() const = 0;
			virtual void setActionSubtype(int subtype) = 0;
			virtual bool isActionSubtypeNull() const { return false; }
			virtual void setActionSubtypeNull() = 0;
			virtual bool isActionSubtypeWritable() const { return true; }

			virtual int getActionIntegerParameter() const = 0;
			virtual void setActionIntegerParameter(int intParam) = 0;
			virtual bool isActionIntegerParameterNull() const { return false; }
			virtual void setActionIntegerParameterNull() = 0;
			virtual bool isActionIntegerParameterWritable() const { return true; }

			virtual std::string getActionStringParameter() const = 0;
			virtual void setActionStringParameter(const std::string& strParam) = 0;
			virtual bool isActionStringParameterNull() const { return false; }
			virtual void setActionStringParameterNull() = 0;
			virtual bool isActionStringParameterWritable() const { return true; }

			// These following 4 methods are here for backwards compatibility to
			// not brake older code. They are deprecated.
			virtual void setActionProduct(const DBProductI* p) = 0;
			virtual bool isActionProductNull() const { return false; }
			virtual void setActionProductNull() = 0;
			virtual bool isActionProductWritable() const { return true; }

			virtual std::string getActionProductNumber() const = 0;
			virtual void setActionProductNumber(const std::string& pn) = 0;
			virtual bool isActionProductNumberNull() const { return false; }
			virtual void setActionProductNumberNull() = 0;
			virtual bool isActionProductNumberWritable() const { return true; }

			virtual std::string getActionCategoryNumber() const = 0;
			virtual void setActionCategoryNumber(const std::string& pcn) = 0;
			virtual bool isActionCategoryNumberNull() const { return false; }
			virtual void setActionCategoryNumberNull() = 0;
			virtual bool isActionCategoryNumberWritable() const { return true; }

			virtual std::string getActionOperatorNumber() const = 0;
			virtual void setActionOperatorNumber(const std::string& pcn) = 0;
			virtual bool isActionOperatorNumberNull() const { return false; }
			virtual void setActionOperatorNumberNull() = 0;
			virtual bool isActionOperatorNumberWritable() const { return true; }

			virtual std::string getActionFeatureNumber() const = 0;
			virtual void setActionFeatureNumber(const std::string& pcn) = 0;
			virtual bool isActionFeatureNumberNull() const { return false; }
			virtual void setActionFeatureNumberNull() = 0;
			virtual bool isActionFeatureNumberWritable() const { return true; }
			
			virtual std::string getScriptName() const  = 0;
			virtual void setScriptName(const std::string& name) = 0;
			virtual bool isScriptNameNull() const { return false; }
			virtual void setScriptNameNull() = 0;
			virtual bool isScriptNameWriteable() const { return true; }
			
			virtual int getScriptInterval() const = 0;
			virtual void setScriptInterval(const int interval) = 0;
			virtual bool isScriptIntervalNull() const { return false; }
			virtual void setScriptIntervalNull() = 0;
			virtual bool isScriptIntervalWriteable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBSOFTKEYI_H
