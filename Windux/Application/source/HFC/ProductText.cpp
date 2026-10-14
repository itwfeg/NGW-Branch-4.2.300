/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductText.h"
#include "Product.h"
#include "TextLinks.h"
#include "Configuration.h"

#include "ExpandedTextTags.h"	
#include "SpecialMessageTags.h"
#include "ProductNotesTags.h"
#include "MarqueeTextTags.h"
#include "ProductText5Tags.h"
#include "ProductText6Tags.h"
#include "ProductText7Tags.h"
#include "ProductText8Tags.h"
#include "ProductText9Tags.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "util/e_string/utilstring.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBProductTextsRM.h"
#include "DBProductText.h"
#include "DBProductsRM.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductText::dataType = 0;
string ProductText::dataTypeName("ProductText"); 
#endif

list<TextFieldOffsets> ProductText::uiFieldList;
list<TextFieldOffsets> ProductText::evolFieldList;


ProductText::ProductText(ProdSupportTextType type)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	textType_ = type;
}

ProductText::ProductText(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			std::list<TextFieldOffsets>::iterator fieldIter = uiFieldList.begin();
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

			// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
			// If we don't receive it via comm/merge/restore, or other mechanism in the subsequent cases, then it remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetTextDeptNum))
			{
				setReceived(fieldOffsetTextDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			ExpandedTextTags   expandedTextTags;
			SpecialMessageTags specialMessageTags;
			ProductNotesTags   productNotesTags;
			MarqueeTextTags    mqTextTags;
			ProductText5Tags   productText5Tags;
			ProductText6Tags   productText6Tags;
			ProductText7Tags   productText7Tags;
			ProductText8Tags   productText8Tags;
			ProductText9Tags   productText9Tags;

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
					if (temp.size())
					{
						//if we can't find it in the expanded text tags then check in special message tags
						if (!(offset = expandedTextTags.getOffset(temp)))
							if (!(offset = specialMessageTags.getOffset(temp)))
								if (!(offset = productNotesTags.getOffset(temp)))
									if (!(offset = mqTextTags.getOffset(temp)))
										if (!(offset = productText5Tags.getOffset(temp)))
											if (!(offset = productText6Tags.getOffset(temp)))
												if (!(offset = productText7Tags.getOffset(temp)))
													if (!(offset = productText8Tags.getOffset(temp)))
															offset = productText9Tags.getOffset(temp);
					}
					else
						offset = 0;

					if (Debug::isTracing("ProductText.ctor"))
						Debug::trace("ProductText.ctor", string("ProductText(data,len,format) token: ") + temp + string("  offset:") + inttostring(offset) + string("  data:") + token->getData());
					if (offset)
					{
						setReceived(offset, true);
						temp = token->getData();
						if (offset == fieldOffsetTextText)
						{
							//check the string, if ultima escape sequence is present then convert to an evol font 
							temp.convertToEvolFontSequences();
							temp.stripChar('\x1D');
						}
						updateField(offset, temp);
					}
				}	
				delete token;
			}

			// For comm/backup/restore, if we didn't receive the deptNum with the data and the config parameter is set, set the deptNum to the currently active dept.
			// If we didn't receive the deptNum with the data and the config parameter is NOT set (normal case), deptNum remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetTextDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetTextDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			break;
	}
}

ProductText::ProductText(DBProductText *dbRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("ProductText.ctor", string("ProductText(DBProductText, DBAccess) - dbRecPtr passed in ") + ptrtostring(dbRec));
	init();

	dbAccess_ = access;

	if (dbRec)
	{
		myDbRecord_ = dbRec;

		deptNum_ = stringtolong(dbRec->getDeptNumber());
		textType_ = (ProdSupportTextType) stringtoint(dbRec->getType());
		textNum_ = stringtolong(dbRec->getNumber());
		text_ = dbRec->getText();
		errorCode_ = RECORD_OK;
	}
}


ProductText::~ProductText()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord_)
		delete myDbRecord_;

	Debug::trace("ProductText.dtor", "    ProductText: " + ptrtostring(this) + "  num " + longtostring(textNum_) + "  deptNum " + longtostring(deptNum_));
}


void ProductText::init()
{
	deptNum_ = 0;
	textType_ = psttUnknown;
	textNum_ = RECORD_NOT_FOUND;
	text_ = "";
	usedByMany_ = false;
	errorCode_ = RECORD_NOT_FOUND;

	dbAccess_ = 0;
	myDbRecord_ = 0;

	initFieldLists();
}

