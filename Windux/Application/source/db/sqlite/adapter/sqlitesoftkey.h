#ifndef SQLITESOFTKEY_H
#define SQLITESOFTKEY_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"
#include "Bool.h"

#include DBSOFTKEYI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteSoftKey : public DBSoftKeyI
	{
		public:

			// Types and subtypes
			typedef DBSoftKeyI interface_type;


			// Constructors and destructor
			SqliteSoftKey(SqliteRecord* r, DBSoftKeyI* d = 0);
			~SqliteSoftKey();


			// Think about replacing this method with two friend functions from the
			// SqliteSoftKeysRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBSoftKeyI
			virtual DBSoftKeyI* clone() const { return new SqliteSoftKey(new SqliteRecord(*rec_)); }
			virtual const DBSoftKeyI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getGroupNumber() const { return rec_->getField(GROUPNUMBER_FIELD); }
			virtual void setGroupNumber(const std::string& gn) { rec_->setField(GROUPNUMBER_FIELD, gn); }
			virtual bool isGroupNumberNull() const { return rec_->isFieldNull(GROUPNUMBER_FIELD); }
			virtual bool isGroupNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const { return rec_->getField(DEPTNUM_FIELD); }
			virtual void setDeptNumber(const std::string& number) { rec_->setField(DEPTNUM_FIELD, number); }
			virtual bool isDeptNumberNull() const { return rec_->isFieldNull(DEPTNUM_FIELD); }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual std::string getHorizontalPosition() const { return rec_->getField(HORIZONTALPOSITION_FIELD); }
			virtual void setHorizontalPosition(const std::string& hp) { rec_->setField(HORIZONTALPOSITION_FIELD, hp); }
			virtual bool isHorizontalPositionNull() const { return rec_->isFieldNull(HORIZONTALPOSITION_FIELD); }
			virtual bool isHorizontalPositionWritable() const { return true; }

			virtual std::string getVerticalPosition() const { return rec_->getField(VERTICALPOSITION_FIELD); }
			virtual void setVerticalPosition(const std::string& vp) { rec_->setField(VERTICALPOSITION_FIELD, vp); }
			virtual bool isVerticalPositionNull() const { return rec_->isFieldNull(VERTICALPOSITION_FIELD); }
			virtual bool isVerticalPositionWritable() const { return true; }

			virtual std::string getActionCategoryNumber() const { return rec_->getField(ACTIONCATEGORYNUMBER_FIELD); }
			virtual void setActionCategoryNumber(const std::string& acn) { rec_->setField(ACTIONCATEGORYNUMBER_FIELD, acn); }
			virtual bool isActionCategoryNumberNull() const { return rec_->isFieldNull(ACTIONCATEGORYNUMBER_FIELD); }
			virtual void setActionCategoryNumberNull() { rec_->setFieldNull(ACTIONCATEGORYNUMBER_FIELD); }
			virtual bool isActionCategoryNumberWritable() const { return true; }

			virtual std::string getActionOperatorNumber() const { return rec_->getField(ACTIONOPERATORNUMBER_FIELD); }
			virtual void setActionOperatorNumber(const std::string& aon) { rec_->setField(ACTIONOPERATORNUMBER_FIELD, aon); }
			virtual bool isActionOperatorNumberNull() const { return rec_->isFieldNull(ACTIONOPERATORNUMBER_FIELD); }
			virtual void setActionOperatorNumberNull() { rec_->setFieldNull(ACTIONOPERATORNUMBER_FIELD); }
			virtual bool isActionOperatorNumberWritable() const { return true; }

			virtual std::string getActionFeatureNumber() const { return rec_->getField(ACTIONFEATURENUMBER_FIELD); }
			virtual void setActionFeatureNumber(const std::string& afn) { rec_->setField(ACTIONFEATURENUMBER_FIELD, afn); }
			virtual bool isActionFeatureNumberNull() const { return rec_->isFieldNull(ACTIONFEATURENUMBER_FIELD); }
			virtual void setActionFeatureNumberNull() { rec_->setFieldNull(ACTIONFEATURENUMBER_FIELD); }
			virtual bool isActionFeatureNumberWritable() const { return true; }

			virtual int getHorizontalSize() const { return rec_->getField(HORIZONTALSIZE_FIELD); }
			virtual void setHorizontalSize(int hs) { rec_->setField(HORIZONTALSIZE_FIELD, hs); }
			virtual bool isHorizontalSizeNull() const { return rec_->isFieldNull(HORIZONTALSIZE_FIELD); }
			virtual void setHorizontalSizeNull() { rec_->setFieldNull(HORIZONTALSIZE_FIELD); }
			virtual bool isHorizontalSizeWritable() const { return true; }

			virtual int getVerticalSize() const { return rec_->getField(VERTICALSIZE_FIELD); }
			virtual void setVerticalSize(int vs) { rec_->setField(VERTICALSIZE_FIELD, vs); }
			virtual bool isVerticalSizeNull() const { return rec_->isFieldNull(VERTICALSIZE_FIELD); }
			virtual void setVerticalSizeNull() { rec_->setFieldNull(VERTICALSIZE_FIELD); }
			virtual bool isVerticalSizeWritable() const { return true; }

			virtual int getBackgroundColor() const { return rec_->getField(BACKGROUNDCOLOR_FIELD); }
			virtual void setBackgroundColor(int vs) { rec_->setField(BACKGROUNDCOLOR_FIELD, vs); }
			virtual bool isBackgroundColorNull() const { return rec_->isFieldNull(BACKGROUNDCOLOR_FIELD); }
			virtual void setBackgroundColorNull() { rec_->setFieldNull(BACKGROUNDCOLOR_FIELD); }
			virtual bool isBackgroundColorWritable() const { return true; }

			virtual int getToggledBackgroundColor() const { return rec_->getField(TOGGLEDBACKGROUNDCOLOR_FIELD); }
			virtual void setToggledBackgroundColor(int vs) { rec_->setField(TOGGLEDBACKGROUNDCOLOR_FIELD, vs); }
			virtual bool isToggledBackgroundColorNull() const { return rec_->isFieldNull(TOGGLEDBACKGROUNDCOLOR_FIELD); }
			virtual void setToggledBackgroundColorNull() { rec_->setFieldNull(TOGGLEDBACKGROUNDCOLOR_FIELD); }
			virtual bool isToggledBackgroundColorWritable() const { return true; }

			virtual int getTextColor() const { return rec_->getField(TEXTCOLOR_FIELD); }
			virtual void setTextColor(int tc) { rec_->setField(TEXTCOLOR_FIELD, tc); }
			virtual bool isTextColorNull() const { return rec_->isFieldNull(TEXTCOLOR_FIELD); }
			virtual void setTextColorNull() { rec_->setFieldNull(TEXTCOLOR_FIELD); }
			virtual bool isTextColorWritable() const { return true; }

			virtual std::string getName1() const { return rec_->getField(NAME1_FIELD); }
			virtual void setName1(const std::string& name) { rec_->setField(NAME1_FIELD, name); }
			virtual bool isName1Null() const { return rec_->isFieldNull(NAME1_FIELD); }
			virtual void setName1Null() { rec_->setFieldNull(NAME1_FIELD); }
			virtual bool isName1Writable() const { return true; }

			virtual std::string getName2() const { return rec_->getField(NAME2_FIELD); }
			virtual void setName2(const std::string& name) { rec_->setField(NAME2_FIELD, name); }
			virtual bool isName2Null() const { return rec_->isFieldNull(NAME2_FIELD); }
			virtual void setName2Null() { rec_->setFieldNull(NAME2_FIELD); }
			virtual bool isName2Writable() const { return true; }

			virtual bool getForceNoBackgroundColor() const { return util_string::stringtobool(rec_->getField(FORCENOBACKGROUNDCOLOR_FIELD)); }
			virtual void setForceNoBackgroundColor(bool force) { rec_->setField(FORCENOBACKGROUNDCOLOR_FIELD, Bool::toString(force)); }
			virtual bool isForceNoBackgroundColorNull() const { return rec_->isFieldNull(FORCENOBACKGROUNDCOLOR_FIELD); }
			virtual void setForceNoBackgroundColorNull() { rec_->setFieldNull(FORCENOBACKGROUNDCOLOR_FIELD); }
			virtual bool isForceNoBackgroundColorWritable() const { return true; }

			virtual bool getForceNoBackgroundImage() const { return util_string::stringtobool(rec_->getField(FORCENOBACKGROUNDIMAGE_FIELD)); }
			virtual void setForceNoBackgroundImage(bool force) { rec_->setField(FORCENOBACKGROUNDIMAGE_FIELD, Bool::toString(force)); }
			virtual bool isForceNoBackgroundImageNull() const { return rec_->isFieldNull(FORCENOBACKGROUNDIMAGE_FIELD); }
			virtual void setForceNoBackgroundImageNull() { rec_->setFieldNull(FORCENOBACKGROUNDIMAGE_FIELD); }
			virtual bool isForceNoBackgroundImageWritable() const { return true; }

			virtual bool getForceNoText() const { return util_string::stringtobool(rec_->getField(FORCENOTEXT_FIELD)); }
			virtual void setForceNoText(bool force) { rec_->setField(FORCENOTEXT_FIELD, Bool::toString(force)); }
			virtual bool isForceNoTextNull() const { return rec_->isFieldNull(FORCENOTEXT_FIELD); }
			virtual void setForceNoTextNull() { rec_->setFieldNull(FORCENOTEXT_FIELD); }
			virtual bool isForceNoTextWritable() const { return true; }

			virtual bool getLinkedToProductionPlanning() const { return util_string::stringtobool(rec_->getField(LINKEDTOPRODUCTIONPLANNING_FIELD)); }
			virtual void setLinkedToProductionPlanning(bool linked) { rec_->setField(LINKEDTOPRODUCTIONPLANNING_FIELD, Bool::toString(linked)); }
			virtual bool isLinkedToProductionPlanningNull() const { return rec_->isFieldNull(LINKEDTOPRODUCTIONPLANNING_FIELD); }
			virtual void setLinkedToProductionPlanningNull() { rec_->setFieldNull(LINKEDTOPRODUCTIONPLANNING_FIELD); }
			virtual bool isLinkedToProductionPlanningWritable() const { return true; }

			virtual int getAction() const { return rec_->getField(ACTION_FIELD); }
			virtual void setAction(int action) { rec_->setField(ACTION_FIELD, action); }
			virtual bool isActionNull() const { return rec_->isFieldNull(ACTION_FIELD); }
			virtual void setActionNull() { rec_->setFieldNull(ACTION_FIELD); }
			virtual bool isActionWritable() const { return true; }

			virtual int getActionSubtype() const { return rec_->getField(ACTIONSUBTYPE_FIELD); }
			virtual void setActionSubtype(int actionSubtype) { rec_->setField(ACTIONSUBTYPE_FIELD, actionSubtype); }
			virtual bool isActionSubtypeNull() const { return rec_->isFieldNull(ACTIONSUBTYPE_FIELD); }
			virtual void setActionSubtypeNull() { rec_->setFieldNull(ACTIONSUBTYPE_FIELD); }
			virtual bool isActionSubtypeWritable() const { return true; }

			virtual std::string getActionProductNumber() const { return rec_->getField(ACTIONPRODUCTNUMBER_FIELD); }
			virtual void setActionProductNumber(const std::string& pn) { rec_->setField(ACTIONPRODUCTNUMBER_FIELD, pn); }
			virtual bool isActionProductNumberNull() const { return rec_->isFieldNull(ACTIONPRODUCTNUMBER_FIELD); }
			virtual void setActionProductNumberNull() { rec_->setFieldNull(ACTIONPRODUCTNUMBER_FIELD); }
			virtual bool isActionProductNumberWritable() const { return true; }

			virtual int getActionIntegerParameter() const { return rec_->getField(ACTIONINTEGERPARAMETER_FIELD); }
			virtual void setActionIntegerParameter(int intParam) { rec_->setField(ACTIONINTEGERPARAMETER_FIELD, intParam); }
			virtual bool isActionIntegerParameterNull() const { return rec_->isFieldNull(ACTIONINTEGERPARAMETER_FIELD); }
			virtual void setActionIntegerParameterNull() { rec_->setFieldNull(ACTIONINTEGERPARAMETER_FIELD); }
			virtual bool isActionIntegerParameterWritable() const { return true; }

			virtual std::string getActionStringParameter() const { return rec_->getField(ACTIONSTRINGPARAMETER_FIELD); }
			virtual void setActionStringParameter(const std::string& strParam) { rec_->setField(ACTIONSTRINGPARAMETER_FIELD, strParam); }
			virtual bool isActionStringParameterNull() const { return rec_->isFieldNull(ACTIONSTRINGPARAMETER_FIELD); }
			virtual void setActionStringParameterNull() { rec_->setFieldNull(ACTIONSTRINGPARAMETER_FIELD); }
			virtual bool isActionStringParameterWritable() const { return true; }

			virtual std::string getBackgroundImage() const { return rec_->getField(BACKGROUNDIMAGE_FIELD); }
			virtual void setBackgroundImage(const std::string& image) { rec_->setField(BACKGROUNDIMAGE_FIELD, image); }
			virtual bool isBackgroundImageNull() const { return rec_->isFieldNull(BACKGROUNDIMAGE_FIELD); }
			virtual void setBackgroundImageNull() { rec_->setFieldNull(BACKGROUNDIMAGE_FIELD); }
			virtual bool isBackgroundImageWritable() const { return true; }
			
			virtual std::string getScriptName() const { return rec_->getField(SCRIPT_NAME_FIELD); }
			virtual void setScriptName(const std::string& name) { rec_->setField(SCRIPT_NAME_FIELD, name); }
			virtual bool isScriptNameNull() const { return rec_->isFieldNull(SCRIPT_NAME_FIELD); }
			virtual void setScriptNameNull() { rec_->setFieldNull(SCRIPT_NAME_FIELD); }
			virtual bool isScriptNameWriteable() const { return true; }
			
			virtual int getScriptInterval() const { return rec_->getField(SCRIPT_INTERVAL_FIELD); }
			virtual void setScriptInterval(const int interval) { rec_->setField(SCRIPT_INTERVAL_FIELD, interval); }
			virtual bool isScriptIntervalNull() const { return rec_->isFieldNull(SCRIPT_INTERVAL_FIELD); }
			virtual void setScriptIntervalNull() { rec_->setFieldNull(SCRIPT_INTERVAL_FIELD); }
			virtual bool isScriptIntervalWriteable() const { return true; }

			virtual std::string getToggledBackgroundImage() const { return rec_->getField(TOGGLEDBACKGROUNDIMAGE_FIELD); }
			virtual void setToggledBackgroundImage(const std::string& image) { rec_->setField(TOGGLEDBACKGROUNDIMAGE_FIELD, image); }
			virtual bool isToggledBackgroundImageNull() const { return rec_->isFieldNull(TOGGLEDBACKGROUNDIMAGE_FIELD); }
			virtual void setToggledBackgroundImageNull() { rec_->setFieldNull(TOGGLEDBACKGROUNDIMAGE_FIELD); }
			virtual bool isToggledBackgroundImageWritable() const { return true; }


			// These following 4 methods are here for backwards compatibility to
			// not brake older code. They are deprecated.
			virtual void setActionProduct(const DBProductI* p);
			virtual bool isActionProductNull() const { return rec_->isFieldNull(ACTIONPRODUCTNUMBER_FIELD); }
			virtual void setActionProductNull() { rec_->setFieldNull(ACTIONPRODUCTNUMBER_FIELD); }
			virtual bool isActionProductWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string GROUPNUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string HORIZONTALPOSITION_FIELD;
			static const std::string VERTICALPOSITION_FIELD;
			static const std::string ACTIONCATEGORYNUMBER_FIELD;
			static const std::string ACTIONOPERATORNUMBER_FIELD;
			static const std::string ACTIONFEATURENUMBER_FIELD;
			static const std::string HORIZONTALSIZE_FIELD;
			static const std::string VERTICALSIZE_FIELD;
			static const std::string BACKGROUNDCOLOR_FIELD;
			static const std::string TOGGLEDBACKGROUNDCOLOR_FIELD;
			static const std::string TEXTCOLOR_FIELD;
			static const std::string NAME1_FIELD;
			static const std::string NAME2_FIELD;
			static const std::string FORCENOBACKGROUNDCOLOR_FIELD;
			static const std::string FORCENOBACKGROUNDIMAGE_FIELD;
			static const std::string FORCENOTEXT_FIELD;
			static const std::string LINKEDTOPRODUCTIONPLANNING_FIELD;
			static const std::string ACTION_FIELD;
			static const std::string ACTIONSUBTYPE_FIELD;
			static const std::string ACTIONPRODUCTNUMBER_FIELD;
			static const std::string ACTIONINTEGERPARAMETER_FIELD;
			static const std::string ACTIONSTRINGPARAMETER_FIELD;
			static const std::string BACKGROUNDIMAGE_FIELD;
			static const std::string SCRIPT_NAME_FIELD;
			static const std::string SCRIPT_INTERVAL_FIELD;
			static const std::string TOGGLEDBACKGROUNDIMAGE_FIELD;


		private:

			SqliteRecord* rec_;
			DBSoftKeyI* decorated_;

			// Do not allow the default copy constructor.
			SqliteSoftKey(const SqliteSoftKey& rec);
			// Do not allow the default assignment operator.
			SqliteSoftKey operator=(const SqliteSoftKey& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESOFTKEY_H
