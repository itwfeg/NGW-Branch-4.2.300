#ifndef DBCATEGORY_H
#define DBCATEGORY_H


#include <string>


#include "edb.h"
#include DBCATEGORYI_INC
#if defined( __linux__ )
    #include DBCATEGORIESRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBCategory
	{
			// DBCategoriesRM class is a friend of DBCategory because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBCategoriesRM;
			friend class DBQuery<DBCategory>;
			friend class DBProductsRM;     // Needs access to decorated_ for cross-referencing.
			friend class DBCategoryStatisticsRM;

		public:

			// Types and subtypes
			typedef edb_adapter::DBCategoryI interface_type;
			typedef DBCategoriesRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBCategory(const DBCategory& rec);  
			~DBCategory();


			// Methods associated with DBCategoryI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

			std::string getParentCategory() const { return decorated_->getParentCategory(); }
			void setParentCategory(const std::string& rc) { decorated_->setParentCategory(rc); }
			bool isParentCategoryNull() const { return decorated_->isParentCategoryNull(); }
			void setParentCategoryNull() { decorated_->setParentCategoryNull(); }
			bool isParentCategoryWritable() const { return decorated_->isParentCategoryWritable(); }

			std::string getName1() const { return decorated_->getName1(); }
			void setName1(const std::string& desc) { decorated_->setName1(desc); }
			bool isName1Null() const { return decorated_->isName1Null(); }
			void setName1Null() { decorated_->setName1Null(); }
			bool isName1Writable() const { return decorated_->isName1Writable(); }

			std::string getName2() const { return decorated_->getName2(); }
			void setName2(const std::string& desc) { decorated_->setName2(desc); }
			bool isName2Null() const { return decorated_->isName2Null(); }
			void setName2Null() { decorated_->setName2Null(); }
			bool isName2Writable() const { return decorated_->isName2Writable(); }

			int getBackgroundColor() const { return decorated_->getBackgroundColor(); }
			void setBackgroundColor(int bc) { decorated_->setBackgroundColor(bc); }
			bool isBackgroundColorNull() const { return decorated_->isBackgroundColorNull(); }
			void setBackgroundColorNull() { decorated_->setBackgroundColorNull(); }
			bool isBackgroundColorWritable() const { return decorated_->isBackgroundColorWritable(); }

			int getTextColor() const { return decorated_->getTextColor(); }
			void setTextColor(int tc) { decorated_->setTextColor(tc); }
			bool isTextColorNull() const { return decorated_->isTextColorNull(); }
			void setTextColorNull() { decorated_->setTextColorNull(); }
			bool isTextColorWritable() const { return decorated_->isTextColorWritable(); }

			std::string getImage() const { return decorated_->getImage(); }
			void setImage(const std::string& desc) { decorated_->setImage(desc); }
			bool isImageNull() const { return decorated_->isImageNull(); }
			void setImageNull() { decorated_->setImageNull(); }
			bool isImageWritable() const { return decorated_->isImageWritable(); }

			std::string getPlaylist() const { return decorated_->getPlaylist(); }
			void setPlaylist(const std::string& list) { decorated_->setPlaylist(list); }
			bool isPlaylistNull() const { return decorated_->isPlaylistNull(); }
			void setPlaylistNull() { decorated_->setPlaylistNull(); }
			bool isPlaylistWritable() const { return decorated_->isPlaylistWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBCategory& rec);


		private:

			edb_adapter::DBCategoryI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBCategory(edb_adapter::DBCategoryI* d);

			// Do not allow the default assignment operator.
			DBCategory operator=(const DBCategory& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCATEGORY_H