// static
void ProductText::initFieldLists()
{
	if (uiFieldList.empty())
	{
		uiFieldList.push_back(fieldOffsetTextNum);
		uiFieldList.push_back(fieldOffsetTextText);
		uiFieldList.push_back(fieldOffsetTextErrorCode);
	}
	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetTextDeptNum);
		evolFieldList.push_back(fieldOffsetTextNum);
		evolFieldList.push_back(fieldOffsetTextText);

		ExpandedTextTags expandedTextTags((FieldOffsetsList *)&evolFieldList);
		SpecialMessageTags specialMessageTags((FieldOffsetsList *)&evolFieldList);
		MarqueeTextTags marqTextTags((FieldOffsetsList *)&evolFieldList);
		ProductNotesTags productNotesTags((FieldOffsetsList *)&evolFieldList);
		ProductText5Tags productText5Tags((FieldOffsetsList *)&evolFieldList);
		ProductText6Tags productText6Tags((FieldOffsetsList *)&evolFieldList);
		ProductText7Tags productText7Tags((FieldOffsetsList *)&evolFieldList);
		ProductText8Tags productText8Tags((FieldOffsetsList *)&evolFieldList);
		ProductText9Tags productText9Tags((FieldOffsetsList *)&evolFieldList);
	}
}


ProdSupportTextType ProductText::getTextType() const
{
	return textType_;
}
void ProductText::setTextType(ProdSupportTextType type)
{
	textType_ = type;
}

HobRecordNum ProductText::getDeptNum() const
{
	return deptNum_;
}
void ProductText::setDeptNum(HobRecordNum num)
{
	deptNum_ = num;
}

HobRecordNum ProductText::getTextNum() const
{
	return textNum_;
}
void ProductText::setTextNum(HobRecordNum num)
{
	textNum_ = num;
}

HobString ProductText::getText() const
{
	return text_;
}
void ProductText::setText(HobString txt)
{
	text_ = txt;
}

bool ProductText::isUsedByMany() const
{
	return usedByMany_;
}
void ProductText::setUsedByMany(bool usedByMany)
{
	usedByMany_ = usedByMany;
}

HobErrorCode ProductText::getErrorCode() const
{
	return errorCode_;
}
void ProductText::setErrorCode(HobErrorCode code)
{
	errorCode_ = code;
}

void ProductText::testIfUsedByMany()
{
	if (myDbRecord_ && dbAccess_)
	{
		TextLinks *textLinks = new TextLinks(dbAccess_, textType_, myDbRecord_);
		if (textLinks)
		{
			edb::DBQuery<edb::DBProduct>* linkList = textLinks->getLinkList();
			if (linkList)
				usedByMany_ = (linkList->size() > 1) ? true : false;
			delete textLinks;
		}
	}
}


// Returns true if the field was updated, false if not
bool ProductText::updateField(int fieldOffset, const HobString &newValue)
{
	if (Debug::isTracing("ProductText"))
		Debug::trace("ProductText", string("updateField() - fieldOffset ") + inttostring(fieldOffset) + "  data " + newValue);

	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		case fieldOffsetTextDeptNum:
			setDeptNum(stringtolong(newValue));
			break;
		case fieldOffsetTextNum:
			setTextNum(stringtolong(newValue));
			break;
		case fieldOffsetTextText:
			setText(newValue);
			break;
		default:
			return false;
			break;
	}

	return true;
}

