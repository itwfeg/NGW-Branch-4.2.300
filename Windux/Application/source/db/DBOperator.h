#ifndef DBOPERATOR_H
#define DBOPERATOR_H


#include <string>


#include "edb.h"
#include DBOPERATORI_INC
#if defined( __linux__ )
    #include DBOPERATORSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBOperator
	{
			// DBOperatorsRM class is a friend of DBOperator because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBOperatorsRM;
			friend class DBQuery<DBOperator>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBOperatorI interface_type;
			typedef DBOperatorsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBOperator(const DBOperator& rec);   
			~DBOperator();


			// Methods from DBOperatorI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getName() const { return decorated_->getName(); }
			void setName(const std::string& name) { decorated_->setName(name); }
			bool isNameNull() const { return decorated_->isNameNull(); }
			void setNameNull() { decorated_->setNameNull(); }
			bool isNameWritable() const { return decorated_->isNameWritable(); }

			std::string getLanguage() const { return decorated_->getLanguage(); }
			void setLanguage(const std::string&  lang) { decorated_->setLanguage(lang); }
			bool isLanguageNull() const { return decorated_->isLanguageNull(); }
			void setLanguageNull() { decorated_->setLanguageNull(); }
			bool isLanguageWritable() const { return decorated_->isLanguageWritable(); }
			
			std::string getRFID() const { return decorated_->getRFID(); }
			void setRFID(const std::string& id) { decorated_->setRFID(id); }
			bool isRFIDNull() const { return decorated_->isRFIDNull(); }
			void setRFIDNull() { decorated_->setRFIDNull(); }
			bool isRFIDWritable() const { return decorated_->isRFIDWritable(); }

			int getClassification() const { return decorated_->getClassification(); }
			void setClassification(int c) { decorated_->setClassification(c); }
			bool isClassificationNull() const { return decorated_->isClassificationNull(); }
			void setClassificationNull() { decorated_->setClassificationNull(); }
			bool isClassificationWritable() const { return decorated_->isClassificationWritable(); }

			bool getSawNote() const { return decorated_->getSawNote(); }
			void setSawNote(bool saw) { decorated_->setSawNote(saw); }
			bool isSawNoteNull() const { return decorated_->isSawNoteNull(); }
			void setSawNoteNull() { decorated_->setSawNoteNull(); }
			bool isSawNoteWritable() const { return decorated_->isSawNoteWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer. 
			friend std::ostream& operator<<(std::ostream& os, const DBOperator& rec);


		private:

			edb_adapter::DBOperatorI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBOperator(edb_adapter::DBOperatorI* d);

			// Do not allow the default assignment operator.
			DBOperator operator=(const DBOperator& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBOPERATOR_H
