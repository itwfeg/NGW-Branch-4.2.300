/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "LabelType.h"
#include "LabelTypeManager.h"
#include "FormRecord.h"
#include "LabelTypeTags.h"
#include "Configuration.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "HobToken.h"

#include "Blob.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBLabel.h"
#include "DBLabelsRM.h"
#include "DBROMLabel.h"
#include "DBROMLabelsRM.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelType::dataType = 0;
std::string LabelType::dataTypeName("LabelType"); 
#endif

list<LabelTypeFieldOffsets> LabelType::uiFieldList;
list<LabelTypeFieldOffsets> LabelType::evolFieldList;
list<LabelTypeFieldOffsets> LabelType::dbindependentFieldList;
list<LabelTypeFieldOffsets> LabelType::viewLabelFieldList;


LabelType::LabelType(HobRecordNum num)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	labelTypeNum = num;

	Debug::trace("LabelType.ctor", "    LabelType: " + ptrtostring(this) + "  num " + longtostring(labelTypeNum) + "  deptNum " + longtostring(deptNum));
}

LabelType::LabelType(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	int offset = 0;

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			std::list<LabelTypeFieldOffsets>::iterator fieldIter = uiFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != uiFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, (token.length() == 0) ? false : true);
				if (token.length())
				{
					token.unQuoteForCSV();
					updateField(offset, token);
				}
				fieldIter++;
			}

			// Only for the UI, if we didn't receive the deptNum with the data, then set the deptNum to the currently active dept.
			// If we don't receive it via comm/merge/restore, or other mechanism in the subsequent cases, then it remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetLabelTypeDeptNum))
			{
				setReceived(fieldOffsetLabelTypeDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			LabelTypeTags labelTypeTags;			

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			int offset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					//get the tag and find out what offset goes with the tag
					temp = token->getTag();
					offset = (temp.size()) ? labelTypeTags.getOffset(temp) : 0;
					if (Debug::isTracing("LabelType.ctor"))
						Debug::trace("LabelType.ctor", string("LabelType(data,len,format) token: ") + temp + string("  offset:") + inttostring(offset) + string("  data:") + token->getData());
					if (offset)
					{
						setReceived(offset, true);
						updateField(offset, token->getData());
					}
				}
				delete token;
			}

			// For comm/backup/restore, if we didn't receive the deptNum with the data and the config parameter is set, set the deptNum to the currently active dept.
			// If we didn't receive the deptNum with the data and the config parameter is NOT set (normal case), deptNum remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetLabelTypeDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetLabelTypeDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}

	Debug::trace("LabelType.ctor", "    LabelType: " + ptrtostring(this) + "  num " + longtostring(labelTypeNum) + "  deptNum " + longtostring(deptNum));
}

