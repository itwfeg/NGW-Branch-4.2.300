#ifndef DBSTREAMINGGRAPHIC_H
#define DBSTREAMINGGRAPHIC_H


#include <string>


#include "edb.h"
#include DBSTREAMINGGRAPHICI_INC
#if defined( __linux__ )
    #include DBSTREAMINGGRAPHICSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBStreamingGraphic
	{
			// DBStreamingGraphicsRM class is a friend of DBStreamingGraphic because it
			// needs access to its "decorated_" member, which is private and we don't
			// want to make it publicly available, neither directly nor through a
			// getter method. It also needs access to the private constructor.
			friend class DBStreamingGraphicsRM;
			friend class DBQuery<DBStreamingGraphic>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBStreamingGraphicI interface_type;
			typedef DBStreamingGraphicsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBStreamingGraphic(const DBStreamingGraphic& rec);
			~DBStreamingGraphic();


			// Methods associated with DBStreamingGraphicI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getImage() const { return decorated_->getImage(); }
			void setImage(const std::string& image) { decorated_->setImage(image); }
			bool isImageNull() const { return decorated_->isImageNull(); }
			void setImageNull() { decorated_->setImageNull(); }
			bool isImageWritable() const { return decorated_->isImageWritable(); }

			int getTime() const { return decorated_->getTime(); }
			void setTime(const int time) { decorated_->setTime(time); }
			bool isTimeNull() const { return decorated_->isTimeNull(); }
			void setTimeNull() { decorated_->setTimeNull(); }
			bool isTimeWritable() const { return decorated_->isTimeWritable(); }

			int getActionType() const { return decorated_->getActionType(); }
			void setActionType(const int type) { decorated_->setActionType(type); }
			bool isActionTypeNull() const { return decorated_->isActionTypeNull(); }
			void setActionTypeNull() { decorated_->setActionTypeNull(); }
			bool isActionTypeWritable() const { return decorated_->isActionTypeWritable(); }

			std::string getActionParameter() const { return decorated_->getActionParameter(); }
			void setActionParameter(const std::string& par) { decorated_->setActionParameter(par); }
			bool isActionParameterNull() const { return decorated_->isActionParameterNull(); }
			void setActionParameterNull() { decorated_->setActionParameterNull(); }
			bool isActionParameterWritable() const { return decorated_->isActionParameterWritable(); }

			std::string getNext() const { return decorated_->getNext(); }
			void setNext(const std::string& rc) { decorated_->setNext(rc); }
			bool isNextNull() const { return decorated_->isNextNull(); }
			void setNextNull() { decorated_->setNextNull(); }
			bool isNextWritable() const { return decorated_->isNextWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBStreamingGraphic& rec);


		private:

			edb_adapter::DBStreamingGraphicI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBStreamingGraphic(edb_adapter::DBStreamingGraphicI* d);

			// Do not allow the default assignment operator.
			DBStreamingGraphic operator=(const DBStreamingGraphic& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSTREAMINGGRAPHIC_H
