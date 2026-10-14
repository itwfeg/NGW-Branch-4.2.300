#ifndef DBFLASHFILE_H
#define DBFLASHFILE_H


#include <string>


#include "edb.h"
#include DBFLASHFILEI_INC
#if defined( __linux__ )
    #include DBFLASHFILESRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBFlashFile
	{
			// DBFlashFilesRM class is a friend of DBFlashFile because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBFlashFilesRM;
			friend class DBQuery<DBFlashFile>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBFlashFileI interface_type;
			typedef DBFlashFilesRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBFlashFile(const DBFlashFile& rec);   // Required for iterator interface
			~DBFlashFile();


			// Methods associated with DBFlashFileI
			std::string getName() const { return decorated_->getName(); }
			void setName(const std::string& name) { decorated_->setName(name); }
			bool isNameNull() const { return decorated_->isNameNull(); }
			bool isNameWritable() const { return decorated_->isNameWritable(); }

			std::string getDate() const { return decorated_->getDate(); }
			void setDate(const std::string& date) { decorated_->setDate(date); }
			bool isDateNull() const { return decorated_->isDateNull(); }
			void setDateNull() { decorated_->setDateNull(); }
			bool isDateWritable() const { return decorated_->isDateWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getName(); }
			void setKey(const std::string& key) { decorated_->setName(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBFlashFile& rec);


		private:

			edb_adapter::DBFlashFileI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBFlashFile(edb_adapter::DBFlashFileI* d);

			// Do not allow the default assignment operator.
			DBFlashFile operator=(const DBFlashFile& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBFLASHFILE_H
