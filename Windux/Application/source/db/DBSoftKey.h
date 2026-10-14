#ifndef DBSOFTKEY_H
#define DBSOFTKEY_H


#include <string>


#include "edb.h"
#include DBSOFTKEYI_INC
#if defined( __linux__ )
    #include DBSOFTKEYSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBProduct;
	template <class DBEntity> class DBQuery;


	class DBSoftKey
	{
			// DBSoftKeysRM class is a friend of DBSoftKey because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBSoftKeysRM;
			friend class DBQuery<DBSoftKey>;
			friend class DBProductsRM;

		public:

			// Types and subtypes
			typedef edb_adapter::DBSoftKeyI interface_type;
			typedef DBSoftKeysRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBSoftKey(const DBSoftKey& rec);
			~DBSoftKey();


			// Methods associated with DBSoftKeyI
			std::string getGroupNumber() const { return decorated_->getGroupNumber(); }
			void setGroupNumber(const std::string& gn) { decorated_->setGroupNumber(gn); }
			bool isGroupNumberNull() const { return decorated_->isGroupNumberNull(); }
			bool isGroupNumberWritable() const { return decorated_->isGroupNumberWritable(); }

			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

			std::string getHorizontalPosition() const { return decorated_->getHorizontalPosition(); }
			void setHorizontalPosition(const std::string& hp) { decorated_->setHorizontalPosition(hp); }
			bool isHorizontalPositionNull() const { return decorated_->isHorizontalPositionNull(); }
			bool isHorizontalPositionWritable() const { return decorated_->isHorizontalPositionWritable(); }

			std::string getVerticalPosition() const { return decorated_->getVerticalPosition(); }
			void setVerticalPosition(const std::string& vp) { decorated_->setVerticalPosition(vp); }
			bool isVerticalPositionNull() const { return decorated_->isVerticalPositionNull(); }
			bool isVerticalPositionWritable() const { return decorated_->isVerticalPositionWritable(); }

			int getHorizontalSize() const { return decorated_->getHorizontalSize(); }
			void setHorizontalSize(int hs) { decorated_->setHorizontalSize(hs); }
			bool isHorizontalSizeNull() const { return decorated_->isHorizontalSizeNull(); }
			void setHorizontalSizeNull() { decorated_->setHorizontalSizeNull(); }
			bool isHorizontalSizeWritable() const { return decorated_->isHorizontalSizeWritable(); }

			int getVerticalSize() const { return decorated_->getVerticalSize(); }
			void setVerticalSize(int vs) { decorated_->setVerticalSize(vs); }
			bool isVerticalSizeNull() const { return decorated_->isVerticalSizeNull(); }
			void setVerticalSizeNull() { decorated_->setVerticalSizeNull(); }
			bool isVerticalSizeWritable() const { return decorated_->isVerticalSizeWritable(); }

			int getTextColor() const { return decorated_->getTextColor(); }
			void setTextColor(int tc) { decorated_->setTextColor(tc); }
			bool isTextColorNull() const { return decorated_->isTextColorNull(); }
			void setTextColorNull() { decorated_->setTextColorNull(); }
			bool isTextColorWritable() const { return decorated_->isTextColorWritable(); }

			int getBackgroundColor() const { return decorated_->getBackgroundColor(); }
			void setBackgroundColor(int bc) { decorated_->setBackgroundColor(bc); }
			bool isBackgroundColorNull() const { return decorated_->isBackgroundColorNull(); }
			void setBackgroundColorNull() { decorated_->setBackgroundColorNull(); }
			bool isBackgroundColorWritable() const { return decorated_->isBackgroundColorWritable(); }

			int getToggledBackgroundColor() const { return decorated_->getToggledBackgroundColor(); }
			void setToggledBackgroundColor(int bc) { decorated_->setToggledBackgroundColor(bc); }
			bool isToggledBackgroundColorNull() const { return decorated_->isToggledBackgroundColorNull(); }
			void setToggledBackgroundColorNull() { decorated_->setToggledBackgroundColorNull(); }
			bool isToggledBackgroundColorWritable() const { return decorated_->isToggledBackgroundColorWritable(); }

			std::string getBackgroundImage() const { return decorated_->getBackgroundImage(); }
			void setBackgroundImage(const std::string& image) { decorated_->setBackgroundImage(image); }
			bool isBackgroundImageNull() const { return decorated_->isBackgroundImageNull(); }
			void setBackgroundImageNull() { decorated_->setBackgroundImageNull(); }
			bool isBackgroundImageWritable() const { return decorated_->isBackgroundImageWritable(); }

			std::string getToggledBackgroundImage() const { return decorated_->getToggledBackgroundImage(); }
			void setToggledBackgroundImage(const std::string& image) { decorated_->setToggledBackgroundImage(image); }
			bool isToggledBackgroundImageNull() const { return decorated_->isToggledBackgroundImageNull(); }
			void setToggledBackgroundImageNull() { decorated_->setToggledBackgroundImageNull(); }
			bool isToggledBackgroundImageWritable() const { return decorated_->isToggledBackgroundImageWritable(); }

			std::string getName1() const { return decorated_->getName1(); }
			void setName1(const std::string& name) { decorated_->setName1(name); }
			bool isName1Null() const { return decorated_->isName1Null(); }
			void setName1Null() { decorated_->setName1Null(); }
			bool isName1Writable() const { return decorated_->isName1Writable(); }

			std::string getName2() const { return decorated_->getName2(); }
			void setName2(const std::string& name) { decorated_->setName2(name); }
			bool isName2Null() const { return decorated_->isName2Null(); }
			void setName2Null() { decorated_->setName2Null(); }
			bool isName2Writable() const { return decorated_->isName2Writable(); }

			bool getForceNoBackgroundColor() const { return decorated_->getForceNoBackgroundColor(); }
			void setForceNoBackgroundColor(bool force) { decorated_->setForceNoBackgroundColor(force); }
			bool isForceNoBackgroundColorNull() const { return decorated_->isForceNoBackgroundColorNull(); }
			void setForceNoBackgroundColorNull() { decorated_->setForceNoBackgroundColorNull(); }
			bool isForceNoBackgroundColorWritable() const { return decorated_->isForceNoBackgroundColorWritable(); }

			bool getForceNoBackgroundImage() const { return decorated_->getForceNoBackgroundImage(); }
			void setForceNoBackgroundImage(bool force) { decorated_->setForceNoBackgroundImage(force); }
			bool isForceNoBackgroundImageNull() const { return decorated_->isForceNoBackgroundImageNull(); }
			void setForceNoBackgroundImageNull() { decorated_->setForceNoBackgroundImageNull(); }
			bool isForceNoBackgroundImageWritable() const { return decorated_->isForceNoBackgroundImageWritable(); }

			bool getForceNoText() const { return decorated_->getForceNoText(); }
			void setForceNoText(bool force) { decorated_->setForceNoText(force); }
			bool isForceNoTextNull() const { return decorated_->isForceNoTextNull(); }
			void setForceNoTextNull() { decorated_->setForceNoTextNull(); }
			bool isForceNoTextWritable() const { return decorated_->isForceNoTextWritable(); }

			bool getLinkedToProductionPlanning() const { return decorated_->getLinkedToProductionPlanning(); }
			void setLinkedToProductionPlanning(bool force) { decorated_->setLinkedToProductionPlanning(force); }
			bool isLinkedToProductionPlanningNull() const { return decorated_->isLinkedToProductionPlanningNull(); }
			void setLinkedToProductionPlanningNull() { decorated_->setLinkedToProductionPlanningNull(); }
			bool isLinkedToProductionPlanningWritable() const { return decorated_->isLinkedToProductionPlanningWritable(); }

			int getAction() const { return decorated_->getAction(); }
			void setAction(int type) { decorated_->setAction(type); }
			bool isActionNull() const { return decorated_->isActionNull(); }
			void setActionNull() { decorated_->setActionNull(); }
			bool isActionWritable() const { return decorated_->isActionWritable(); }

			int getActionSubtype() const { return decorated_->getActionSubtype(); }
			void setActionSubtype(int subtype) { decorated_->setActionSubtype(subtype); }
			bool isActionSubtypeNull() const { return decorated_->isActionSubtypeNull(); }
			void setActionSubtypeNull() { decorated_->setActionSubtypeNull(); }
			bool isActionSubtypeWritable() const { return decorated_->isActionSubtypeWritable(); }

			int getActionIntegerParameter() const { return decorated_->getActionIntegerParameter(); }
			void setActionIntegerParameter(int intparam) { decorated_->setActionIntegerParameter(intparam); }
			bool isActionIntegerParameterNull() const { return decorated_->isActionIntegerParameterNull(); }
			void setActionIntegerParameterNull() { decorated_->setActionIntegerParameterNull(); }
			bool isActionIntegerParameterWritable() const { return decorated_->isActionIntegerParameterWritable(); }

			std::string getActionStringParameter() const { return decorated_->getActionStringParameter(); }
			void setActionStringParameter(const std::string& strparam) { decorated_->setActionStringParameter(strparam); }
			bool isActionStringParameterNull() const { return decorated_->isActionStringParameterNull(); }
			void setActionStringParameterNull() { decorated_->setActionStringParameterNull(); }
			bool isActionStringParameterWritable() const { return decorated_->isActionStringParameterWritable(); }

			// These following 4 methods are here for backwards compatibility to
			// not brake older code. They are deprecated.
			void setActionProduct(const DBProduct* p);
			bool isActionProductNull() const { return decorated_->isActionProductNull(); }
			void setActionProductNull() { decorated_->setActionProductNull(); }
			bool isActionProductWritable() const { return decorated_->isActionProductWritable(); }

			std::string getActionProductNumber() const { return decorated_->getActionProductNumber(); }
			void setActionProductNumber(const std::string& pn) { decorated_->setActionProductNumber(pn); }
			bool isActionProductNumberNull() const { return decorated_->isActionProductNumberNull(); }
			void setActionProductNumberNull() { decorated_->setActionProductNumberNull(); }
			bool isActionProductNumberWritable() const { return decorated_->isActionProductNumberWritable(); }

			std::string getActionCategoryNumber() const { return decorated_->getActionCategoryNumber(); }
			void setActionCategoryNumber(const std::string& pcn) { decorated_->setActionCategoryNumber(pcn); }
			bool isActionCategoryNumberNull() const { return decorated_->isActionCategoryNumberNull(); }
			void setActionCategoryNumberNull() { decorated_->setActionCategoryNumberNull(); }
			bool isActionCategoryNumberWritable() const { return decorated_->isActionCategoryNumberWritable(); }

			std::string getActionOperatorNumber() const { return decorated_->getActionOperatorNumber(); }
			void setActionOperatorNumber(const std::string& pcn) { decorated_->setActionOperatorNumber(pcn); }
			bool isActionOperatorNumberNull() const { return decorated_->isActionOperatorNumberNull(); }
			void setActionOperatorNumberNull() { decorated_->setActionOperatorNumberNull(); }
			bool isActionOperatorNumberWritable() const { return decorated_->isActionOperatorNumberWritable(); }

			std::string getActionFeatureNumber() const { return decorated_->getActionFeatureNumber(); }
			void setActionFeatureNumber(const std::string& pcn) { decorated_->setActionFeatureNumber(pcn); }
			bool isActionFeatureNumberNull() const { return decorated_->isActionFeatureNumberNull(); }
			void setActionFeatureNumberNull() { decorated_->setActionFeatureNumberNull(); }
			bool isActionFeatureNumberWritable() const { return decorated_->isActionFeatureNumberWritable(); }
			
			std::string getScriptName() const { return decorated_->getScriptName(); }
			void setScriptName(const std::string& name) { return decorated_->setScriptName(name); }
			bool isScriptNameNull() const { return decorated_->isScriptNameNull(); }
			void setScriptNameNull() { return decorated_->setScriptNameNull(); }
			bool isScriptNameWriteable() { return decorated_->isScriptNameWriteable(); }

			int getScriptInterval() const { return decorated_->getScriptInterval(); }
			void setScriptInterval(const int interval) { return decorated_->setScriptInterval(interval); }
			bool isScriptIntervalNull() const { return decorated_->isScriptIntervalNull(); }
			void setScriptIntervalNull() { return decorated_->setScriptIntervalNull(); }
			bool isScriptIntervalWriteable() const { return decorated_->isScriptIntervalWriteable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey1() const { return decorated_->getGroupNumber(); }
			void setKey1(const std::string& key) { decorated_->setGroupNumber(key); }
			std::string getKey2() const { return decorated_->getHorizontalPosition(); }
			void setKey2(const std::string& key) { decorated_->setHorizontalPosition(key); }
			std::string getKey3() const { return decorated_->getVerticalPosition(); }
			void setKey3(const std::string& key) { decorated_->setVerticalPosition(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBSoftKey& rec);


		private:

			edb_adapter::DBSoftKeyI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBSoftKey(edb_adapter::DBSoftKeyI* d);

			// Do not allow the default assignment operator.
			DBSoftKey operator=(const DBSoftKey& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSOFTKEY_H
