/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcProductSupportDataMsg.h"
#include "Product.h"
#include "ProductText.h"
#include "Nutrifacts.h"

#include "HobTokenizerCSV.h"
#include "HFCManagerContainer.h"
#include "ProductManager.h"
#include "NutrifactsManager.h"
#include "ProductTextManager.h"
#include "SessionMessage.h"
#include "Configuration.h"

#include "DBProductTextsRM.h"
#include "DBProductText.h"
#include "DBNutritionsRM.h"
#include "DBNutrition.h"
#include "DBProductsRM.h"
#include "DBAccess.h"

#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Bool.h"
#include "Hex.h"

#include "util/e_string/utilstring.h"
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcProductSupportDataMsg::dataType = 0;
std::string HfcProductSupportDataMsg::dataTypeName("HfcProductSupportDataMsg"); 
#endif

HfcProductSupportDataMsg::HfcProductSupportDataMsg(Product *pProduct)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	myProduct = pProduct;
}

HfcProductSupportDataMsg::HfcProductSupportDataMsg(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case InternalFormat:
			//setAllReceived(true);
			// Why am I receiving a HfcProductSupportDataMsg from the RTC????  I shouldn't be!
			break;

		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch(loopCnt)
				{
					case fieldOffsetPSDCurrentProductNum:
						setCurrentProductNum((HobRecordNum)Long::valueOf(token.c_str()));
						break;
					case fieldOffsetPSDTextType:
						setProdSupportTextType((ProdSupportTextType)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPSDUsedByMany:
						//	We don't want this back from the UI
						break;
					case fieldOffsetPSDFunction:
						setProdSupportFunction((ProdSupportFunction)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPSDProductNum:
						setProductNum((HobRecordNum)Long::valueOf(token.c_str()));
						break;
					case fieldOffsetPSDText:
					{
						HobString tempText(token);
						Debug::trace("HfcProductSupportDataMsg", HobString("String: ") + tempText);

						token.unQuoteForCSV();
						setText(token);
					}
						break;
					case fieldOffsetPSDTextBlock2:
					{
						HobString tempText(token);
						Debug::trace("HfcProductSupportDataMsg", HobString("TextBlock2: ") + tempText);

						token.unQuoteForCSV();
						setTextBlock2(token);
					}
						break;
					case fieldOffsetPSDTextBlock3:
					{
						HobString tempText(token);
						Debug::trace("HfcProductSupportDataMsg", HobString("TextBlock3: ") + tempText);

						token.unQuoteForCSV();
						setTextBlock3(token);
					}
						break;
					case fieldOffsetPSDTextBlock4:
					{
						HobString tempText(token);
						Debug::trace("HfcProductSupportDataMsg", HobString("TextBlock4: ") + tempText);

						token.unQuoteForCSV();
						setTextBlock4(token);
					}
						break;

					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcProductSupportDataMsg::~HfcProductSupportDataMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


void HfcProductSupportDataMsg::init()
{
	myProduct = NULL;
	currentProductNum = RECORD_NOT_FOUND;
	supportTextType = psttUnknown;
	text = "";
	textBlock2 = "";
	textBlock3 = "";
	textBlock4 = "";
	editFunction = psfNoFunction;
	productNum = RECORD_NOT_FOUND;
	usedByMany = false;
}


HobRecordNum HfcProductSupportDataMsg::getCurrentProductNum()
{
	return currentProductNum;
}
void HfcProductSupportDataMsg::setCurrentProductNum(HobRecordNum productNum)
{
	this->currentProductNum = productNum;
}

ProdSupportTextType HfcProductSupportDataMsg::getProdSupportTextType()
{
	return supportTextType;
}
void HfcProductSupportDataMsg::setProdSupportTextType(ProdSupportTextType type)
{
	this->supportTextType = type;
}

HobString HfcProductSupportDataMsg::getText()
{
	return text;
}
void HfcProductSupportDataMsg::setText(HobString text)
{
	int len = (int)text.length();
	Debug::trace("HfcProductSupportDataMsg", HobString("setText length = ") + HobString(Integer::toString(len)));
	this->text = text;
}

HobString HfcProductSupportDataMsg::getTextBlock2()
{
	return textBlock2;
}
void HfcProductSupportDataMsg::setTextBlock2(HobString text)
{
	this->textBlock2 = text;
}

HobString HfcProductSupportDataMsg::getTextBlock3()
{
	return textBlock3;
}
void HfcProductSupportDataMsg::setTextBlock3(HobString text)
{
	this->textBlock3 = text;
}

HobString HfcProductSupportDataMsg::getTextBlock4()
{
	return textBlock4;
}
void HfcProductSupportDataMsg::setTextBlock4(HobString text)
{
	this->textBlock4 = text;
}

ProdSupportFunction HfcProductSupportDataMsg::getProdSupportFunction()
{
	return editFunction;
}
void HfcProductSupportDataMsg::setProdSupportFunction(ProdSupportFunction function)
{
	this->editFunction = function;
}

HobRecordNum HfcProductSupportDataMsg::getProductNum()
{
	return productNum;
}
void HfcProductSupportDataMsg::setProductNum(HobRecordNum productNum)
{
	this->productNum = productNum;
}


bool HfcProductSupportDataMsg::isUsedByMany()
{
	return usedByMany;
}
void HfcProductSupportDataMsg::setUsedByMany(bool usedByMany)
{
	this->usedByMany = usedByMany;
}


void HfcProductSupportDataMsg::newText(DBAccess *dbAccess, Product *product)
{
	if (!product || !dbAccess)
		return;

	HobString	 textIs;
	bool		 reuseText = false;
	HobRecordNum textNum;

	switch (supportTextType)
	{
		case psttNutrifacts:
		{
			Debug::trace("HfcProductSupportDataMsg", "newText");

			Nutrifacts *currentNutrifacts = product->getNutrifacts();
			if (currentNutrifacts)
			{
				if ((currentNutrifacts->getTextNum() != RECORD_NOT_FOUND) && (currentNutrifacts->isUsedByMany() == false))
				{
					//	Reuse the current expanded text as no other Product uses it.
					Debug::trace("HfcProductSupportDataMsg", "newText: setting NutritionFacts Text to empty");
//					currentNutrifacts->setText("");
					currentNutrifacts->setColumn1Data("");
					currentNutrifacts->setColumn2Data("");
					currentNutrifacts->setColumn3Data("");
					currentNutrifacts->setColumn4Data("");
					reuseText = true;
				}
			}

			if (reuseText == false)
			{
				Nutrifacts *newNutrifacts;

				//	Get a unique, new Nutrifacts number.
				//	Typically the text number is the same as the Product number, but not always.
				//	Check by calling some routine that will	return this product number, if available, or a number closest to it.
				textNum = findNewNutrifactsNumber(dbAccess, currentProductNum, product->getDeptNum());

				if (textNum != RECORD_NOT_FOUND)
				{
					//	Create a new Nutrifacts object.
					Debug::trace("HfcProductSupportDataMsg", "newText: going to create new Nutrition record");
					newNutrifacts = new Nutrifacts();

					if (newNutrifacts != NULL)
					{
						//	Set the Nutrifacts number.
						newNutrifacts->setDeptNum(product->getDeptNum());
						newNutrifacts->setTextNum(textNum);
						//	Set the Database pointer.
						newNutrifacts->setDBAccess(dbAccess);
						//	This is a new record, so it is OK.
						newNutrifacts->setErrorCode(RECORD_OK);

						product->setNutrifacts(newNutrifacts);
//						this->text = newNutrifacts->getText();
						this->text = newNutrifacts->getColumn1Data();
						this->textBlock2 = newNutrifacts->getColumn2Data();
						this->textBlock3 = newNutrifacts->getColumn3Data();
						this->textBlock4 = newNutrifacts->getColumn4Data();

						Debug::trace("HfcProductSupportDataMsg", "newText: created new Nutrition record");
					}
				}
				else
					Debug::trace("HfcProductSupportDataMsg", "newText: Nutrition record not found");

			}
			this->editFunction = psfNoFunction;
			textIs = "Nutrifacts";
			break;
		}

		case psttExpandedText:
		case psttSpecialMessage:
		case psttProductNote:
		case psttMarqueeText:
		case psttProductText5:
		case psttProductText6:
		case psttProductText7:
		case psttProductText8:
		case psttProductText9:
		{
			ProductText *currentProductText = product->getProductText(supportTextType);
			if (currentProductText)
			{
				if ((currentProductText->getTextNum() != RECORD_NOT_FOUND) && (currentProductText->isUsedByMany() == false))
				{
					//	Reuse the current product text, as no other Product uses it.
					currentProductText->setText("");
					reuseText = true;
				}
			}

			if (reuseText == false)
			{
				ProductText *newProductText;

				//	Get a unique, new Product text number.
				//	Typically the text number is the same as the Product number, but not always.
				//	Check by calling some routine that will	return this product number, if available, or a number closest to it.
				textNum = findNewProductTextNumber(dbAccess, currentProductNum, product->getDeptNum(), supportTextType);

				if (textNum != RECORD_NOT_FOUND)
				{
					//	Create a new Product text class.
					newProductText = new ProductText(supportTextType);

					if (newProductText != NULL)
					{
						//	Set the Product text number.
						newProductText->setDeptNum(product->getDeptNum());
						newProductText->setTextNum(textNum);
						//	Set the Database pointer.
						newProductText->setDBAccess(dbAccess);
						//	This is a new record, so it is OK.
						newProductText->setErrorCode(RECORD_OK);

						product->setProductText(supportTextType, newProductText);
						this->text = newProductText->getText();
					}
				}

			}
			this->editFunction = psfNoFunction;
			textIs = "ProductText type:" + inttostring(supportTextType);
			break;
		}

		default:
			textIs = "UNKNOWN";
			break;
	}
	Debug::trace("HfcProductSupportDataMsg", HobString("New text: ") + textIs + HobString(" *") + this->text.c_str() + HobString("*"));
}


bool HfcProductSupportDataMsg::copyText(DBAccess *dbAccess, Product *product)
{
	if (product == NULL)
		return false;

	HobString textIs;
	bool foundText = false;
	HobRecordNum reuseNumber = RECORD_NOT_FOUND;

	switch (supportTextType)
	{
		case psttNutrifacts:
		{
			Nutrifacts *currentNutrifacts = product->getNutrifacts();
			if (currentNutrifacts)
			{
				if ((currentNutrifacts->getTextNum() != RECORD_NOT_FOUND) && (currentNutrifacts->isUsedByMany() == false))
				{
					//	Reuse the current expanded text number as no other Product uses it.
					reuseNumber = currentNutrifacts->getTextNum();
				}
			}
			//	Read the Nutrifacts associated with the specified productNum.
			HobRecordNum textNum = getNutrifactsNumFromProduct(dbAccess, productNum, product->getDeptNum());
			Nutrifacts *copiedNutrifacts = Nutrifacts::findRecord(textNum, product->getDeptNum(), dbAccess);

			if (copiedNutrifacts)
			{
				if (reuseNumber != RECORD_NOT_FOUND)
				{
					 textNum = reuseNumber;
				}
				else
				{
					//	Get a unique, new Nutrifacts number.
					//	Typically the text number is the same as the Product number, but not always.
					//	Check by calling some routine that will	return this product number, if available, or a number closest to it.
					textNum = findNewNutrifactsNumber(dbAccess, currentProductNum, product->getDeptNum());
				}
				copiedNutrifacts->setDeptNum(product->getDeptNum());
				copiedNutrifacts->setTextNum(textNum);

				product->setNutrifacts(copiedNutrifacts);
//				this->text = copiedNutrifacts->getText();
				this->text = copiedNutrifacts->getColumn1Data();
				this->textBlock2 = copiedNutrifacts->getColumn2Data();
				this->textBlock3 = copiedNutrifacts->getColumn3Data();
				this->textBlock4 = copiedNutrifacts->getColumn4Data();

				foundText = true;
			}
			this->editFunction = psfNoFunction;
			textIs = "Nutrifacts";
			break;
		}

		case psttExpandedText:
		case psttSpecialMessage:
		case psttProductNote:
		case psttMarqueeText:
		case psttProductText5:
		case psttProductText6:
		case psttProductText7:
		case psttProductText8:
		case psttProductText9:
		{
			ProductText *currentProductText = product->getProductText(supportTextType);
			if (currentProductText)
			{
				if (currentProductText->getTextNum() != RECORD_NOT_FOUND && currentProductText->isUsedByMany() == false)
					//	Reuse the current product text number, as no other Product uses it.
					reuseNumber = currentProductText->getTextNum();
			}
			//	Read the Product text associated with the specified productNum.
			HobRecordNum textNum = getProductTextNumFromProduct(dbAccess, productNum, product->getDeptNum(), supportTextType);
			ProductText *copiedProductText = ProductText::findRecord(supportTextType, textNum, product->getDeptNum(), dbAccess);

			if (copiedProductText)
			{
				if (reuseNumber != RECORD_NOT_FOUND)
					 textNum = reuseNumber;
				else
				{
					//	Get a unique, new Product text number for this type.
					//	Typically the text number is the same as the Product number, but not always.
					//	Check by calling some routine that will	return this product number, if available, or a number closest to it.
					textNum = findNewProductTextNumber(dbAccess, currentProductNum, product->getDeptNum(), supportTextType);
				}
				copiedProductText->setTextNum(textNum);

				product->setProductText(supportTextType, copiedProductText);
				this->text = copiedProductText->getText();

				foundText = true;
			}

			this->editFunction = psfNoFunction;
			textIs = "ProductText type:" + inttostring(supportTextType);
			break;
		}

		default:
			textIs = "UNKNOWN";
			break;
	}

	Debug::trace("HfcProductSupportDataMsg", HobString("Copy: ") + textIs + HobString(" from Product # ") + Integer::toString(productNum));
	return foundText;
}

void HfcProductSupportDataMsg::deleteText(DBAccess *dbAccess, Product *product)
{
	if (!dbAccess || !product)
		return;

	HobString	textIs("");
	bool		usedByMany = false;

	switch (supportTextType)
	{
		case psttNutrifacts:
		{
			Nutrifacts *currentNutrifacts = product->getNutrifacts();
			if(currentNutrifacts)
			{
				usedByMany = currentNutrifacts->isUsedByMany();
				Debug::trace("HfcProductSupportDataMsg", HobString("deleteText, Nutrifacts:  usedByMany = ") + HobString(Integer::toString(usedByMany)));
				
				if(editFunction == psfDeleteForAll)
				{
					//	Delete the Nutrifacts for all PLU's that use it.
					removeNutrifactsFromAllProducts(dbAccess, currentNutrifacts);
				}
				if((editFunction == psfDeleteForAll) || ((editFunction == psfDelete) && (usedByMany == false)))
				{
					Debug::trace("HfcProductSupportDataMsg", HobString("Deleting the Nutrifacts record."));

					if (mgrContainer && mgrContainer->getNutrifactsManager())
					{
						SessionMessage msg(NutrifactsDev, DtDelete);
						msg.setHfcObject(currentNutrifacts);
						mgrContainer->getNutrifactsManager()->handleMessage(&msg);
					}
					else
						// Leave this here just in case the mgrContainer is null, which it shouldn't be.
						currentNutrifacts->deleteRecord();
				}
			}
			//	Delete the Nutrifacts number for this PLU.
			Nutrifacts *noNutrifacts = new Nutrifacts();
			product->setNutrifacts(noNutrifacts);
			textIs = "Nutrifacts";
			break;
		}

		case psttExpandedText:
		case psttSpecialMessage:
		case psttProductNote:
		case psttMarqueeText:
		case psttProductText5:
		case psttProductText6:
		case psttProductText7:
		case psttProductText8:
		case psttProductText9:
		{
			ProductText *currentProductText = product->getProductText(supportTextType);
			if (currentProductText)
			{
				usedByMany = currentProductText->isUsedByMany();
				Debug::trace("HfcProductSupportDataMsg", "deleteText, ProductText:  type:" + inttostring(supportTextType) + "  usedByMany:" + booltostring(usedByMany));
				
				if (editFunction == psfDeleteForAll)
				{
					//	Delete the Product text for all PLU's that use it.
					removeProductTextFromAllProducts(dbAccess, currentProductText);
				}

				if ((editFunction == psfDeleteForAll) || (editFunction == psfDelete && usedByMany == false))
				{
					Debug::trace("HfcProductSupportDataMsg", "Deleting the Product text record.  type:" + inttostring(supportTextType));

					if (mgrContainer && mgrContainer->getProductTextManager(supportTextType))
					{
						EvolDeviceType dev = UnknownDevice;
						if (supportTextType == psttExpandedText)
							dev = ExpandedTextDev;
						else if (supportTextType == psttSpecialMessage)
							dev = SpecialMessageDev;
						else if (supportTextType == psttProductNote)
							dev = ProductNotesDev;
						else if (supportTextType == psttMarqueeText)
							dev = MarqueeTextDev;
						else if (supportTextType == psttProductText5)
							dev = ProductText5Dev;
						else if (supportTextType == psttProductText6)
							dev = ProductText6Dev;
						else if (supportTextType == psttProductText7)
							dev = ProductText7Dev;
						else if (supportTextType == psttProductText8)
							dev = ProductText8Dev;
						else if (supportTextType == psttProductText9)
							dev = ProductText9Dev;

						if (dev != UnknownDevice)
						{
							SessionMessage msg(dev, DtDelete);
							msg.setHfcObject(currentProductText);
							mgrContainer->getProductTextManager(supportTextType)->handleMessage(&msg);
						}
					}
					else
						// Leave this here just in case the mgrContainer is null, which it shouldn't be.
						currentProductText->deleteRecord();
				}
			}

			//	Delete the Product text number for this PLU.
			ProductText *noProductText = new ProductText(supportTextType);
			product->setProductText(supportTextType, noProductText);

			textIs = "ProductText type:" + inttostring(supportTextType);
			break;
		}

		default:
			textIs = "UNKNOWN";
			break;
	}
	Debug::trace("HfcProductSupportDataMsg", HobString("Delete: ") + textIs + HobString(" from Product # ") + Integer::toString(currentProductNum));
}


bool HfcProductSupportDataMsg::useText(DBAccess *dbAccess, Product *product)
{
	if (!product)
		return false;

	HobString textIs;
	bool foundText = false;

	switch (supportTextType)
	{
		case psttNutrifacts:
		{
			//	Read the Nutrifacts associated with the specified productNum.
			HobRecordNum textNum = getNutrifactsNumFromProduct(dbAccess, productNum, product->getDeptNum());
			Nutrifacts *usedNutrifacts = Nutrifacts::findRecord(textNum, product->getDeptNum(), dbAccess);

			//	Note: The Nutrifacts can not be edited under this selection.
			if (Debug::isTracing("HfcProductSupportDataMsg"))
				Debug::trace("HfcProductSupportDataMsg", HobString(" useText()   Nutrifacts: ") + HobString(Hex::toString((long long) usedNutrifacts)) + HobString("  Nutrifacts number:") + (usedNutrifacts ? HobString(Long::toString(usedNutrifacts->getTextNum())) : HobString("0")) + HobString(" - useNutrifacts()"));

			if (usedNutrifacts)
			{
				usedNutrifacts->setUsedByMany(true);

				setUsedByMany(true);
//				setText(usedNutrifacts->getText());
				setText(usedNutrifacts->getColumn1Data());
				setTextBlock2(usedNutrifacts->getColumn2Data());
				setTextBlock3(usedNutrifacts->getColumn3Data());
				setTextBlock4(usedNutrifacts->getColumn4Data());
				setProdSupportFunction(psfNoFunction);
				
				product->setNutrifacts(usedNutrifacts);
				if (Debug::isTracing("HfcProductSupportDataMsg"))
					Debug::trace("HfcProductSupportDataMsg", HobString(" useText()   usedNutrifacts->isUsedByMany() = ") + HobString(Bool::toString(usedNutrifacts->isUsedByMany())));
				foundText = true;
			}
			textIs = "Nutrifacts";
			break;
		}

		case psttExpandedText:
		case psttSpecialMessage:
		case psttProductNote:
		case psttMarqueeText:
		case psttProductText5:
		case psttProductText6:
		case psttProductText7:
		case psttProductText8:
		case psttProductText9:
		{
			//	Read the Product text associated with the specified productNum.
			HobRecordNum textNum = getProductTextNumFromProduct(dbAccess, productNum, product->getDeptNum(), supportTextType);
			ProductText *usedProductText = ProductText::findRecord(supportTextType, textNum, product->getDeptNum(), dbAccess);

			//	Note: The Product text can not be edited under this selection.
			if (Debug::isTracing("HfcProductSupportDataMsg"))
				Debug::trace("HfcProductSupportDataMsg", " useText()   ProductText: " + ptrtostring(usedProductText) + "  type:" + inttostring(supportTextType) + "  number:" + (usedProductText ? longtostring(usedProductText->getTextNum()) : "0"));

			if (usedProductText)
			{
				usedProductText->setUsedByMany(true);

				setUsedByMany(true);
				setText(usedProductText->getText());
				setProdSupportFunction(psfNoFunction);
				
				product->setProductText(supportTextType, usedProductText);

				if (Debug::isTracing("HfcProductSupportDataMsg"))
					Debug::trace("HfcProductSupportDataMsg", " useText()   usedExpandedText->isUsedByMany() = " + booltostring(usedProductText->isUsedByMany()));
				foundText = true;
			}

			textIs = "ProductText type:" + inttostring(supportTextType);
			break;
		}

		default:
			textIs = "UNKNOWN";
			break;
	}
	Debug::trace("HfcProductSupportDataMsg", HobString("Use: ") + textIs + HobString(" from Product # ") + Integer::toString(productNum) + HobString("   foundText = ") + Bool::toString(foundText));
	return foundText;
}

TextLinks *HfcProductSupportDataMsg::getTextLinks(DBAccess *dbAccess, Product *product)
{
	TextLinks *textLinks = 0;

	if (dbAccess && product)
	{
		switch (supportTextType)
		{
			case psttNutrifacts:
			{
				Debug::trace("HfcProductSupportDataMsg", HobString("    getTextLinks():   psttNutrifacts"));
				Nutrifacts *myNutrifacts = product->getNutrifacts();
				if (myNutrifacts && (myNutrifacts->getErrorCode() != RECORD_NOT_FOUND))
					textLinks = new TextLinks(dbAccess, psttNutrifacts, myNutrifacts->getMyDbRecord());
				else
					Debug::trace("HfcProductSupportDataMsg", HobString("    getTextLinks():   no Nutrifacts"));
				break;
			}

			case psttExpandedText:
			case psttSpecialMessage:
			case psttProductNote:
			case psttMarqueeText:
			case psttProductText5:
			case psttProductText6:
			case psttProductText7:
			case psttProductText8:
			case psttProductText9:
			{
				Debug::trace("HfcProductSupportDataMsg", "  getTextLinks():  type:" + inttostring(supportTextType));
				ProductText *myProductText = product->getProductText(supportTextType);
				if (myProductText && myProductText->getErrorCode() != RECORD_NOT_FOUND)
					textLinks = new TextLinks(dbAccess, supportTextType, myProductText->getMyDbRecord());
				else
					Debug::trace("HfcProductSupportDataMsg", "  getTextLinks():   no ProductText for type:" + inttostring(supportTextType));
				break;
			}

			default:
				break;
		}
	}
	return textLinks;
}


HobRecordNum HfcProductSupportDataMsg::findNewProductTextNumber(DBAccess *dbAccess, HobRecordNum textNum, HobRecordNum deptNum, ProdSupportTextType supportTextType)
{
	if (dbAccess && textNum > 0)	//	No need to access the Database if the number is not > 0
	{
		edb::DBProductTextsRM *productTextsRM = dbAccess->getDBProductTextsRM(supportTextType);
		if (productTextsRM)
		{ 
			edb::DBProductText *dbProductTextRec = productTextsRM->findNumber(longtostring(textNum), longtostring(deptNum));  // Don't need to pass in supportTextType here, the getDBProductTextsRM already knows which Product Text type is is associated with.
			while (dbProductTextRec)
			{
				delete dbProductTextRec;
				dbProductTextRec = productTextsRM->findNumber(longtostring(++textNum), longtostring(deptNum));
			}
		}
	}
	return textNum;
}

HobRecordNum HfcProductSupportDataMsg::getProductTextNumFromProduct(DBAccess *dbAccess, HobRecordNum productNum, HobRecordNum deptNum, ProdSupportTextType supportTextType)
{
	HobRecordNum num = 0;

	if (dbAccess && productNum > 0)	//	No need to access the Database if the number is not > 0
	{
		DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
		if (dbProductsRM)
		{ 
			DBProduct *dbProductRec = dbProductsRM->findPLU(longtostring(productNum), longtostring(deptNum));
			if (dbProductRec)
			{
				switch (supportTextType)
				{
					case psttExpandedText:
						num = (dbProductRec->isTextNull()) ? 0 : Integer::valueOf(dbProductRec->getText());
						break;
					case psttSpecialMessage:
						num = (dbProductRec->isSpecialMessageNull()) ? 0 : Integer::valueOf(dbProductRec->getSpecialMessage());
						break;
					case psttProductNote:
						num = (dbProductRec->isNoteNull()) ? 0 : Integer::valueOf(dbProductRec->getNote());
						break;
					case psttMarqueeText:
						num = (dbProductRec->isMarqueeNull()) ? 0 : Integer::valueOf(dbProductRec->getMarquee());
						break;
					case psttProductText5:
						num = (dbProductRec->isProductText5Null()) ? 0 : Integer::valueOf(dbProductRec->getProductText5());
						break;
					case psttProductText6:
						num = (dbProductRec->isProductText6Null()) ? 0 : Integer::valueOf(dbProductRec->getProductText6());
						break;
					case psttProductText7:
						num = (dbProductRec->isProductText7Null()) ? 0 : Integer::valueOf(dbProductRec->getProductText7());
						break;
					case psttProductText8:
						num = (dbProductRec->isProductText8Null()) ? 0 : Integer::valueOf(dbProductRec->getProductText8());
						break;
					case psttProductText9:
						num = (dbProductRec->isProductText9Null()) ? 0 : Integer::valueOf(dbProductRec->getProductText9());
						break;
					default:
						break;
				}
				delete dbProductRec;
			}
		}
	}
	return num;
}

void HfcProductSupportDataMsg::removeProductTextFromAllProducts(DBAccess *dbAccess, ProductText *productText)
{
	if (!dbAccess || !productText || !mgrContainer || !mgrContainer->getProductManager())
		return;

	ProdSupportTextType type = productText->getTextType();
	if (type <= psttUnknown || type >= psttLastProductText)
		return;

	if (productText->getTextNum() != RECORD_NOT_FOUND)	//	Can't delete anything if there is no Product text record.
	{
		Debug::trace("HfcProductSupportDataMsg", "removeProductTextFromAllProducts()");
		ProductManager *prodMgr = mgrContainer->getProductManager();

		TextLinks *textLinks = new TextLinks(dbAccess, type, productText->getMyDbRecord());
		if (textLinks)
		{
			DBQuery<DBProduct>* productList = textLinks->getLinkList();
			if (productList)
			{
				DBQuery<DBProduct>::iterator iter = productList->begin();
				while (iter != productList->end())
				{
					DBProduct *p = new DBProduct((*iter));
					++iter;

					Product *prod = new Product(p, dbAccess);
					if (prod)
					{
						prod->setProductText(type, 0);
						prodMgr->handleSaveRecord(prod);
						delete prod;
					}
				}
			}
			delete textLinks;
		}
	}
}



HobRecordNum HfcProductSupportDataMsg::getNutrifactsNumFromProduct(DBAccess *dbAccess, HobRecordNum productNum, HobRecordNum deptNum)
{
	HobRecordNum nutrifactsNum = 0;

	if (dbAccess && (productNum > 0))	//	No need to access the Database if the number is not > 0
	{
		DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
		if (dbProductsRM)
		{ 
			DBProduct *dbProductRec = dbProductsRM->findPLU(longtostring(productNum), longtostring(deptNum));
			if (dbProductRec)
			{
				nutrifactsNum = Integer::valueOf(dbProductRec->getNutrition());
				delete dbProductRec;
			}
		}
	}
	return nutrifactsNum;
}

HobRecordNum HfcProductSupportDataMsg::findNewNutrifactsNumber(DBAccess *dbAccess, HobRecordNum textNum, HobRecordNum deptNum)
{
	if (dbAccess && (textNum > 0))	//	No need to access the Database if the number is not > 0
	{
		DBNutritionsRM *dbNutritionsRM = dbAccess->getDBNutritionsRM();
		if (dbNutritionsRM)
		{
			DBNutrition *dbNutrifactsRec = dbNutritionsRM->findNumber(longtostring(textNum), longtostring(deptNum));
			while(dbNutrifactsRec != NULL)
			{
				delete dbNutrifactsRec;
				dbNutrifactsRec = dbNutritionsRM->findNumber(longtostring(++textNum), longtostring(deptNum));
			}
		}
	}
	return textNum;
}

void HfcProductSupportDataMsg::removeNutrifactsFromAllProducts(DBAccess *dbAccess, Nutrifacts *nutrifacts)
{
	if((dbAccess == NULL) || (nutrifacts == NULL) || mgrContainer == NULL || mgrContainer->getProductManager() == NULL)
		return;

	if(nutrifacts->getTextNum() != RECORD_NOT_FOUND)	//	Can't delete anything if there is no Nutrifacts record.
	{
		Debug::trace("HfcProductSupportDataMsg", HobString("removeNutrifactsFromAllProducts()"));
		TextLinks *textLinks = new TextLinks(dbAccess, psttNutrifacts, nutrifacts->getMyDbRecord());

		DBQuery<DBProduct>* productList = textLinks->getLinkList();
		if ( productList )
		{
			// DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();    // Unused
	//		int count = productList->size();

			ProductManager *prodMgr = mgrContainer->getProductManager();

			DBQuery<DBProduct>::iterator iter = productList->begin();
			while (iter != productList->end())
			{
				DBProduct *p = new DBProduct((*iter));
				++iter;
				Product *prod = new Product(p, dbAccess);
				prod->setNutrifacts(0);
				prodMgr->handleSaveRecord(prod);
				delete prod;

				//iter->setNutritionNull();	//	Set the Nutrifacts value to NULL. No Nutrifacts.
				//dbProductsRM->saveRecord(&*iter);
				//++iter;
			}
		}
		delete textLinks;
	}
}



const char *HfcProductSupportDataMsg::serialize(int format) const
{
	HfcObject::serializedData = "";

	switch (format)
	{
		case BrowserFormat:
			// Serialize the ProductNum, Special Message, Expanded Text, Nutritional Info, ... for the browser.
			serializedData = "";
			for (int loopCnt = 1; loopCnt < endProductSupportDataFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetPSDCurrentProductNum:
						serializedData.append(Long::toString(currentProductNum));
						break;
					case fieldOffsetPSDTextType:
						serializedData.append(Integer::toString(supportTextType));
						break;
					case fieldOffsetPSDUsedByMany:
						serializedData.append(Bool::toString(usedByMany));
						break;
					case fieldOffsetPSDFunction:
						serializedData.append((editFunction != psfNoFunction) ? Integer::toString(editFunction) : "");
						break;
					case fieldOffsetPSDProductNum:
						serializedData.append((editFunction != psfNoFunction) ? Long::toString(productNum) : "");
						break;
					case fieldOffsetPSDText:
						if(editFunction == psfNoFunction)
						{
							HobString tempText(text);
							tempText.quoteForCSV();
							serializedData.append(tempText);
						}
						else
							serializedData.append("");
						break;
					case fieldOffsetPSDTextBlock2:
						if ((editFunction == psfNoFunction))
						{
							HobString tempText(textBlock2);
							tempText.quoteForCSV();
							serializedData.append(tempText);
						}
						else
							serializedData.append("");
						break;
					case fieldOffsetPSDTextBlock3:
						if ((editFunction == psfNoFunction))
						{
							HobString tempText(textBlock3);
							tempText.quoteForCSV();
							serializedData.append(tempText);
						}
						else
							serializedData.append("");
						break;
					case fieldOffsetPSDTextBlock4:
						if ((editFunction == psfNoFunction))
						{
							HobString tempText(textBlock4);
							tempText.quoteForCSV();
							serializedData.append(tempText);
						}
						else
							serializedData.append("");
						break;
					default:
						break;
				}
			}
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.c_str();
}

void HfcProductSupportDataMsg::setMgrContainer(hfc::HFCManagerContainer *container)
{
	mgrContainer = container;
}
