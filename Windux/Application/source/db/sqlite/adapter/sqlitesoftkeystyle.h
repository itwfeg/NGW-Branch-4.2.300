#ifndef SQLITESOFTKEYSTYLE_H
#define SQLITESOFTKEYSTYLE_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBSOFTKEYSTYLEI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteSoftKeyStyle : public DBSoftKeyStyleI
	{
		public:

			// Types and subtypes
			typedef DBSoftKeyStyleI interface_type;


			// Constructors and destructor
			SqliteSoftKeyStyle(SqliteRecord* r, DBSoftKeyStyleI* d = 0);
			~SqliteSoftKeyStyle();


			// Think about replacing this method with two friend functions from the
			// SqliteSoftKeyStylesRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBSoftKeyStyleI
			virtual DBSoftKeyStyleI* clone() const { return new SqliteSoftKeyStyle(new SqliteRecord(*rec_)); }
			virtual const DBSoftKeyStyleI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getNumber() const { return rec_->getField(NUMBER_FIELD); }
			virtual void setNumber(const std::string& gn) { rec_->setField(NUMBER_FIELD, gn); }
			virtual bool isNumberNull() const { return rec_->isFieldNull(NUMBER_FIELD); }
			virtual bool isNumberWritable() const { return true; }

			virtual int getDefaultTextLocation() const { return rec_->getField(DEFAULTTEXTLOCATION_FIELD); }
			virtual void setDefaultTextLocation(int dtl) { rec_->setField(DEFAULTTEXTLOCATION_FIELD, dtl); }
			virtual bool isDefaultTextLocationNull() const { return rec_->isFieldNull(DEFAULTTEXTLOCATION_FIELD); }
			virtual void setDefaultTextLocationNull() { rec_->setFieldNull(DEFAULTTEXTLOCATION_FIELD); }
			virtual bool isDefaultTextLocationWritable() const { return true; }

			virtual int getDefaultTextColor() const { return rec_->getField(DEFAULTTEXTCOLOR_FIELD); }
			virtual void setDefaultTextColor(int dtl) { rec_->setField(DEFAULTTEXTCOLOR_FIELD, dtl); }
			virtual bool isDefaultTextColorNull() const { return rec_->isFieldNull(DEFAULTTEXTCOLOR_FIELD); }
			virtual void setDefaultTextColorNull() { rec_->setFieldNull(DEFAULTTEXTCOLOR_FIELD); }
			virtual bool isDefaultTextColorWritable() const { return true; }

			virtual int getDefaultGrayedOutTextColor() const { return rec_->getField(DEFAULTGRAYEDOUTTEXTCOLOR_FIELD); }
			virtual void setDefaultGrayedOutTextColor(int dtl) { rec_->setField(DEFAULTGRAYEDOUTTEXTCOLOR_FIELD, dtl); }
			virtual bool isDefaultGrayedOutTextColorNull() const { return rec_->isFieldNull(DEFAULTGRAYEDOUTTEXTCOLOR_FIELD); }
			virtual void setDefaultGrayedOutTextColorNull() { rec_->setFieldNull(DEFAULTGRAYEDOUTTEXTCOLOR_FIELD); }
			virtual bool isDefaultGrayedOutTextColorWritable() const { return true; }

			virtual int getDefaultForegroundImageLocation() const { return rec_->getField(DEFAULTFOREGROUNDIMAGELOCATION_FIELD); }
			virtual void setDefaultForegroundImageLocation(int dfil) { rec_->setField(DEFAULTFOREGROUNDIMAGELOCATION_FIELD, dfil); }
			virtual bool isDefaultForegroundImageLocationNull() const { return rec_->isFieldNull(DEFAULTFOREGROUNDIMAGELOCATION_FIELD); }
			virtual void setDefaultForegroundImageLocationNull() { rec_->setFieldNull(DEFAULTFOREGROUNDIMAGELOCATION_FIELD); }
			virtual bool isDefaultForegroundImageLocationWritable() const { return true; }

			virtual int getDefaultBackgroundColor() const { return rec_->getField(DEFAULTBACKGROUNDCOLOR_FIELD); }
			virtual void setDefaultBackgroundColor(int dbc) { rec_->setField(DEFAULTBACKGROUNDCOLOR_FIELD, dbc); }
			virtual bool isDefaultBackgroundColorNull() const { return rec_->isFieldNull(DEFAULTBACKGROUNDCOLOR_FIELD); }
			virtual void setDefaultBackgroundColorNull() { rec_->setFieldNull(DEFAULTBACKGROUNDCOLOR_FIELD); }
			virtual bool isDefaultBackgroundColorWritable() const { return true; }

			virtual std::string getDefaultBackgroundImage() const { return rec_->getField(DEFAULTBACKGROUNDIMAGE_FIELD); }
			virtual void setDefaultBackgroundImage(const std::string& dbi) { rec_->setField(DEFAULTBACKGROUNDIMAGE_FIELD, dbi); }
			virtual bool isDefaultBackgroundImageNull() const { return rec_->isFieldNull(DEFAULTBACKGROUNDIMAGE_FIELD); }
			virtual void setDefaultBackgroundImageNull() { rec_->setFieldNull(DEFAULTBACKGROUNDIMAGE_FIELD); }
			virtual bool isDefaultBackgroundImageWritable() const { return true; }

			virtual std::string getImagesDirectory() const { return rec_->getField(IMAGESDIRECTORY_FIELD); }
			virtual void setImagesDirectory(const std::string& id) { rec_->setField(IMAGESDIRECTORY_FIELD, id); }
			virtual bool isImagesDirectoryNull() const { return rec_->isFieldNull(IMAGESDIRECTORY_FIELD); }
			virtual void setImagesDirectoryNull() { rec_->setFieldNull(IMAGESDIRECTORY_FIELD); }
			virtual bool isImagesDirectoryWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string DEFAULTTEXTLOCATION_FIELD;
			static const std::string DEFAULTTEXTCOLOR_FIELD;
			static const std::string DEFAULTGRAYEDOUTTEXTCOLOR_FIELD;
			static const std::string DEFAULTFOREGROUNDIMAGELOCATION_FIELD;
			static const std::string DEFAULTBACKGROUNDCOLOR_FIELD;
			static const std::string DEFAULTBACKGROUNDIMAGE_FIELD;
			static const std::string IMAGESDIRECTORY_FIELD;


		private:

			SqliteRecord* rec_;
			DBSoftKeyStyleI* decorated_;

			// Do not allow the default copy constructor.
			SqliteSoftKeyStyle(const SqliteSoftKeyStyle& rec);
			// Do not allow the default assignment operator.
			SqliteSoftKeyStyle operator=(const SqliteSoftKeyStyle& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESOFTKEYSTYLE_H
