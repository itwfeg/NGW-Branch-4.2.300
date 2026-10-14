#ifndef SQLITECATEGORY_H
#define SQLITECATEGORY_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBCATEGORYI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteCategory : public DBCategoryI
	{
		public:

			// Types and subtypes
			typedef DBCategoryI interface_type;

			
			// Constructors and destructor
			SqliteCategory(SqliteRecord* r, DBCategoryI* d = 0);
			~SqliteCategory();


			// Think about replacing this method with two friend functions from the
			// SqliteCategorysRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBCategoryI
			virtual DBCategoryI* clone() const { return new SqliteCategory(new SqliteRecord(*rec_)); }
			virtual const DBCategoryI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getNumber() const { return rec_->getField(NUMBER_FIELD); }
			virtual void setNumber(const std::string& number) { rec_->setField(NUMBER_FIELD, number); }
			virtual bool isNumberNull() const { return rec_->isFieldNull(NUMBER_FIELD); }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const { return rec_->getField(DEPTNUM_FIELD); }
			virtual void setDeptNumber(const std::string& number) { rec_->setField(DEPTNUM_FIELD, number); }
			virtual bool isDeptNumberNull() const { return rec_->isFieldNull(DEPTNUM_FIELD); }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual std::string getParentCategory() const { return rec_->getField(PARENTCATEGORY_FIELD); }
			virtual void setParentCategory(const std::string& rc) { rec_->setField(PARENTCATEGORY_FIELD, rc); }
			virtual bool isParentCategoryNull() const { return rec_->isFieldNull(PARENTCATEGORY_FIELD); }
			virtual void setParentCategoryNull() { rec_->setFieldNull(PARENTCATEGORY_FIELD); }
			virtual bool isParentCategoryWritable() const { return true; }

			virtual std::string getName1() const { return rec_->getField(NAME1_FIELD); }
			virtual void setName1(const std::string& desc) { rec_->setField(NAME1_FIELD, desc); }
			virtual bool isName1Null() const { return rec_->isFieldNull(NAME1_FIELD); }
			virtual void setName1Null() { rec_->setFieldNull(NAME1_FIELD); }
			virtual bool isName1Writable() const { return true; }

			virtual std::string getName2() const { return rec_->getField(NAME2_FIELD); }
			virtual void setName2(const std::string& desc) { rec_->setField(NAME2_FIELD, desc); }
			virtual bool isName2Null() const { return rec_->isFieldNull(NAME2_FIELD); }
			virtual void setName2Null() { rec_->setFieldNull(NAME2_FIELD); }
			virtual bool isName2Writable() const { return true; }

			virtual std::string getCodedName1() const { return rec_->getField(CODEDNAME1_FIELD); }
			virtual void setCodedName1(const std::string& name) { rec_->setField(CODEDNAME1_FIELD, name); }
			virtual bool isCodedName1Null() const { return rec_->isFieldNull(CODEDNAME1_FIELD); }
			virtual void setCodedName1Null() { rec_->setFieldNull(CODEDNAME1_FIELD); }
			virtual bool isCodedName1Writable() const { return true; }

			virtual std::string getCodedName2() const { return rec_->getField(CODEDNAME2_FIELD); }
			virtual void setCodedName2(const std::string& name) { rec_->setField(CODEDNAME2_FIELD, name); }
			virtual bool isCodedName2Null() const { return rec_->isFieldNull(CODEDNAME2_FIELD); }
			virtual void setCodedName2Null() { rec_->setFieldNull(CODEDNAME2_FIELD); }
			virtual bool isCodedName2Writable() const { return true; }

			virtual int getBackgroundColor() const { return rec_->getField(BACKGROUNDCOLOR_FIELD); }
			virtual void setBackgroundColor(int vs) { rec_->setField(BACKGROUNDCOLOR_FIELD, vs); }
			virtual bool isBackgroundColorNull() const { return rec_->isFieldNull(BACKGROUNDCOLOR_FIELD); }
			virtual void setBackgroundColorNull() { rec_->setFieldNull(BACKGROUNDCOLOR_FIELD); }
			virtual bool isBackgroundColorWritable() const { return true; }

			virtual int getTextColor() const { return rec_->getField(TEXTCOLOR_FIELD); }
			virtual void setTextColor(int tc) { rec_->setField(TEXTCOLOR_FIELD, tc); }
			virtual bool isTextColorNull() const { return rec_->isFieldNull(TEXTCOLOR_FIELD); }
			virtual void setTextColorNull() { rec_->setFieldNull(TEXTCOLOR_FIELD); }
			virtual bool isTextColorWritable() const { return true; }

			virtual std::string getImage() const { return rec_->getField(IMAGE_FIELD); }
			virtual void setImage(const std::string& desc) { rec_->setField(IMAGE_FIELD, desc); }
			virtual bool isImageNull() const { return rec_->isFieldNull(IMAGE_FIELD); }
			virtual void setImageNull() { rec_->setFieldNull(IMAGE_FIELD); }
			virtual bool isImageWritable() const { return true; }

			virtual std::string getPlaylist() const { return rec_->getField(PLAYLIST_FIELD); }
			virtual void setPlaylist(const std::string& list) { rec_->setField(PLAYLIST_FIELD, list); }
			virtual bool isPlaylistNull() const { return rec_->isFieldNull(PLAYLIST_FIELD); }
			virtual void setPlaylistNull() { rec_->setFieldNull(PLAYLIST_FIELD); }
			virtual bool isPlaylistWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string PARENTCATEGORY_FIELD;
			static const std::string NAME1_FIELD;
			static const std::string NAME2_FIELD;
			static const std::string CODEDNAME1_FIELD;
			static const std::string CODEDNAME2_FIELD;
			static const std::string BACKGROUNDCOLOR_FIELD;
			static const std::string TEXTCOLOR_FIELD;
			static const std::string IMAGE_FIELD;
			static const std::string PLAYLIST_FIELD;


		private:

			SqliteRecord* rec_;
			DBCategoryI* decorated_;

			// Do not allow the default copy constructor.
			SqliteCategory(const SqliteCategory& rec);
			// Do not allow the default assignment operator.
			SqliteCategory operator=(const SqliteCategory& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECATEGORY_H
