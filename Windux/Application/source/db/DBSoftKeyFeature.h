#ifndef DBSOFTKEYFEATURE_H
#define DBSOFTKEYFEATURE_H


#include <string>


#include "edb.h"
#include DBSOFTKEYFEATUREI_INC
#if defined( __linux__ )
    #include DBSOFTKEYFEATURESRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBSoftKeyFeature
	{
			// DBSoftKeyFeaturesRM class is a friend of DBSoftKeyFeature because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBSoftKeyFeaturesRM;
			friend class DBQuery<DBSoftKeyFeature>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBSoftKeyFeatureI interface_type;
			typedef DBSoftKeyFeaturesRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBSoftKeyFeature(const DBSoftKeyFeature& rec);
			~DBSoftKeyFeature();


			// Methods associated with DBSoftKeyFeatureI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& gn) { decorated_->setNumber(gn); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

			int getDefaultHorizontalGridSize() const { return decorated_->getDefaultHorizontalGridSize(); }
			void setDefaultHorizontalGridSize(int dhgs) { decorated_->setDefaultHorizontalGridSize(dhgs); }
			bool isDefaultHorizontalGridSizeNull() const { return decorated_->isDefaultHorizontalGridSizeNull(); }
			void setDefaultHorizontalGridSizeNull() { decorated_->setDefaultHorizontalGridSizeNull(); }
			bool isDefaultHorizontalGridSizeWritable() const { return decorated_->isDefaultHorizontalGridSizeWritable(); }

			int getDefaultVerticalGridSize() const { return decorated_->getDefaultVerticalGridSize(); }
			void setDefaultVerticalGridSize(int dvgs) { decorated_->setDefaultVerticalGridSize(dvgs); }
			bool isDefaultVerticalGridSizeNull() const { return decorated_->isDefaultVerticalGridSizeNull(); }
			void setDefaultVerticalGridSizeNull() { decorated_->setDefaultVerticalGridSizeNull(); }
			bool isDefaultVerticalGridSizeWritable() const { return decorated_->isDefaultVerticalGridSizeWritable(); }

			int getDefaultHorizontalSpacing() const { return decorated_->getDefaultHorizontalSpacing(); }
			void setDefaultHorizontalSpacing(int hs) { decorated_->setDefaultHorizontalSpacing(hs); }
			bool isDefaultHorizontalSpacingNull() const { return decorated_->isDefaultHorizontalSpacingNull(); }
			void setDefaultHorizontalSpacingNull() { decorated_->setDefaultHorizontalSpacingNull(); }
			bool isDefaultHorizontalSpacingWritable() const { return decorated_->isDefaultHorizontalSpacingWritable(); }

			int getDefaultVerticalSpacing() const { return decorated_->getDefaultVerticalSpacing(); }
			void setDefaultVerticalSpacing(int vs) { decorated_->setDefaultVerticalSpacing(vs); }
			bool isDefaultVerticalSpacingNull() const { return decorated_->isDefaultVerticalSpacingNull(); }
			void setDefaultVerticalSpacingNull() { decorated_->setDefaultVerticalSpacingNull(); }
			bool isDefaultVerticalSpacingWritable() const { return decorated_->isDefaultVerticalSpacingWritable(); }

			int getDefaultGroupNumber() const { return decorated_->getDefaultGroupNumber(); }
			void setDefaultGroupNumber(int dgn) { decorated_->setDefaultGroupNumber(dgn); }
			bool isDefaultGroupNumberNull() const { return decorated_->isDefaultGroupNumberNull(); }
			void setDefaultGroupNumberNull() { decorated_->setDefaultGroupNumberNull(); }
			bool isDefaultGroupNumberWritable() const { return decorated_->isDefaultGroupNumberWritable(); }

			int getSoftKeyStyleNumber() const { return decorated_->getSoftKeyStyleNumber(); }
			void setSoftKeyStyleNumber(int dgn) { decorated_->setSoftKeyStyleNumber(dgn); }
			bool isSoftKeyStyleNumberNull() const { return decorated_->isSoftKeyStyleNumberNull(); }
			void setSoftKeyStyleNumberNull() { decorated_->setSoftKeyStyleNumberNull(); }
			bool isSoftKeyStyleNumberWritable() const { return decorated_->isSoftKeyStyleNumberWritable(); }

			int getFixedRowColMethod() const { return decorated_->getFixedRowColMethod(); }
			void setFixedRowColMethod(int frcm) { decorated_->setFixedRowColMethod(frcm); }
			bool isFixedRowColMethodNull() const { return decorated_->isFixedRowColMethodNull(); }
			void setFixedRowColMethodNull() { decorated_->setFixedRowColMethodNull(); }
			bool isFixedRowColMethodWritable() const { return decorated_->isFixedRowColMethodWritable(); }

			int getDefaultButton() const { return decorated_->getDefaultButton(); }
			void setDefaultButton(int db) { decorated_->setDefaultButton(db); }
			bool isDefaultButtonNull() const { return decorated_->isDefaultButtonNull(); }
			void setDefaultButtonNull() { decorated_->setDefaultButtonNull(); }
			bool isDefaultButtonWritable() const { return decorated_->isDefaultButtonWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBSoftKeyFeature& rec);


		private:

			edb_adapter::DBSoftKeyFeatureI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBSoftKeyFeature(edb_adapter::DBSoftKeyFeatureI* d);

			// Do not allow the default assignment operator.
			DBSoftKeyFeature operator=(const DBSoftKeyFeature& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSOFTKEYFEATURE_H
