#ifndef SQLITESOFTKEYFEATURE_H
#define SQLITESOFTKEYFEATURE_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBSOFTKEYFEATUREI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteSoftKeyFeature : public DBSoftKeyFeatureI
	{
		public:

			// Types and subtypes
			typedef DBSoftKeyFeatureI interface_type;


			// Constructors and destructor
			SqliteSoftKeyFeature(SqliteRecord* r, DBSoftKeyFeatureI* d = 0);
			~SqliteSoftKeyFeature();


			// Think about replacing this method with two friend functions from the
			// SqliteSoftKeyFeaturesRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBSoftKeyFeatureI
			virtual DBSoftKeyFeatureI* clone() const { return new SqliteSoftKeyFeature(new SqliteRecord(*rec_)); }
			virtual const DBSoftKeyFeatureI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getNumber() const { return rec_->getField(NUMBER_FIELD); }
			virtual void setNumber(const std::string& gn) { rec_->setField(NUMBER_FIELD, gn); }
			virtual bool isNumberNull() const { return rec_->isFieldNull(NUMBER_FIELD); }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const { return rec_->getField(DEPTNUM_FIELD); }
			virtual void setDeptNumber(const std::string& number) { rec_->setField(DEPTNUM_FIELD, number); }
			virtual bool isDeptNumberNull() const { return rec_->isFieldNull(DEPTNUM_FIELD); }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual int getSoftKeyStyleNumber() const { return rec_->getField(SOFTKEYSTYLENUMBER_FIELD); }
			virtual void setSoftKeyStyleNumber(int dbc) { rec_->setField(SOFTKEYSTYLENUMBER_FIELD, dbc); }
			virtual bool isSoftKeyStyleNumberNull() const { return rec_->isFieldNull(SOFTKEYSTYLENUMBER_FIELD); }
			virtual void setSoftKeyStyleNumberNull() { rec_->setFieldNull(SOFTKEYSTYLENUMBER_FIELD); }
			virtual bool isSoftKeyStyleNumberWritable() const { return true; }

			virtual int getDefaultGroupNumber() const { return rec_->getField(DEFAULTGROUPNUMBER_FIELD); }
			virtual void setDefaultGroupNumber(int dbc) { rec_->setField(DEFAULTGROUPNUMBER_FIELD, dbc); }
			virtual bool isDefaultGroupNumberNull() const { return rec_->isFieldNull(DEFAULTGROUPNUMBER_FIELD); }
			virtual void setDefaultGroupNumberNull() { rec_->setFieldNull(DEFAULTGROUPNUMBER_FIELD); }
			virtual bool isDefaultGroupNumberWritable() const { return true; }

			virtual int getDefaultHorizontalGridSize() const { return rec_->getField(DEFAULTHORIZONTALGRIDSIZE_FIELD); }
			virtual void setDefaultHorizontalGridSize(int dhgs) { rec_->setField(DEFAULTHORIZONTALGRIDSIZE_FIELD, dhgs); }
			virtual bool isDefaultHorizontalGridSizeNull() const { return rec_->isFieldNull(DEFAULTHORIZONTALGRIDSIZE_FIELD); }
			virtual void setDefaultHorizontalGridSizeNull() { rec_->setFieldNull(DEFAULTHORIZONTALGRIDSIZE_FIELD); }
			virtual bool isDefaultHorizontalGridSizeWritable() const { return true; }

			virtual int getDefaultVerticalGridSize() const { return rec_->getField(DEFAULTVERTICALGRIDSIZE_FIELD); }
			virtual void setDefaultVerticalGridSize(int dvgs) { rec_->setField(DEFAULTVERTICALGRIDSIZE_FIELD, dvgs); }
			virtual bool isDefaultVerticalGridSizeNull() const { return rec_->isFieldNull(DEFAULTVERTICALGRIDSIZE_FIELD); }
			virtual void setDefaultVerticalGridSizeNull() { rec_->setFieldNull(DEFAULTVERTICALGRIDSIZE_FIELD); }
			virtual bool isDefaultVerticalGridSizeWritable() const { return true; }

			virtual int getDefaultHorizontalSpacing() const { return rec_->getField(DEFAULTHORIZONTALSPACING_FIELD); }
			virtual void setDefaultHorizontalSpacing(int hs) { rec_->setField(DEFAULTHORIZONTALSPACING_FIELD, hs); }
			virtual bool isDefaultHorizontalSpacingNull() const { return rec_->isFieldNull(DEFAULTHORIZONTALSPACING_FIELD); }
			virtual void setDefaultHorizontalSpacingNull() { rec_->setFieldNull(DEFAULTHORIZONTALSPACING_FIELD); }
			virtual bool isDefaultHorizontalSpacingWritable() const { return true; }

			virtual int getDefaultVerticalSpacing() const { return rec_->getField(DEFAULTVERTICALSPACING_FIELD); }
			virtual void setDefaultVerticalSpacing(int vs) { rec_->setField(DEFAULTVERTICALSPACING_FIELD, vs); }
			virtual bool isDefaultVerticalSpacingNull() const { return rec_->isFieldNull(DEFAULTVERTICALSPACING_FIELD); }
			virtual void setDefaultVerticalSpacingNull() { rec_->setFieldNull(DEFAULTVERTICALSPACING_FIELD); }
			virtual bool isDefaultVerticalSpacingWritable() const { return true; }

			virtual int getFixedRowColMethod() const { return rec_->getField(FIXEDROWCOLMETHOD_FIELD); }
			virtual void setFixedRowColMethod(int vs) { rec_->setField(FIXEDROWCOLMETHOD_FIELD, vs); }
			virtual bool isFixedRowColMethodNull() const { return rec_->isFieldNull(FIXEDROWCOLMETHOD_FIELD); }
			virtual void setFixedRowColMethodNull() { rec_->setFieldNull(FIXEDROWCOLMETHOD_FIELD); }
			virtual bool isFixedRowColMethodWritable() const { return true; }

			virtual int getDefaultButton() const { return rec_->getField(DEFAULTBUTTON_FIELD); }
			virtual void setDefaultButton(int vs) { rec_->setField(DEFAULTBUTTON_FIELD, vs); }
			virtual bool isDefaultButtonNull() const { return rec_->isFieldNull(DEFAULTBUTTON_FIELD); }
			virtual void setDefaultButtonNull() { rec_->setFieldNull(DEFAULTBUTTON_FIELD); }
			virtual bool isDefaultButtonWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string SOFTKEYSTYLENUMBER_FIELD;
			static const std::string DEFAULTGROUPNUMBER_FIELD;
			static const std::string DEFAULTHORIZONTALGRIDSIZE_FIELD;
			static const std::string DEFAULTVERTICALGRIDSIZE_FIELD;
			static const std::string DEFAULTHORIZONTALSPACING_FIELD;
			static const std::string DEFAULTVERTICALSPACING_FIELD;
			static const std::string FIXEDROWCOLMETHOD_FIELD;
			static const std::string DEFAULTBUTTON_FIELD;


		private:

			SqliteRecord* rec_;
			DBSoftKeyFeatureI* decorated_;

			// Do not allow the default copy constructor.
			SqliteSoftKeyFeature(const SqliteSoftKeyFeature& rec);
			// Do not allow the default assignment operator.
			SqliteSoftKeyFeature operator=(const SqliteSoftKeyFeature& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESOFTKEYFEATURE_H
