#ifndef SQLITESOFTKEYGROUP_H
#define SQLITESOFTKEYGROUP_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBSOFTKEYGROUPI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteSoftKeyGroup : public DBSoftKeyGroupI
	{
		public:

			// Types and subtypes
			typedef DBSoftKeyGroupI interface_type;


			// Constructors and destructor
			SqliteSoftKeyGroup(SqliteRecord* r, DBSoftKeyGroupI* d = 0);
			~SqliteSoftKeyGroup();


			// Think about replacing this method with two friend functions from the
			// SqliteSoftKeyGroupsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBSoftKeyGroupI
			virtual DBSoftKeyGroupI* clone() const { return new SqliteSoftKeyGroup(new SqliteRecord(*rec_)); }
			virtual const DBSoftKeyGroupI* getDecorated() const { return decorated_; }

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

			virtual int getHorizontalGridSize() const { return rec_->getField(HORIZONTALGRIDSIZE_FIELD); }
			virtual void setHorizontalGridSize(int dhgs) { rec_->setField(HORIZONTALGRIDSIZE_FIELD, dhgs); }
			virtual bool isHorizontalGridSizeNull() const { return rec_->isFieldNull(HORIZONTALGRIDSIZE_FIELD); }
			virtual void setHorizontalGridSizeNull() { rec_->setFieldNull(HORIZONTALGRIDSIZE_FIELD); }
			virtual bool isHorizontalGridSizeWritable() const { return true; }

			virtual int getVerticalGridSize() const { return rec_->getField(VERTICALGRIDSIZE_FIELD); }
			virtual void setVerticalGridSize(int dvgs) { rec_->setField(VERTICALGRIDSIZE_FIELD, dvgs); }
			virtual bool isVerticalGridSizeNull() const { return rec_->isFieldNull(VERTICALGRIDSIZE_FIELD); }
			virtual void setVerticalGridSizeNull() { rec_->setFieldNull(VERTICALGRIDSIZE_FIELD); }
			virtual bool isVerticalGridSizeWritable() const { return true; }

			virtual int getHorizontalSpacing() const { return rec_->getField(HORIZONTALSPACING_FIELD); }
			virtual void setHorizontalSpacing(int hs) { rec_->setField(HORIZONTALSPACING_FIELD, hs); }
			virtual bool isHorizontalSpacingNull() const { return rec_->isFieldNull(HORIZONTALSPACING_FIELD); }
			virtual void setHorizontalSpacingNull() { rec_->setFieldNull(HORIZONTALSPACING_FIELD); }
			virtual bool isHorizontalSpacingWritable() const { return true; }

			virtual int getVerticalSpacing() const { return rec_->getField(VERTICALSPACING_FIELD); }
			virtual void setVerticalSpacing(int vs) { rec_->setField(VERTICALSPACING_FIELD, vs); }
			virtual bool isVerticalSpacingNull() const { return rec_->isFieldNull(VERTICALSPACING_FIELD); }
			virtual void setVerticalSpacingNull() { rec_->setFieldNull(VERTICALSPACING_FIELD); }
			virtual bool isVerticalSpacingWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string HORIZONTALGRIDSIZE_FIELD;
			static const std::string VERTICALGRIDSIZE_FIELD;
			static const std::string HORIZONTALSPACING_FIELD;
			static const std::string VERTICALSPACING_FIELD;


		private:

			SqliteRecord* rec_;
			DBSoftKeyGroupI* decorated_;

			// Do not allow the default copy constructor.
			SqliteSoftKeyGroup(const SqliteSoftKeyGroup& rec);
			// Do not allow the default assignment operator.
			SqliteSoftKeyGroup operator=(const SqliteSoftKeyGroup& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESOFTKEYGROUP_H