LabelType::LabelType(DBLabel *dbRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool isTracing = Debug::isTracing("LabelType.ctor");
	if (isTracing)
		Debug::trace("LabelType.ctor", string("LabelType(DBLabel, DBAccess) - dbRecPtr passed in ") + ptrtostring(dbRec));
	init();

	dbAccess = access;

	if (dbRec)
	{
		myDbRecord = dbRec;

		deptNum = Long::valueOf(dbRec->getDeptNumber());
		labelTypeNum = Long::valueOf(dbRec->getNumber());
		rotationOnLabelStock = (LabelRotationOnLabelStock) Integer::valueOf(dbRec->getRotation());
		string labelBinary = dbRec->getBinary();
		Blob *labelBlob = new Blob(labelBinary);

		util_time::Time t0 = util_time::Time();

		// In Windux and before, we store the un-nibblized (binary) data for the label type in the database.
		// For Fastrack, we are storing the nibblized data in the database,
		// so we need to unnibblize it to get the binary representation of the label type.
		labelBlob->unNibblizeBlob();

		util_time::Time t1 = util_time::Time();
		if (isTracing)
			Debug::trace("LabelType.ctor", 28, string("time to unnibblize label data = ") + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

		label = new HobartLabel(labelBlob);

		if (label->isFormatChanged())
		{
			if (isTracing)
			{
				Debug::trace("LabelType.ctor", string("  after convert, before save - num ") + longtostring(labelTypeNum) + "  deptNum " + longtostring(deptNum));
				show();
			}
			saveRecord();
		}
		else if (isTracing)
			Debug::trace("LabelType.ctor", 20, "No label conversion was necessary.");

		if (labelBlob)
			delete labelBlob;

		errorCode = RECORD_OK;
	}

	Debug::trace("LabelType.ctor", "    LabelType: " + ptrtostring(this) + "  num " + longtostring(labelTypeNum) + "  deptNum " + longtostring(deptNum));
}


LabelType::LabelType(DBROMLabel *dbRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool isTracing = Debug::isTracing("LabelType.ctor");
	if (isTracing)
		Debug::trace("LabelType.ctor", string("LabelType(DBROMLabel, DBAccess) - dbRecPtr passed in ") + ptrtostring(dbRec));
	init();

	dbAccess = access;

	if (dbRec)
	{
		myROMDbRecord = dbRec;

		custom = false;
		// ROM labels aren't 'by dept', so just assign the deptNum to be 0; DON'T MAKE IT RECORD_NOT_FOUND - that was the cause of the bug SOF-1824
		deptNum = 0;
		labelTypeNum = Integer::valueOf(dbRec->getNumber());
		rotationOnLabelStock = (LabelRotationOnLabelStock) Integer::valueOf(dbRec->getRotation());
		string labelBinary = dbRec->getBinary();
		Blob *labelBlob = new Blob(labelBinary);

		util_time::Time t0 = util_time::Time();

		// In Windux and before, we store the un-nibblized (binary) data for the label type in the database.
		// For Fastrack, we are storing the nibblized data in the database,
		// so we need to unnibblize it to get the binary representation of the label type.
		labelBlob->unNibblizeBlob();

		util_time::Time t1 = util_time::Time();
		if (isTracing)
			Debug::trace("LabelType.ctor", 28, string("time to unnibblize label data = ") + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

		label = new HobartLabel(labelBlob);

		if (label->isFormatChanged())
		{
			if (isTracing)
			{
				Debug::trace("LabelType.ctor", string("  after convert, before save - num ") + longtostring(labelTypeNum) + "  deptNum " + longtostring(deptNum));
				show();
			}
			saveRecord();
		}

		if (labelBlob)
			delete labelBlob;

		errorCode = RECORD_OK;
	}

	Debug::trace("LabelType.ctor", "    LabelType: " + ptrtostring(this) + "  num " + longtostring(labelTypeNum) + "  deptNum " + longtostring(deptNum));
}


LabelType::~LabelType()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("LabelType.dtor", "    LabelType: " + ptrtostring(this) + "  num " + longtostring(labelTypeNum) + "  deptNum " + longtostring(deptNum));

	if (label)
		delete label;

	if (embeddedNutritionLabelType && embeddedNutritionLabelType != this)
		delete embeddedNutritionLabelType;

	if (myDbRecord)
		delete myDbRecord;

	if (myROMDbRecord)
		delete myROMDbRecord;

	// Dispose of the embedded labels (the container will go away automatically)
	cleanupEmbeddedLabelsContainer();
}


void LabelType::init()
{
	deptNum = 0;
	labelTypeNum = RECORD_NOT_FOUND;
	rotationOnLabelStock = LabelNoRotation;
	resourceLength = 0;
	sequence = 1;
	sequenceMax = 1;
	custom = true;
	errorCode = RECORD_NOT_FOUND;
	label = (HobartLabel *) NULL;
	barcodeType = RECORD_NOT_FOUND;
	labelMediaError = LabelInvalid;
	includeEmbeddedLabels = false;

	embeddedNutritionLabelType = (LabelType *)NULL;

	dbAccess = (DBAccess *)NULL;
	myDbRecord = 0;
	myROMDbRecord = 0;

	initFieldLists();
}

// static
void LabelType::initFieldLists()
{
	if (uiFieldList.empty())
	{
		uiFieldList.push_back(fieldOffsetLabelTypeNum);
		uiFieldList.push_back(fieldOffsetLabelTypeIncludeEmbedded);
		uiFieldList.push_back(fieldOffsetLabelTypeRotationOnLabelStock);
	}

	if (viewLabelFieldList.empty())
	{
		viewLabelFieldList.push_back(fieldOffsetLabelTypeNum);
		viewLabelFieldList.push_back(fieldOffsetLabelTypeWidth);
		viewLabelFieldList.push_back(fieldOffsetLabelTypeLength);
		viewLabelFieldList.push_back(fieldOffsetLabelTypeIsCustom);
		viewLabelFieldList.push_back(fieldOffsetLabelTypeRotationOnLabelStock);
		viewLabelFieldList.push_back(fieldOffsetLabelTypeBinary);

		LabelTypeTags labelTypeTags((FieldOffsetsList *)&viewLabelFieldList);
	}

	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetLabelTypeDeptNum);
		evolFieldList.push_back(fieldOffsetLabelTypeNum);
		evolFieldList.push_back(fieldOffsetLabelTypeRotationOnLabelStock);
		evolFieldList.push_back(fieldOffsetLabelTypeBinary);
		evolFieldList.push_back(fieldOffsetLabelTypeBinaryLength);
		evolFieldList.push_back(fieldOffsetLabelTypeSequence);
		evolFieldList.push_back(fieldOffsetLabelTypeSequenceMax);

		LabelTypeTags labelTypeTags((FieldOffsetsList *)&evolFieldList);
	}

	if (dbindependentFieldList.empty())
	{
		dbindependentFieldList.push_back(fieldOffsetLabelTypeDeptNum);
		dbindependentFieldList.push_back(fieldOffsetLabelTypeNum);
		dbindependentFieldList.push_back(fieldOffsetLabelTypeRotationOnLabelStock);
		dbindependentFieldList.push_back(fieldOffsetLabelTypeBinary);

		LabelTypeTags labelTypeTags((FieldOffsetsList *)&dbindependentFieldList);
	}
}

void LabelType::setupLabel(const std::string &binary)
{
		Blob *labelBlob = new Blob(binary);
		if (labelBlob)
		{
			label = new HobartLabel(labelBlob);
			delete labelBlob;
			errorCode = RECORD_OK;
		}
}

