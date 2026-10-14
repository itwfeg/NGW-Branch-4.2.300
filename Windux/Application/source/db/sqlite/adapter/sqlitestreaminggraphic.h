#ifndef SQLITESTREAMINGGRAPHIC_H
#define SQLITESTREAMINGGRAPHIC_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBSTREAMINGGRAPHICI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteStreamingGraphic : public DBStreamingGraphicI
	{
		public:

			// Types and subtypes
			typedef DBStreamingGraphicI interface_type;

			
			// Constructors and destructor
			SqliteStreamingGraphic(SqliteRecord* r, DBStreamingGraphicI* d = 0);
			~SqliteStreamingGraphic();


			// Think about replacing this method with two friend functions from the
			// SqliteStreamingGraphicsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBStreamingGraphicI
			virtual DBStreamingGraphicI* clone() const { return new SqliteStreamingGraphic(new SqliteRecord(*rec_)); }
			virtual const DBStreamingGraphicI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getNumber() const { return rec_->getField(NUMBER_FIELD); }
			virtual void setNumber(const std::string& number) { rec_->setField(NUMBER_FIELD, number); }
			virtual bool isNumberNull() const { return rec_->isFieldNull(NUMBER_FIELD); }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getNext() const { return rec_->getField(NEXT_FIELD); }
			virtual void setNext(const std::string& sg) { rec_->setField(NEXT_FIELD, sg); }
			virtual bool isNextNull() const { return rec_->isFieldNull(NEXT_FIELD); }
			virtual void setNextNull() { rec_->setFieldNull(NEXT_FIELD); }
			virtual bool isNextWritable() const { return true; }

			virtual int getTime() const { return rec_->getField(TIME_FIELD); }
			virtual void setTime(const int time) { rec_->setField(TIME_FIELD, time); }
			virtual bool isTimeNull() const { return rec_->isFieldNull(TIME_FIELD); }
			virtual void setTimeNull() { rec_->setFieldNull(TIME_FIELD); }
			virtual bool isTimeWritable() const { return true; }

			virtual int getActionType() const { return rec_->getField(ACTIONTYPE_FIELD); }
			virtual void setActionType(const int type) { rec_->setField(ACTIONTYPE_FIELD, type); }
			virtual bool isActionTypeNull() const { return rec_->isFieldNull(ACTIONTYPE_FIELD); }
			virtual void setActionTypeNull() { rec_->setFieldNull(ACTIONTYPE_FIELD); }
			virtual bool isActionTypeWritable() const { return true; }

			virtual std::string getActionParameter() const { return rec_->getField(ACTIONPARAMETER_FIELD); }
			virtual void setActionParameter(const std::string& par) { rec_->setField(ACTIONPARAMETER_FIELD, par); }
			virtual bool isActionParameterNull() const { return rec_->isFieldNull(ACTIONPARAMETER_FIELD); }
			virtual void setActionParameterNull() { rec_->setFieldNull(ACTIONPARAMETER_FIELD); }
			virtual bool isActionParameterWritable() const { return true; }

			virtual std::string getImage() const { return rec_->getField(IMAGE_FIELD); }
			virtual void setImage(const std::string& image) { rec_->setField(IMAGE_FIELD, image); }
			virtual bool isImageNull() const { return rec_->isFieldNull(IMAGE_FIELD); }
			virtual void setImageNull() { rec_->setFieldNull(IMAGE_FIELD); }
			virtual bool isImageWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string NEXT_FIELD;
			static const std::string TIME_FIELD;
			static const std::string ACTIONTYPE_FIELD;
			static const std::string ACTIONPARAMETER_FIELD;
			static const std::string IMAGE_FIELD;


		private:

			SqliteRecord* rec_;
			DBStreamingGraphicI* decorated_;

			// Do not allow the default copy constructor.
			SqliteStreamingGraphic(const SqliteStreamingGraphic& rec);
			// Do not allow the default assignment operator.
			SqliteStreamingGraphic operator=(const SqliteStreamingGraphic& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESTREAMINGGRAPHIC_H
