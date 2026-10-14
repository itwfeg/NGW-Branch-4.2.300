/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef LABELTYPE_H
#define LABELTYPE_H

#include "evolution.h"
#include "HfcObject.h"
#include "Label.h"
#include "HobartLabel.h"
#include "Printer.h"
#include <list>
#include <vector>

class LabelStockDescriptor;
class FormRecord;

class DBAccess;
namespace edb 
{
class DBLabel;
class DBROMLabel;
}


typedef enum
{
	LabelNoRotation,	//	Label is upright on the label stock, not rotated
	LabelUpsideDown		//	Label is upside down on the label stock, rotated 180 degrees
} LabelRotationOnLabelStock;

typedef enum 
{ 
	LabelOk,			//	Label found with no family search
	LabelCorrected,		//	Found a label in family to fit the label stock
	LabelInvalid,		//	Does not exist
	LabelWontFit		//	Can't find in family a label to fit the label stock
} LabelMediaReturn;


typedef enum LabelTypeFieldOffsets
{
	fieldOffsetLabelTypeNum = 1,
	fieldOffsetLabelTypeIsCustom,
	fieldOffsetLabelTypeBinary,
	fieldOffsetLabelTypeBinaryLength,
	fieldOffsetLabelTypeSequence,
	fieldOffsetLabelTypeSequenceMax,
	fieldOffsetLabelTypeWidth,
	fieldOffsetLabelTypeLength,
	fieldOffsetLabelTypeIncludeEmbedded,

//	fieldOffsetLabelTypeErrorCode,
/*
	fieldOffsetLabelTypeWidth,
	fieldOffsetLabelTypeOrientation,
	fieldOffsetLabelTypeSkew,
	fieldOffsetLabelTypeBarcodeType,
*/

	fieldOffsetLabelTypeDeptNum,
	fieldOffsetLabelTypeRotationOnLabelStock,
	endLabelTypeFieldOffsets
}   LabelTypeFieldOffsets_t;



class LabelType : public HfcObject
{
	public:
		// --- Types and subtypes ---
		typedef std::vector<LabelType*> labelContainer;
		typedef labelContainer::const_iterator labelContainerIterator;

		typedef HobartLabel::FormsListType formContainer;
		typedef formContainer::const_iterator formContainerIterator;


		// --- Constructors and destructor ---
		LabelType(HobRecordNum labelTypeNum);
		LabelType(const char *data, int len, int formatType);
		LabelType(edb::DBLabel *dbRec, DBAccess *access = (DBAccess *)NULL);
		LabelType(edb::DBROMLabel *dbRec, DBAccess *access = (DBAccess *)NULL);

		~LabelType();

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum num);

		HobRecordNum getLabelTypeNum() const;
		void setLabelTypeNum(HobRecordNum num);

		LabelRotationOnLabelStock getRotationOnLabelStock() const;
		void setRotationOnLabelStock(LabelRotationOnLabelStock rotation);

		const HobartLabel* getHobartLabel() const;
		void setHobartLabel(HobartLabel* label);

		void setCustom(bool c) { custom = c; }
		bool isCustom() const {return custom;}
		bool isRom() const {return !isCustom();}

		int getResourceLength() const;
		void setResourceLength(int length);

		short getSequence() const;
		void setSequence(short seq);

		short getSequenceMax() const;
		void setSequenceMax(short seqMax);

		UINT16 getLength() const;
		void setLength(UINT16 length);
		double getLengthInUnits(bool isMetric) const;

		UINT16 getWidthDots() const;
		void setWidthDots(UINT16 widthDots);
		LabelWidth getWidth() const;
		void setWidth(LabelWidth width);
		double getWidthInUnits(bool isMetric) const;

		LabelOrientation getOrientation() const;
		void setOrientation(LabelOrientation orientation);

		LabelStockDescriptor *getLabelStockDescriptor();

		INT16 getSkew() const;
		void setSkew(INT16 labelSkew);

		formContainer* getFormsList();
		INT32 getNumberOfForms();
		void startAtListTop();
		FormRecord *getFormFromList();

		FormRecord *findFieldType(FieldType type);
		short getBarcodeType();
		short determineBarcodeType();
		short determineSpecializedBarcodeType();
		bool containsBarcodeType(UINT8 typeBeingLookedFor);

		const std::string &getOrigBinaryData() const;
		void setOrigBinaryData(const std::string &binary);

		LabelMediaReturn getLabelMediaError() const;
		bool isLabelMediaError() const;

		void setLabelMediaError(LabelMediaReturn errorNum);

		const LabelType *getEmbeddedNutritionLabelType() const;
		bool labelHasNutrifacts();
		bool labelIsEvolutionNutrifacts();
		bool labelHasFrequentShopper();
		bool labelHasDiscountPriceInBarcode();
		bool labelHasKbdNetWeight();
		bool labelHasPortions();

		bool doIncludeEmbeddedLabels() const;
		void setIncludeEmbeddedLabels(bool doInclude);

		// This method looks for a specific label form ('type') and returns true
		// if it finds it. In that case, it also returns its width and height
		// in the 'width' and 'height' parameters.
		bool getLabelFormSize(FieldType type, int& width, int& height);
		bool getLabelFormSizeAndFont(FieldType type, int& width, int& height, std::string& fontName, int& pointSize);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);

		void setupLabel(const std::string &binary);

		// --- Getters and setters ---
		bool isOldFormat() const;
		bool isNewFormat() const;

		void generateEmbeddedLabels();
		const labelContainer& getEmbeddedLabels();

		static bool isLabelFormAccumulateItemsItemLabelType(UINT16 verticalJustifyNum);

		bool updateField(int fieldOffset, const HobString &newValue);
		void updateRecord(const LabelType *newData);

		bool isFieldQuoted(int fieldOffset) const;
		void getFieldAsString(int fieldOffset, HobString &result, int format) const;
		virtual const char *serialize(int format) const;
		const std::string serializeLabelToView() const;

		DBAccess *getDBAccess() const;
		void setDBAccess(DBAccess *dbAccess);
		bool isRecordValid() const;
		int deleteRecord();
		void saveRecord();

		static LabelType *findRecord(HobRecordNum labelTypeNum, HobRecordNum deptNum, DBAccess *dbAccess);
		static LabelType *findCustomRecord(HobRecordNum labelTypeNum, HobRecordNum deptNum, DBAccess *dbAccess);
		static LabelType *findROMRecord(HobRecordNum labelTypeNum, DBAccess *dbAccess);

		static void initFieldLists();

		void show() const;


	private:

		HobRecordNum	deptNum;
		HobRecordNum	labelTypeNum;
		LabelRotationOnLabelStock	rotationOnLabelStock;
		HobErrorCode	errorCode;
		HobartLabel		*label;
		short			barcodeType;
		bool			custom;
		bool			includeEmbeddedLabels;
		LabelMediaReturn	labelMediaError;	//	Will this label fit on the label stock?

		// Vars to support the Ultima format of labels
		mutable int resourceLength;
		short sequence;
		short sequenceMax;

		LabelType *embeddedNutritionLabelType;
		labelContainer embeddedLabels_;

		DBAccess* dbAccess;
		edb::DBLabel* myDbRecord;
		edb::DBROMLabel* myROMDbRecord;

		std::string origBinaryData;
		
		static std::list<LabelTypeFieldOffsets> uiFieldList, evolFieldList, dbindependentFieldList, viewLabelFieldList;

		void init();
		void cleanupEmbeddedLabelsContainer();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //LABELTYPE_H