bool LabelType::isOldFormat() const
{
	if ( label )
	{
		return label->isFormatChanged();
	}
	else
	{
		if ( origBinaryData.size() )
		{
			Blob labelBlob(origBinaryData);

			util_time::Time t0 = util_time::Time();

			// In Windux and before, we store the un-nibblized (binary) data for the label type in the database.
			// For Fastrack, we are storing the nibblized data in the database,
			// so we need to unnibblize it to get the binary representation of the label type.
			labelBlob.unNibblizeBlob();

			util_time::Time t1 = util_time::Time();
			Debug::trace("LabelType", 28, "time to unnibblize label data = " + util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

			HobartLabel hLabel(&labelBlob);
			return hLabel.isFormatChanged();
		}
		else
			return true;
	}
}

bool LabelType::isNewFormat() const
{
	return !isOldFormat();
}


HobRecordNum LabelType::getDeptNum() const
{
	return deptNum;
}
void LabelType::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

HobRecordNum LabelType::getLabelTypeNum() const
{
	return labelTypeNum;
}
void LabelType::setLabelTypeNum(HobRecordNum num)
{
	labelTypeNum = num;
}

LabelRotationOnLabelStock LabelType::getRotationOnLabelStock() const
{
	return rotationOnLabelStock;
}
void LabelType::setRotationOnLabelStock(LabelRotationOnLabelStock rotation)
{
	rotationOnLabelStock = rotation;
}


int LabelType::getResourceLength() const
{
	return resourceLength;
}
void LabelType::setResourceLength(int length)
{
	resourceLength = length;
}

short LabelType::getSequence() const
{
	return sequence;
}
void LabelType::setSequence(short seq)
{
	sequence = seq;
}

short LabelType::getSequenceMax() const
{
	return sequenceMax;
}
void LabelType::setSequenceMax(short seqMax)
{
	sequenceMax = seqMax;
}

const string &LabelType::getOrigBinaryData() const
{
	return origBinaryData;
}
void LabelType::setOrigBinaryData(const string &binary)
{
	origBinaryData = binary;
}

HobErrorCode LabelType::getErrorCode() const
{
	return errorCode;
}
void LabelType::setErrorCode(HobErrorCode code)
{
	this->errorCode = code;
}


const HobartLabel* LabelType::getHobartLabel() const
{
	return label;
}

void LabelType::setHobartLabel(HobartLabel* l)
{
	if ( label )
	{
		delete label;
		label = 0;
	}

	if ( l )
	{
		label = l;

		errorCode = RECORD_OK;
	}
	else
		errorCode = RECORD_NOT_FOUND;
}


UINT16 LabelType::getLength() const
{
	return (label && label->getLabelStockDescriptor()) ? label->getLabelStockDescriptor()->getLength() : 0;
}
void LabelType::setLength(UINT16 length)
{
	if (label && label->getLabelStockDescriptor())
		label->getLabelStockDescriptor()->setLength(length);
}

double LabelType::getLengthInUnits(bool isMetric) const
{
	return (label && label->getLabelStockDescriptor()) ? label->getLabelStockDescriptor()->getLengthInUnits(isMetric) : 0;
}


UINT16 LabelType::getWidthDots() const
{
	return (label && label->getLabelStockDescriptor()) ? label->getLabelStockDescriptor()->getWidthDots() : 0;
}
void LabelType::setWidthDots(UINT16 widthDots)
{
	if (label && label->getLabelStockDescriptor())
		label->getLabelStockDescriptor()->setWidthDots(widthDots);
}

LabelWidth LabelType::getWidth() const
{
	return (label && label->getLabelStockDescriptor()) ? label->getLabelStockDescriptor()->getWidth() : UnknownLabel;
}
void LabelType::setWidth(LabelWidth width)
{
	if (label && label->getLabelStockDescriptor())
		label->getLabelStockDescriptor()->setWidth(width);
}

double LabelType::getWidthInUnits(bool isMetric) const
{
	return (label && label->getLabelStockDescriptor()) ? label->getLabelStockDescriptor()->getWidthInUnits(isMetric) : 0;
}


LabelOrientation LabelType::getOrientation() const
{
	return (label && label->getLabelStockDescriptor()) ? label->getLabelStockDescriptor()->getLabelOrientation() : PortraitLabel;
}
void LabelType::setOrientation(LabelOrientation labelOrientation)
{
	if (label && label->getLabelStockDescriptor())
		label->getLabelStockDescriptor()->setLabelOrientation(labelOrientation);
}

LabelStockDescriptor *LabelType::getLabelStockDescriptor()
{
	return (label) ? label->getLabelStockDescriptor() : 0;
}

INT16 LabelType::getSkew() const
{
	return (label && label->getLabelStockDescriptor()) ? label->getLabelStockDescriptor()->getSkew() : 0;
}
void LabelType::setSkew(INT16 skew)
{
	if (label && label->getLabelStockDescriptor())
		label->getLabelStockDescriptor()->setSkew(skew);
}


LabelType::formContainer *LabelType::getFormsList()
{
	return (label) ? label->getFormsList() : 0;
}

FormRecord *LabelType::getFormFromList()
{
	return (label) ? label->getFormFromList() : 0;
}

void LabelType::startAtListTop()
{
	if (label)
		label->startAtListTop();
}

INT32 LabelType::getNumberOfForms()
{
	return (label) ? label->getNumberOfForms() : 0;
}


FormRecord *LabelType::findFieldType(FieldType type)
{
	FormRecord *form = (FormRecord *) NULL;
	if ( label )
	{
		formContainer* formsList = label->getFormsList();
		if ( formsList )
		{
			formContainerIterator iter = formsList->begin();
			while ( iter != formsList->end() )
			{
				if ( (*iter)->getFieldType() == type )
				{
					form = *iter;
					break;
				}

				++iter;
			}
		}
	}

	return form;
}

/* Changed for the functionality of the original Accumumlate items
short LabelType::getBarcodeType()
{
	if(barcodeType == RECORD_NOT_FOUND)
	{
		FormRecord *formRecord = findFieldType(BARCODE);
		if(formRecord)
			barcodeType = formRecord->getFieldSubType();
	}
	return barcodeType;
}
*/

short LabelType::getBarcodeType()
{
	if (barcodeType == RECORD_NOT_FOUND)
		barcodeType = determineBarcodeType();
	return barcodeType;
}

short LabelType::determineBarcodeType()
{
	short bcType = RECORD_NOT_FOUND;

	FormRecord *formRecord = findFieldType(BARCODE);
	if (formRecord)
		bcType = formRecord->getFieldSubType();
	else
	{
		// This label type doesn't contain a barcode, but does any embedded label?  Check it out...
		bool result = false;
		LabelType *embeddedLabelType = (LabelType *)NULL;
		if ( label )
		{
			formContainer* formsList = label->getFormsList();
			if ( formsList )
			{
				formContainerIterator iter = formsList->begin();
				while ( iter != formsList->end() && result == false )
				{
					if ( (*iter)->getFieldType() == INDIRECT_LABEL_REFERENCE )
					{
						formRecord = *iter;
						if (dbAccess)
						{
							HobRecordNum embeddedLabelTypeNumber = formRecord->getVerticalJustify();	// Yes, this field is also used to indicate the embedded label type number.
							embeddedLabelType = LabelType::findRecord(embeddedLabelTypeNumber, getDeptNum(), dbAccess);
							if ( embeddedLabelType )
							{
								bcType = embeddedLabelType->determineBarcodeType();
								delete embeddedLabelType;
								embeddedLabelType = (LabelType *)NULL;
								if ( bcType != RECORD_NOT_FOUND )
									break;
							}
						}
					}

					++iter;
				}
			}
		}
	}

	return bcType;
}

short LabelType::determineSpecializedBarcodeType()
{
	short bcType = RECORD_NOT_FOUND;
	FormRecord *formRecord = 0;

	formContainer* formsList = label->getFormsList();
	if (formsList)
	{
		formContainerIterator iter = formsList->begin();
		while (iter != formsList->end() && bcType == RECORD_NOT_FOUND)
		{
			if ((*iter)->getFieldType() == BARCODE)
			{
				UINT8 subType = (*iter)->getFieldSubType();
				if (subType >= UPCA && subType < DATAMATRIX)
					bcType = subType;
			}

			++iter;
		}
	}

	if (bcType == RECORD_NOT_FOUND)
	{
		// This label type doesn't contain a barcode, but does any embedded label?  Check it out...
		bool result = false;
		LabelType *embeddedLabelType = (LabelType *)NULL;
		if (label)
		{
			formContainer* formsList = label->getFormsList();
			if (formsList)
			{
				formContainerIterator iter = formsList->begin();
				while (iter != formsList->end() && result == false)
				{
					if ((*iter)->getFieldType() == INDIRECT_LABEL_REFERENCE)
					{
						formRecord = *iter;
						if (dbAccess)
						{
							HobRecordNum embeddedLabelTypeNumber = formRecord->getVerticalJustify();	// Yes, this field is also used to indicate the embedded label type number.
							embeddedLabelType = LabelType::findRecord(embeddedLabelTypeNumber, getDeptNum(), dbAccess);
							if (embeddedLabelType)
							{
								bcType = embeddedLabelType->determineSpecializedBarcodeType();
								delete embeddedLabelType;
								embeddedLabelType = (LabelType *)NULL;
								if (bcType != RECORD_NOT_FOUND)
									break;
							}
						}
					}

					++iter;
				}
			}
		}
	}

	return bcType;
}


bool LabelType::containsBarcodeType(UINT8 typeBeingLookedFor)
{
	FormRecord *formRecord = 0;

	if (!label)
		return false;			// if we dont have a label lets bail. Otherwise we crash
	
	formContainer* formsList = label->getFormsList();
	if (formsList)
	{
		formContainerIterator iter = formsList->begin();
		while (iter != formsList->end())
		{
			if ((*iter)->getFieldType() == BARCODE)
			{
				UINT8 subType = (*iter)->getFieldSubType();
				if (subType == typeBeingLookedFor)
					return true;
			}

			++iter;
		}
	}

	// This label type doesn't contain a barcode, but does any embedded label?  Check it out...
	LabelType *embeddedLabelType = (LabelType *)NULL;
	if (label)
	{
		formContainer* formsList = label->getFormsList();
		if (formsList)
		{
			formContainerIterator iter = formsList->begin();
			while (iter != formsList->end())
			{
				if ((*iter)->getFieldType() == INDIRECT_LABEL_REFERENCE)
				{
					formRecord = *iter;
					if (dbAccess)
					{
						HobRecordNum embeddedLabelTypeNumber = formRecord->getVerticalJustify();	// Yes, this field is also used to indicate the embedded label type number.
						embeddedLabelType = LabelType::findRecord(embeddedLabelTypeNumber, getDeptNum(), dbAccess);
						if (embeddedLabelType)
						{
							bool found = embeddedLabelType->containsBarcodeType(typeBeingLookedFor);
							delete embeddedLabelType;
							embeddedLabelType = (LabelType *)NULL;
							if (found)
								return true;
						}
					}
				}

				++iter;
			}
		}
	}

	return false;
}


LabelMediaReturn LabelType::getLabelMediaError() const
{
	Debug::trace("LabelType.LabelMediaError", HobString("getLabelMediaError(): ") + HobString(Integer::toString(labelMediaError)));
	Debug::trace("LabelType.This", "this: " + ptrtostring(this));
	return labelMediaError;
}

bool LabelType::isLabelMediaError() const
{
	Debug::trace("LabelType.LabelMediaError", HobString("isLabelMediaError(): ") + HobString(Integer::toString(labelMediaError)));
	Debug::trace("LabelType.This", "this: " + ptrtostring(this));
	return ((labelMediaError == LabelInvalid) || (labelMediaError == LabelWontFit));
}

void LabelType::setLabelMediaError(LabelMediaReturn errorNum)
{
	labelMediaError = errorNum;
	Debug::trace("LabelType.LabelMediaError", HobString("setLabelMediaError(): ") + HobString(Integer::toString(errorNum)));
	Debug::trace("LabelType.This", "this: " + ptrtostring(this));
}


const LabelType *LabelType::getEmbeddedNutritionLabelType() const
{
	return embeddedNutritionLabelType;
}

bool LabelType::labelHasNutrifacts()
{
	FormRecord *formRecord = findFieldType(NUTRI_COLUMN_1);
	if(formRecord)
	{
		embeddedNutritionLabelType = this;
		return true;
	}

	formRecord = findFieldType(NUTRI_COLUMN_2);
	if(formRecord)
	{
		embeddedNutritionLabelType = this;
		return true;
	}

	formRecord = findFieldType(NUTRI_COLUMN_3);
	if(formRecord)
	{
		embeddedNutritionLabelType = this;
		return true;
	}

	formRecord = findFieldType(NUTRI_COLUMN_4);
	if(formRecord)
	{
		embeddedNutritionLabelType = this;
		return true;
	}

	// This label is not a nutrifact label (doesn't contain nutrifact fields),
	// however, it may contain an embedded label that is the nutrifact label.
	// So see if this is the case.

	bool result = false;
	LabelType *embeddedLabelType = (LabelType *)NULL;
	if ( label )
	{
		formContainer* formsList = label->getFormsList();
		if ( formsList )
		{
			formContainerIterator iter = formsList->begin();
			while ( iter != formsList->end() && result == false )
			{
				if ( (*iter)->getFieldType() == INDIRECT_LABEL_REFERENCE )
				{
					formRecord = *iter;
					if (dbAccess)
					{
						HobRecordNum embeddedLabelTypeNumber = formRecord->getVerticalJustify();	// Yes, this field is also used to indicate the embedded label type number.
						embeddedLabelType = findRecord(embeddedLabelTypeNumber, deptNum, dbAccess);
						if ( embeddedLabelType )
						{
							result = embeddedLabelType->labelHasNutrifacts();
							if ( result == false )
								delete embeddedLabelType;
						}
					}
				}

				++iter;
			}
		}
	}

	if (embeddedLabelType)
		embeddedNutritionLabelType = embeddedLabelType;

	return result;
}

bool LabelType::labelIsEvolutionNutrifacts()
{
	bool isEvolutionNutrifacts = false;
	FieldType fieldType;

	if ( label )
	{
		formContainer* formsList = label->getFormsList();
		if ( formsList )
		{
			formContainerIterator iter = formsList->begin();
			while ( iter != formsList->end() )
			{
				fieldType = (*iter)->getFieldType();
				if ( (fieldType == NUTRI_COLUMN_1) || (fieldType == NUTRI_COLUMN_2) ||
					 (fieldType == NUTRI_COLUMN_3) || (fieldType == NUTRI_COLUMN_4) )
				{
					isEvolutionNutrifacts = true;
					break;
				}

				++iter;
			}
		}
	}

	return isEvolutionNutrifacts;
}


bool LabelType::labelHasFrequentShopper()
{
	FormRecord *formRecord = findFieldType(DISCOUNT_PRICE);
	if(formRecord)
		return true;

	formRecord = findFieldType(DISCOUNT_SAVINGS);
	if(formRecord)
		return true;

	formRecord = findFieldType(DISCOUNT_UNIT_PRICE);
	if(formRecord)
		return true;


    Debug::trace("LabelType.DiscountPrice", "labelHasFrequentShopper() DID NOT FIND any FrequentShopper discount fieldType in the label");

	return false;
}

bool LabelType::labelHasDiscountPriceInBarcode()
{
	Debug::trace("LabelType.DiscountPrice", "labelHasDiscountPriceInBarcode() START");

	FormRecord *formRecord = getFormFromList();

	if (formRecord)
	{
	    if (formRecord->isFormFlagDiscPrOnBarCode())
	    {
		    Debug::trace("LabelType.DiscountPrice", "labelHasDiscountPriceInBarcode() FOUND isFormFlagDiscPrOnBarCode() FLAG in the label");
		    return true;
	    }
    }

	Debug::trace("LabelType.DiscountPrice", "labelHasDiscountPriceInBarcode() DID NOT FIND isFormFlagDiscPrOnBarCode() FLAG in the label");
	Debug::trace("LabelType.DiscountPrice", "labelHasDiscountPriceInBarcode() END");

	return false;
}

bool LabelType::labelHasKbdNetWeight()
{
	FormRecord *formRecord = findFieldType(FORCED_KB_WEIGHT);
	if (formRecord)
		return true;
	else
		return false;
}

bool LabelType::labelHasPortions()
{
	FormRecord *formRecord = findFieldType(PORTION_COUNT);
	if (formRecord)
	{
		Debug::trace("LabelType.Portions", "labelHasPortions() = true");
		return true;
	}
	else
	{
		Debug::trace("LabelType.Portions", "labelHasPortions() = false");
		return false;
	}
}


bool LabelType::getLabelFormSize(FieldType type, int& width, int& height)
{
	FormRecord* form = findFieldType(type);
	if ( form )
	{
		width = form->getSizeHorizontal();
		height = form->getSizeVertical();

		return true;
	}

	return false;
}


bool LabelType::getLabelFormSizeAndFont(FieldType type, int& width, int& height, std::string& fontName, int& pointSize)
{
	FormRecord* form = findFieldType(type);
	if ( form )
	{
		width = form->getSizeHorizontal();
		height = form->getSizeVertical();
		fontName.assign(form->getFamilyName());
		pointSize = form->getPointSize();

		return true;
	}

	return false;
}


const LabelType::labelContainer& LabelType::getEmbeddedLabels()
{
	return embeddedLabels_;
}

void LabelType::generateEmbeddedLabels()
{
	if (!label)
		return;

	cleanupEmbeddedLabelsContainer();

	if (errorCode == RECORD_NOT_FOUND)
		return;

	Debug::trace("LabelType.embeddedLabels", "Going to generate embedded labels for label " + longtostring(getLabelTypeNum()));

	formContainer* formsList = label->getFormsList();
	if (formsList)
	{
		formContainerIterator iter = formsList->begin();
		while (iter != formsList->end())
		{
			FormRecord* formRecord = *iter;
			if (formRecord->getFieldType() == INDIRECT_LABEL_REFERENCE && dbAccess)
			{
				Debug::trace("LabelType.embeddedLabels", "Found reference to embedded label " +
					inttostring(formRecord->getVerticalJustify()));

				HobRecordNum ltNum = formRecord->getVerticalJustify();
				bool isAccumItemsItemLabelType = isLabelFormAccumulateItemsItemLabelType(ltNum);
				if (isAccumItemsItemLabelType)
					ltNum = Configuration::getInstance()->toIntCfgI(CfgInternal::ACCUMITEMSITEMLABELTYPE);

				LabelType* embeddedLabelType = findRecord(ltNum, deptNum, dbAccess);
				if (embeddedLabelType)
				{
					if (isAccumItemsItemLabelType)
						embeddedLabelType->setLabelTypeNum(formRecord->getVerticalJustify());

					// Add the embedded label to the container of embedded labels
					embeddedLabels_.push_back(embeddedLabelType);

					// Generate embedded labels for this found label
					Debug::trace("LabelType.embeddedLabels", "Going to generate embedded labels for this embedded label");
					embeddedLabelType->generateEmbeddedLabels();
				}
				else
					Debug::trace("LabelType.embeddedLabels", "********* Embedded label not present in database ********");
			}

			++iter;
		}
	}
}


// static
bool LabelType::isLabelFormAccumulateItemsItemLabelType(UINT16 verticalJustifyNum)
{
	int ltNum = verticalJustifyNum / 100;
	int accumItemsItemLabelTypeNum = Configuration::getInstance()->toIntCfgI(CfgInternal::ACCUMITEMSITEMLABELTYPE);
	if ( verticalJustifyNum == accumItemsItemLabelTypeNum || ltNum == accumItemsItemLabelTypeNum )
		return true;
	return false;
}


void LabelType::cleanupEmbeddedLabelsContainer()
{
	Debug::trace("LabelType.embeddedLabels", "Cleaning up embeddedLabels container for label " + longtostring(getLabelTypeNum()));

	labelContainerIterator iter = embeddedLabels_.begin();
	while (iter != embeddedLabels_.end())
	{
		// Delete the label
		delete *iter;

		// Move on to the next label
		++iter;
	}

	embeddedLabels_.clear();
}


bool LabelType::doIncludeEmbeddedLabels() const
{
	Debug::trace("LabelType", HobString("doIncludeEmbeddedLabels():  includeEmbeddedLabels = ") + HobString(Bool::toString(includeEmbeddedLabels)));
	return includeEmbeddedLabels;
}
void LabelType::setIncludeEmbeddedLabels(bool doInclude)
{
	includeEmbeddedLabels = doInclude;
	Debug::trace("LabelType", HobString("setIncludeEmbeddedLabels():  includeEmbeddedLabels = ") + HobString(Bool::toString(includeEmbeddedLabels)));
}


bool LabelType::updateField(int fieldOffset, const HobString &newValue)
{
	if (Debug::isTracing("LabelType"))
		Debug::trace("LabelType", string("updateField() - fieldOffset ") + inttostring(fieldOffset) + "  data " + newValue);

	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		case fieldOffsetLabelTypeDeptNum:
			setDeptNum(Long::valueOf(newValue));
			break;
		case fieldOffsetLabelTypeNum:
			setLabelTypeNum(Long::valueOf(newValue));
			break;
		case fieldOffsetLabelTypeRotationOnLabelStock:
			setRotationOnLabelStock((LabelRotationOnLabelStock) Integer::valueOf(newValue));
			break;
		case fieldOffsetLabelTypeIncludeEmbedded:
			setIncludeEmbeddedLabels(Bool::valueOf(newValue));
			break;
		case fieldOffsetLabelTypeBinaryLength:
			setResourceLength(Integer::valueOf(newValue));
			break;
		case fieldOffsetLabelTypeSequence:
			setSequence((short)Integer::valueOf(newValue));
			break;
		case fieldOffsetLabelTypeSequenceMax:
			setSequenceMax((short)Integer::valueOf(newValue));
			break;
		case fieldOffsetLabelTypeBinary:
		{
			/* This is what Windux and before did to un-nibblize the label type data to create the binary image of the label.
			* It then stored the binary data in the database.  We aren't doing this anymore for Fastrack; we are storing
			* the nibblized label type data in the database.
			HobString unnibbData("");
			HobString nibbData = newValue->getData();
			char *unnibbBuffer = new char[nibbData.size()/2 + 1];
			char *unnibbPtr = unnibbBuffer;
			HobString oneByte;
			int unnibbDataLength = 0;
			for (unsigned int i = 0; i < nibbData.size(); i+=2)
			{
			oneByte = nibbData.substr(i,2);
			char val = (char)Hex::valueOf(oneByte.c_str());
			*(unnibbPtr++) = val;
			//unnibbData.append(&val, 1);
			unnibbDataLength++;
			}
			origBinaryData = std::string(unnibbBuffer, unnibbDataLength);

			delete [] unnibbBuffer;
			*/

			// Here's what we're doing for Fastrack to store the nibblized data in the database.
			origBinaryData = newValue;
			break;
		}
		default:
			return false;
			break;
	}

	return true;
}

