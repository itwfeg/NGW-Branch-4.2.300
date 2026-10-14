/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTTEXT_H
#define PRODUCTTEXT_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

#include "DBProductText.h"
class DBAccess;
namespace edb
{
	class DBProductText;
}


typedef enum ProdSupportTextType
{
	psttUnknown,

	psttExpandedText,
	psttSpecialMessage,
	psttProductNote,
	psttMarqueeText,
	psttProductText5,  // CookingInstructionsDev
	psttProductText6,  // RecipeDev
	psttProductText7,  // ExpandedText2Dev
	psttProductText8,  // SuggestiveSellingDev
	psttProductText9,  // AllergensDev

	psttLastProductText,

	psttNutrifacts = 20,
	endProdSupportTextType,

	// The following are used when determining the size of the fields in the label for the current product.
	// References to them are in operateMenuScripts.asp, when sending the 101,33 (ProductDev, ProductReadLabelFormSize) message.
	psttProductDescription = 30,  //This was created to determine the size of the editor to edit the product description; it is not truly a product support record type
	psttCoolTrackingText,         //This was created to determine the size of the editor to edit the cool tracking text; it is not truly a product support record type
	endProdSupportTextTypeForEditors

}   ProdSupportTextType_t;


typedef enum TextFieldOffsets
{
	fieldOffsetTextNum = 1,
	fieldOffsetTextText,
	fieldOffsetTextErrorCode,
	fieldOffsetTextDeptNum,
	endTextFieldOffsets
}   TextFieldOffsets_t;

class ProductText : public HfcObject
{
	public:

		ProductText(ProdSupportTextType textType);
		ProductText(const char *data, int len, int formatType);
		ProductText(edb::DBProductText *dbRec, DBAccess *access = (DBAccess *)NULL);

		virtual ~ProductText();


		ProdSupportTextType getTextType() const;
		void setTextType(ProdSupportTextType type);

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum num);

		HobRecordNum getTextNum() const;
		void setTextNum(HobRecordNum num);

		HobString getText() const;
		void setText(HobString txt);

		bool isUsedByMany() const;
		void setUsedByMany(bool usedByMany);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode code);

		void testIfUsedByMany();


		bool updateField(int fieldOffset, const HobString &newValue);
		virtual void updateRecord(const ProductText *newData);

		bool isFieldQuoted(int fieldOffset) const;
		void getFieldAsString(int fieldOffset, HobString &result, int format) const;
		virtual const char *serialize(int format) const;

		edb::DBProductText *getMyDbRecord() const;
		void setDBAccess(DBAccess *dbAccess);
		bool isRecordValid() const;
		int deleteRecord();
		void saveRecord();

		static ProductText *findRecord(ProdSupportTextType type, HobRecordNum textNum, HobRecordNum deptNum, DBAccess *dbAccess);
		
		static void initFieldLists();


	protected:

		ProdSupportTextType textType_;

		DBAccess *dbAccess_;

		edb::DBProductText *myDbRecord_;


	private:

		HobRecordNum	textNum_;
		HobRecordNum	deptNum_;
		HobString 		text_;
		bool			usedByMany_;
		HobErrorCode	errorCode_;

		static std::list<TextFieldOffsets> uiFieldList, evolFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //PRODUCTTEXT_H


