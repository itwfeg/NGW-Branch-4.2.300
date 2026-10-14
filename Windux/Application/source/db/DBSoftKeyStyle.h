#ifndef DBSOFTKEYSTYLE_H
#define DBSOFTKEYSTYLE_H


#include <string>


#include "edb.h"
#include DBSOFTKEYSTYLEI_INC
#if defined( __linux__ )
    #include DBSOFTKEYSTYLESRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBSoftKeyStyle
	{
			// DBSoftKeyStylesRM class is a friend of DBSoftKeyStyle because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBSoftKeyStylesRM;
			friend class DBQuery<DBSoftKeyStyle>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBSoftKeyStyleI interface_type;
			typedef DBSoftKeyStylesRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBSoftKeyStyle(const DBSoftKeyStyle& rec);
			~DBSoftKeyStyle();


			// Methods associated with DBSoftKeyStyleI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& gn) { decorated_->setNumber(gn); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			int getDefaultTextLocation() const { return decorated_->getDefaultTextLocation(); }
			void setDefaultTextLocation(int hs) { decorated_->setDefaultTextLocation(hs); }
			bool isDefaultTextLocationNull() const { return decorated_->isDefaultTextLocationNull(); }
			void setDefaultTextLocationNull() { decorated_->setDefaultTextLocationNull(); }
			bool isDefaultTextLocationWritable() const { return decorated_->isDefaultTextLocationWritable(); }

			int getDefaultTextColor() const { return decorated_->getDefaultTextColor(); }
			void setDefaultTextColor(int hs) { decorated_->setDefaultTextColor(hs); }
			bool isDefaultTextColorNull() const { return decorated_->isDefaultTextColorNull(); }
			void setDefaultTextColorNull() { decorated_->setDefaultTextColorNull(); }
			bool isDefaultTextColorWritable() const { return decorated_->isDefaultTextColorWritable(); }

			int getDefaultGrayedOutTextColor() const { return decorated_->getDefaultGrayedOutTextColor(); }
			void setDefaultGrayedOutTextColor(int hs) { decorated_->setDefaultGrayedOutTextColor(hs); }
			bool isDefaultGrayedOutTextColorNull() const { return decorated_->isDefaultGrayedOutTextColorNull(); }
			void setDefaultGrayedOutTextColorNull() { decorated_->setDefaultGrayedOutTextColorNull(); }
			bool isDefaultGrayedOutTextColorWritable() const { return decorated_->isDefaultGrayedOutTextColorWritable(); }

			int getDefaultForegroundImageLocation() const { return decorated_->getDefaultForegroundImageLocation(); }
			void setDefaultForegroundImageLocation(int hs) { decorated_->setDefaultForegroundImageLocation(hs); }
			bool isDefaultForegroundImageLocationNull() const { return decorated_->isDefaultForegroundImageLocationNull(); }
			void setDefaultForegroundImageLocationNull() { decorated_->setDefaultForegroundImageLocationNull(); }
			bool isDefaultForegroundImageLocationWritable() const { return decorated_->isDefaultForegroundImageLocationWritable(); }

			int getDefaultBackgroundColor() const { return decorated_->getDefaultBackgroundColor(); }
			void setDefaultBackgroundColor(int hs) { decorated_->setDefaultBackgroundColor(hs); }
			bool isDefaultBackgroundColorNull() const { return decorated_->isDefaultBackgroundColorNull(); }
			void setDefaultBackgroundColorNull() { decorated_->setDefaultBackgroundColorNull(); }
			bool isDefaultBackgroundColorWritable() const { return decorated_->isDefaultBackgroundColorWritable(); }

			std::string getDefaultBackgroundImage() const { return decorated_->getDefaultBackgroundImage(); }
			void setDefaultBackgroundImage(const std::string& dbi) { decorated_->setDefaultBackgroundImage(dbi); }
			bool isDefaultBackgroundImageNull() const { return decorated_->isDefaultBackgroundImageNull(); }
			void setDefaultBackgroundImageNull() { decorated_->setDefaultBackgroundImageNull(); }
			bool isDefaultBackgroundImageWritable() const { return decorated_->isDefaultBackgroundImageWritable(); }

			std::string getImagesDirectory() const { return decorated_->getImagesDirectory(); }
			void setImagesDirectory(const std::string& id) { decorated_->setImagesDirectory(id); }
			bool isImagesDirectoryNull() const { return decorated_->isImagesDirectoryNull(); }
			void setImagesDirectoryNull() { decorated_->setImagesDirectoryNull(); }
			bool isImagesDirectoryWritable() const { return decorated_->isImagesDirectoryWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBSoftKeyStyle& rec);


		private:

			edb_adapter::DBSoftKeyStyleI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBSoftKeyStyle(edb_adapter::DBSoftKeyStyleI* d);

			// Do not allow the default assignment operator.
			DBSoftKeyStyle operator=(const DBSoftKeyStyle& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSOFTKEYSTYLE_H