void LabelType::updateRecord(const LabelType *newData)
{
	if (!newData)
		return;

	if ( !custom )
	{
		// SJJC: Do we need to delete myROMDbRecord and the RM?
		custom = true;
	}
	short loopCnt = 1;
	while (loopCnt < endLabelTypeFieldOffsets)
	{
		if (newData->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetLabelTypeDeptNum:
					setDeptNum(newData->getDeptNum());
					break;
				case fieldOffsetLabelTypeNum:
					setLabelTypeNum(newData->getLabelTypeNum());
					break;
				case fieldOffsetLabelTypeRotationOnLabelStock:
					setRotationOnLabelStock(newData->getRotationOnLabelStock());
					break;
				case fieldOffsetLabelTypeBinary:
					setOrigBinaryData(newData->getOrigBinaryData());
					break;
				case fieldOffsetLabelTypeBinaryLength:
					setResourceLength(newData->getResourceLength());
					break;
				case fieldOffsetLabelTypeSequence:
					setSequence(newData->getSequence());
					break;
				case fieldOffsetLabelTypeSequenceMax:
					setSequenceMax(newData->getSequenceMax());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


bool LabelType::isFieldQuoted(int fieldOffset) const
{
	switch (fieldOffset)
	{
		case fieldOffsetLabelTypeWidth:
		case fieldOffsetLabelTypeLength:
			return true;
			break;
		default:
			break;
	}
	return false;
}

void LabelType::getFieldAsString(int fieldOffset, HobString &result, int format) const
{
	result = "";

	switch (fieldOffset)
	{
		case fieldOffsetLabelTypeDeptNum:
			result = longtostring(deptNum);
			break;
		case fieldOffsetLabelTypeNum:
			result = longtostring(labelTypeNum);
			break;
		case fieldOffsetLabelTypeRotationOnLabelStock:
			result = inttostring((int)rotationOnLabelStock);
			break;
		case fieldOffsetLabelTypeBinaryLength:
			result = inttostring(resourceLength);
			break;
		case fieldOffsetLabelTypeSequence:
			result = inttostring(sequence);
			break;
		case fieldOffsetLabelTypeSequenceMax:
			result = inttostring(sequenceMax);
			break;
		case fieldOffsetLabelTypeBinary:
		{
			bool recordPresent = false;
			if (myROMDbRecord)
			{
				// For Fastrack, we are storing the nibblized data in the database,
				// so we don't need to nibblize it when serializing to send out.
				result = myROMDbRecord->getBinary();  //Hex::nibblize(binary.data(), (int)binary.size());
				recordPresent = true;
			}
			else if (myDbRecord)
			{
				// For Fastrack, we are storing the nibblized data in the database,
				// so we don't need to nibblize it when serializing to send out.
				result = myDbRecord->getBinary();  //Hex::nibblize(binary.data(), (int)binary.size());
				recordPresent = true;
			}
			else if (getOrigBinaryData().size())
			{
				// For Fastrack, we are storing the nibblized data in the database,
				// so we don't need to nibblize it when serializing to send out.
				result = getOrigBinaryData();  //Hex::nibblize(binary.data(), (int)binary.size());
				recordPresent = true;
			}
			if (recordPresent)
				resourceLength = (int)result.size();
			break;
		}
		case fieldOffsetLabelTypeWidth:
			// PTR 1093 - quoted values possible
			result = doubletostring(getWidthInUnits(MetSig::isScaleMetric()), 1, 3);
			break;
		case fieldOffsetLabelTypeLength:
			// PTR 1093 - quoted values possible
			result = doubletostring(getLengthInUnits(MetSig::isScaleMetric()), 1, 3);
			break;
		case fieldOffsetLabelTypeIsCustom:
			result = Bool::toString(isCustom());
			break;
		default:
			break;
	}

	if (Debug::isTracing("LabelType.getFieldAsString"))
		Debug::trace("LabelType.getFieldAsString", string("fieldOffset ") + inttostring(fieldOffset) + string("  result ") + result + string("  format ") + inttostring(format));
}

const char *LabelType::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<LabelTypeFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case ViewLabelFormat:	//	Soon to be ViewLabelFormat
		{
			serializedData.append(serializeLabelToView());

			//	Serialize any embedded labels
			labelContainerIterator iter = embeddedLabels_.begin();
			while ( iter != embeddedLabels_.end() )
			{
				serializedData.append(BrowserFormatSeparator);
				serializedData.append((*iter)->serializeLabelToView());

				// Move on to the next label
				++iter;
			}
			break;
		}

		case EvolutionTaggedFormat:
		case SynchronizationTaggedFormat:
		{
			LabelTypeTags labelTypeTags;
			HobString tag = "";
			HobString fieldData = "";
			
			fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = labelTypeTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData, format);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		case DBIndependentFormatFormat:
		{
			LabelTypeTags labelTypeTags;
			HobString tag = "";
			HobString fieldData = "";
			
			fieldIter = dbindependentFieldList.begin();
			while (fieldIter != dbindependentFieldList.end())
			{
				tag = labelTypeTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData, format);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		default:
			serializedData = string("LabelType::serialize - unknown format:") + inttostring(format);
			break;
	}
	return serializedData.data();
}

const string LabelType::serializeLabelToView() const
{
	string labelData = "";

	bool firstTime = true;
	HobString fieldData = "";
	list<LabelTypeFieldOffsets>::iterator fieldIter = viewLabelFieldList.begin();
	while (fieldIter != viewLabelFieldList.end())
	{
		if (firstTime)
			firstTime = false;
		else
			labelData.append(BrowserFormatSeparator);

		getFieldAsString(*fieldIter, fieldData, ViewLabelFormat);
		if (isFieldQuoted(*fieldIter))
			fieldData.quoteForCSV();

		labelData.append(fieldData);
		fieldIter++;
	}

	return labelData;
}


DBAccess *LabelType::getDBAccess() const
{
	return dbAccess;
}

void LabelType::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}

bool LabelType::isRecordValid() const
{
	if (myDbRecord)
		return myDbRecord->isRecordValid();
	else if (myROMDbRecord)
		return myROMDbRecord->isRecordValid();

	return false;
}

int LabelType::deleteRecord()
{
	int result = 0;
	if (custom && myDbRecord && dbAccess)
	{
		DBLabelsRM *dbRM = dbAccess->getDBLabelsRM();
		if (dbRM)
		{
			Debug::trace("LabelType", string("deleteRecord() - num ") + longtostring(labelTypeNum) + "  deptNum " + longtostring(deptNum));
			result = dbRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void LabelType::saveRecord()
{
	if (custom && labelTypeNum > 0 && dbAccess)
	{
		DBLabelsRM *dbRM = dbAccess->getDBLabelsRM();
		if (!dbRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("LabelType", string("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbRM->createRecord();
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the dbLabelRecord
			myDbRecord->setDeptNumber(longtostring(deptNum));
			myDbRecord->setNumber(longtostring(labelTypeNum));
			myDbRecord->setRotation(inttostring(rotationOnLabelStock));
			if (!origBinaryData.empty())
			{
				Debug::trace("LabelType", string("    saving unknown format from origBinaryData - LabelType: ") + longtostring(labelTypeNum));
				myDbRecord->setBinary(origBinaryData);
			}
			else if (label && label->isFormatChanged())
			{
				Debug::trace("LabelType", string("    saving old format LabelType: ") + longtostring(labelTypeNum));
				const Blob &binaryBlob = label->getBinaryData();

				/* This is what Windux and before used to do, since they stored the label type's binary data in the database.
				char *binaryChar = (char *) (const_cast<Blob &>(binaryBlob)).getBlobBuffer();
				int binaryLength = (const_cast<Blob &>(binaryBlob)).getSize();
				std::string binary(binaryChar, binaryLength);
				myDbRecord->setBinary(binary);
				*/

				// This is what Fastrack does to store the nibblized data in the database.
				(const_cast<Blob &> (binaryBlob)).nibblizeBlob();
				char *nibbChar = (char *)(const_cast<Blob &>(binaryBlob)).getNibblizedBuffer();
				int nibbLength = (const_cast<Blob &>(binaryBlob)).getNibblizedSize();
				std::string nibbData(nibbChar, nibbLength);
				myDbRecord->setBinary(nibbData);
			}
			else
				Debug::trace("LabelType", string("    DON'T KNOW HOW TO SAVE THIS - but should already be in new format - LabelTypeNum: ") + longtostring(labelTypeNum));

			try
			{
				dbRM->saveRecord(myDbRecord);		//save record to the database
				Debug::trace("LabelType", string("saveRecord() - saved num ") + longtostring(labelTypeNum) + string("  deptNum ") + longtostring(deptNum));
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "LabelType::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}


// static
LabelType *LabelType::findRecord(HobRecordNum labelTypeNum, HobRecordNum deptNum, DBAccess *dbAccess)
{
	LabelType *rec = NULL;

	// DeptNum can be 0, so leave next check as < 0
	if (labelTypeNum <= 0 || deptNum < 0 || !dbAccess)
		return rec;

	DBLabelsRM *dbRM = dbAccess->getDBLabelsRM();
	if (!dbRM)
		return rec;

	// FT-529: search ROM labels also
	DBROMLabelsRM *dbRMrom = dbAccess->getDBROMLabelsRM();
	if (!dbRMrom)
		return rec;

	try
	{
		DBLabel *dbRec = dbRM->findNumber(longtostring(labelTypeNum), longtostring(deptNum));
		if (dbRec)
		{
			rec = new LabelType(dbRec, dbAccess);
			if (!rec)
			{
				delete dbRec;
				Debug::trace("LabelType", "findRecord() - rec not created: " + ptrtostring(rec) + "  num " + longtostring(labelTypeNum) + "  deptNum " + longtostring(deptNum));
				return 0;
			}
		}
		else
		{
			// If the label type is not found in the dept that was passed in, look for it in the default department (dept 0)
			dbRec = dbRM->findNumber(longtostring(labelTypeNum), "0");
			if (dbRec)
			{
				rec = new LabelType(dbRec, dbAccess);
				if (!rec)
				{
					delete dbRec;
					Debug::trace("LabelType", "findRecord() - rec not created: " + ptrtostring(rec) + "  num " + longtostring(labelTypeNum) + "  deptNum 0");
					return 0;
				}
				else
					// Make this label look like a ROM label so the user can't delete it, since it was found in the default department, not the currently active dept.
					rec->setCustom(false);
			}
			// FT-529: search ROM labels also
			else
			{
				DBROMLabel* dbRec = dbRMrom->findNumber(longtostring(labelTypeNum));
				if (dbRec)
				{
					rec = new LabelType(dbRec, dbAccess);
					if (!rec)
					{
						delete dbRec;
						Debug::trace("LabelType", "findRecord() - rom rec not created: " + ptrtostring(rec) + "  num " + longtostring(labelTypeNum));
						return 0;
					}
				}
				else
				{
					rec = new LabelType(labelTypeNum);
					rec->setDeptNum(deptNum);
				}
			}
		}

		if (Debug::isTracing("LabelType"))
			Debug::trace("LabelType", "findRecord() - rec ptr: " + ptrtostring(rec) + "  num " + longtostring(rec->getLabelTypeNum()) + "  deptNum " + longtostring(rec->getDeptNum()));
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("LabelType::findRecord() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return rec;
}

// static
LabelType *LabelType::findCustomRecord(HobRecordNum labelTypeNum, HobRecordNum deptNum, DBAccess *dbAccess)
{
	LabelType *rec = NULL;

	// DeptNum can be 0, so leave next check as < 0
	if (labelTypeNum <= 0 || deptNum < 0 || !dbAccess)
		return rec;

	DBLabelsRM *dbRM = dbAccess->getDBLabelsRM();
	if (!dbRM)
		return rec;

	try
	{
		DBLabel *dbRec = dbRM->findNumber(longtostring(labelTypeNum), longtostring(deptNum));
		if (dbRec)
		{
			rec = new LabelType(dbRec, dbAccess);
			if (!rec)
			{
				delete dbRec;
				Debug::trace("LabelType", "findCustomRecord() - rec not created: " + ptrtostring(rec) + "  num " + longtostring(labelTypeNum) + "  deptNum " + longtostring(deptNum));
			}
			else if (Debug::isTracing("LabelType"))
				Debug::trace("LabelType", "findCustomRecord() - rec ptr: " + ptrtostring(rec) + "  num " + longtostring(rec->getLabelTypeNum()) + "  deptNum " + longtostring(rec->getDeptNum()));
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("LabelType::findCustomRecord() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return rec;
}

// static
LabelType *LabelType::findROMRecord(HobRecordNum labelTypeNum, DBAccess *dbAccess)
{
	LabelType *rec = NULL;

	if (labelTypeNum <= 0 || !dbAccess)
		return rec;

	DBROMLabelsRM *dbRM = dbAccess->getDBROMLabelsRM();
	if (!dbRM)
		return rec;

	try
	{
		DBROMLabel *dbRec = dbRM->findNumber(longtostring(labelTypeNum));
		if (dbRec)
		{
			rec = new LabelType(dbRec, dbAccess);
			if (!rec)
			{
				delete dbRec;
				Debug::trace("LabelType", "findROMRecord() - rec not created: " + ptrtostring(rec) + "  num " + longtostring(labelTypeNum));
			}
			else if (Debug::isTracing("LabelType"))
				Debug::trace("LabelType", "findROMRecord() - rec ptr: " + ptrtostring(rec) + "  num " + longtostring(rec->getLabelTypeNum()));
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("LabelType::findROMRecord() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return rec;
}


void LabelType::show() const
{
	Debug::trace("LabelType", "LabelType number: " + util_string::longtostring(labelTypeNum) + (custom ? " (custom)" : " (ROM)"));
	Debug::trace("LabelType", "rotationOnLabelStock: " + inttostring((int)rotationOnLabelStock));
	Debug::trace("LabelType", "HobartLabel: ");
	label->show();
}
