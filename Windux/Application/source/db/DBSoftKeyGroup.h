#ifndef DBSOFTKEYGROUP_H
#define DBSOFTKEYGROUP_H


#include <string>


#include "edb.h"
#include DBSOFTKEYGROUPI_INC
#if defined( __linux__ )
    #include DBSOFTKEYGROUPSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBSoftKeyGroup
	{
			// DBSoftKeyGroupsRM class is a friend of DBSoftKeyGroup because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBSoftKeyGroupsRM;
			friend class DBSoftKeysRM;
			friend class DBQuery<DBSoftKeyGroup>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBSoftKeyGroupI interface_type;
			typedef DBSoftKeyGroupsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBSoftKeyGroup(const DBSoftKeyGroup& rec);
			~DBSoftKeyGroup();


			// Methods associated with DBSoftKeyGroupI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& gn) { decorated_->setNumber(gn); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

			int getHorizontalGridSize() const { return decorated_->getHorizontalGridSize(); }
			void setHorizontalGridSize(int hs) { decorated_->setHorizontalGridSize(hs); }
			bool isHorizontalGridSizeNull() const { return decorated_->isHorizontalGridSizeNull(); }
			void setHorizontalGridSizeNull() { decorated_->setHorizontalGridSizeNull(); }
			bool isHorizontalGridSizeWritable() const { return decorated_->isHorizontalGridSizeWritable(); }

			int getVerticalGridSize() const { return decorated_->getVerticalGridSize(); }
			void setVerticalGridSize(int vs) { decorated_->setVerticalGridSize(vs); }
			bool isVerticalGridSizeNull() const { return decorated_->isVerticalGridSizeNull(); }
			void setVerticalGridSizeNull() { decorated_->setVerticalGridSizeNull(); }
			bool isVerticalGridSizeWritable() const { return decorated_->isVerticalGridSizeWritable(); }

			int getHorizontalSpacing() const { return decorated_->getHorizontalSpacing(); }
			void setHorizontalSpacing(int hs) { decorated_->setHorizontalSpacing(hs); }
			bool isHorizontalSpacingNull() const { return decorated_->isHorizontalSpacingNull(); }
			void setHorizontalSpacingNull() { decorated_->setHorizontalSpacingNull(); }
			bool isHorizontalSpacingWritable() const { return decorated_->isHorizontalSpacingWritable(); }

			int getVerticalSpacing() const { return decorated_->getVerticalSpacing(); }
			void setVerticalSpacing(int vs) { decorated_->setVerticalSpacing(vs); }
			bool isVerticalSpacingNull() const { return decorated_->isVerticalSpacingNull(); }
			void setVerticalSpacingNull() { decorated_->setVerticalSpacingNull(); }
			bool isVerticalSpacingWritable() const { return decorated_->isVerticalSpacingWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBSoftKeyGroup& rec);


		private:

			edb_adapter::DBSoftKeyGroupI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBSoftKeyGroup(edb_adapter::DBSoftKeyGroupI* d);

			// Do not allow the default assignment operator.
			DBSoftKeyGroup operator=(const DBSoftKeyGroup& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSOFTKEYGROUP_H