void ProductText::updateRecord(const ProductText *newData)
{
	if (!newData)
		return;

	if (Debug::isTracing("ProductText"))
	{
		Debug::trace("ProductText", string("updateRecord() - this num ") + longtostring(textNum_) + string("  this DeptNum ") + longtostring(deptNum_));
		Debug::trace("ProductText", string("updateRecord() - passed in num ") + longtostring(newData->getTextNum()) + string("  passed in DeptNum ") + longtostring(newData->getDeptNum()));
	}

	short loopCnt = 1;
	while (loopCnt < endTextFieldOffsets)
	{
		if (newData->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetTextDeptNum:
					setDeptNum(newData->getDeptNum());
					break;
				case fieldOffsetTextNum:
					setTextNum(newData->getTextNum());
					break;
				case fieldOffsetTextText:
					setText(newData->getText());
					break;
				case fieldOffsetTextErrorCode:
					setErrorCode(newData->getErrorCode());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


bool ProductText::isFieldQuoted(int fieldOffset) const
{
	switch (fieldOffset)
	{
		case fieldOffsetTextText:
			return true;
			break;
		default:
			break;
	}
	return false;
}

void ProductText::getFieldAsString(int fieldOffset, HobString &result, int format) const
{
	result = "";

	switch (fieldOffset)
	{
		case fieldOffsetTextDeptNum:
			result = longtostring(deptNum_);
			break;
		case fieldOffsetTextNum:
			result = longtostring(textNum_);
			break;
		case fieldOffsetTextText:
			result = text_;
			break;
		case fieldOffsetTextErrorCode:
			result = inttostring(errorCode_);
			break;
		default:
			break;
	}

	if (Debug::isTracing("ProductText.getFieldAsString"))
		Debug::trace("ProductText.getFieldAsString", string("fieldOffset ") + inttostring(fieldOffset) + string("  result ") + result + string("  format ") + inttostring(format));
}

const char *ProductText::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	list<TextFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case BrowserFormat:
		{
			bool firstTime = true;
			HobString fieldData = "";
			fieldIter = uiFieldList.begin();
			while (fieldIter != uiFieldList.end())
			{
				if (firstTime)
					firstTime = false;
				else
					serializedData.append(BrowserFormatSeparator);

				getFieldAsString(*fieldIter, fieldData, format);
				if (isFieldQuoted(*fieldIter))
					fieldData.quoteForCSV();

				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			Tags *whichTags = 0;
			if (textType_ == psttExpandedText)
				whichTags = new ExpandedTextTags();
			else if (textType_ == psttSpecialMessage)
				whichTags = new SpecialMessageTags();
			else if (textType_ == psttProductNote)
				whichTags = new ProductNotesTags();
			else if (textType_ == psttMarqueeText)
				whichTags = new MarqueeTextTags();
			else if (textType_ == psttProductText5)
				whichTags = new ProductText5Tags();
			else if (textType_ == psttProductText6)
				whichTags = new ProductText6Tags();
			else if (textType_ == psttProductText7)
				whichTags = new ProductText7Tags();
			else if (textType_ == psttProductText8)
				whichTags = new ProductText8Tags();
			else if (textType_ == psttProductText9)
				whichTags = new ProductText9Tags();

			if (whichTags)
			{
				HobString tag = "";
				HobString fieldData = "";

				fieldIter = evolFieldList.begin();
				while (fieldIter != evolFieldList.end())
				{
					tag = whichTags->getTag(*fieldIter);
					if (tag.size())
					{
						getFieldAsString(*fieldIter, fieldData, EvolutionTaggedFormat);

						if (format == UltimaTaggedFormat && *fieldIter == fieldOffsetTextText)
							fieldData.convertToUltimaFontSequences();

						serializedData.append(tag);
						serializedData.append(fieldData);
						serializedData.append(UnitSeparator);
					}
					fieldIter++;
				}
				delete whichTags;
			}
			break;
		}

		default:
			serializedData = string("ProductText::serialize - unknown format:") + inttostring(format);
			break;
	}
	return serializedData.data();
}


DBProductText *ProductText::getMyDbRecord() const
{
	return myDbRecord_;
}

void ProductText::setDBAccess(DBAccess *access)
{
	dbAccess_ = access;
}

bool ProductText::isRecordValid() const
{
	if (myDbRecord_)
		return myDbRecord_->isRecordValid();
	else
		return false;
}

int ProductText::deleteRecord()
{
	int result = 0;
	if (myDbRecord_ && dbAccess_)   //No need to delete if there is no record
	{
		DBProductTextsRM *dbRM = dbAccess_->getDBProductTextsRM(textType_);
		if (dbRM)
		{
			Debug::trace("ProductText", string("deleteRecord() - type ") + inttostring((int)textType_) + string("  num ") + longtostring(textNum_) + string("  deptNum ") + longtostring(deptNum_));
			result = dbRM->deleteRecord(myDbRecord_);
		}
	}
	return result;
}

void ProductText::saveRecord()
{
	if (textType_ > psttUnknown && textType_ < psttLastProductText && textNum_ > 0 && dbAccess_)   //No need to save to the Database if the number is 0
	{
		DBProductTextsRM *dbRM = dbAccess_->getDBProductTextsRM(textType_);
		if (!dbRM)
			return;

		if (!myDbRecord_)
		{
			Debug::trace("ProductText", string("saveRecord() - don't have a db record, creating it..."));
			myDbRecord_ = dbRM->createRecord();
		}
		
		if (myDbRecord_)
		{
			myDbRecord_->setDeptNumber(longtostring(deptNum_));
			myDbRecord_->setNumber(longtostring(textNum_));
			myDbRecord_->setText(text_);

			try
			{
				dbRM->saveRecord(myDbRecord_);
				Debug::trace("ProductText", string("saveRecord() - saved type ") + inttostring((int)textType_) + string("  num ") + longtostring(textNum_) + string("  deptNum ") + longtostring(deptNum_));
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "ProductText::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + inttostring(t) + HobString("  info:") + dbe.info());
			}
		}
	}
}

// static
ProductText *ProductText::findRecord(ProdSupportTextType type, HobRecordNum textNum, HobRecordNum deptNum, DBAccess *dbAccess)
{
	ProductText *rec = (ProductText *)NULL;

	if (type <= psttUnknown || type >= psttLastProductText || textNum <= 0 || deptNum < 0 || !dbAccess)
		return rec;

	DBProductTextsRM *dbRM = dbAccess->getDBProductTextsRM(type);
	if (!dbRM)
		return rec;

	try
	{
		DBProductText *dbRec = dbRM->findNumber(longtostring(textNum), longtostring(deptNum), (int)type);
		if (dbRec)
		{
			rec = new ProductText(dbRec, dbAccess);
			if (!rec)
				delete dbRec;
			else
			{
				if (Debug::isTracing("ProductText"))
					Debug::trace("ProductText", string("findRecord() - rec ptr: ") + ptrtostring(rec) + "  type " + inttostring(rec->getTextType()) + "  num " + longtostring(rec->getTextNum()) + "  deptNum " + longtostring(rec->getDeptNum()));
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("ProductText::findRecord() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + inttostring(t) + HobString("  info:") + dbe.info());
		}
	}

	return rec;
}

