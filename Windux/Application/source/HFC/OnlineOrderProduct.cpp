/* Evolution Scale Project

 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#define USE_NEW_EVOL_TAGS

#include "OnlineOrderProduct.h"
#include "ProductDefault.h"
#include "MetSig.h"
#include "ProductTags.h"	
#include "ProductDevEvent.h"
#include "ProductTransaction.h"
#include "Class.h"
#include "Nutrifacts.h"
#include "ProductText.h"
#include "Graphic.h"
#include "HfcError.h"
#include "HfcErrorGraphic.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"		
#include "HobTokenTagged.h"

#include "Configuration.h"
#include "SessionManager.h"
#include "SessionMessage.h"
#include "Session.h"
#include "Rounding.h"
#include "LabelTypeManager.h"

#include "WriteSingleField.h"

#include "FrequentShopper.h"

#include "HfcWeigherStatus.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBProductsRM.h"
#include "DBProduct.h"
#include "DBCoolListsRM.h"
#include "DBCoolList.h"
#include "DBCoolListTextsRM.h"
#include "DBCoolListText.h"
#include "DBLabelsRM.h"
#include "DBROMLabelsRM.h"

#include "MultiLanguageText.h"

#include "MemShow.h"


#include "Special.h"
#include "Category.h"

#include "LabelApplier.h"

#include "COOLText.h"
#include "COOLPretext.h"

#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Bool.h"
#include "Hex.h"
#include "math.h"

using namespace std;


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int OnlineOrderProduct::_dataType_ = 0;
	std::string OnlineOrderProduct::_dataTypeName_("OnlineOrderProduct");
#endif

list<OnlineOrderProduct::ProductFieldOffsets> OnlineOrderProduct::uiFieldList;


OnlineOrderProduct::OnlineOrderProduct()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	
	Debug::trace("OnlineOrderProduct.ctor", " OnlineOrderProduct()  this: " + ptrtostring(this) );

	init();
}

OnlineOrderProduct::OnlineOrderProduct(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	
	Debug::trace("Product.ctor", " OnlineOrderProduct(data,len,formatType)  this: " + ptrtostring(this) );
	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			std::list<ProductFieldOffsets>::iterator fieldIter = uiFieldList.begin();
			int offset;

			// Only for the UI, initially set the deptNum to the currently active dept.
			// If we receive the deptNum with the data (which at the time of multi-dept development we don't), then the dept number received will override this one.
			// Commented out because we receive the deptNum in the message
			//setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != uiFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, (token.length() == 0) ? false : true);
				if (token.length())
				{
					token.unQuoteForCSV();
					updateField(offset, token, false, formatType);
				}
				fieldIter++;
			}

			break;
		}

		default:
			break;
	}
}


OnlineOrderProduct::OnlineOrderProduct(DBProduct *dbRec, DBAccess *access, bool loadLinkedData) : Product(dbRec, access, loadLinkedData)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


OnlineOrderProduct::~OnlineOrderProduct()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	Debug::trace("Product.dtor", "    OnlineOrderProduct: " + ptrtostring(this) + "  num " + longtostring(productNum) + "  deptNum " + longtostring(deptNum));

}

void OnlineOrderProduct::init()
{
	orderNum = 42;
	orderProductId = 0;
	customerName = "John Doe";
	pickupTime = "ASAP";
	initFieldLists();
}

// static
void OnlineOrderProduct::initFieldLists()
{
	if (uiFieldList.empty())
	{
		uiFieldList.push_back(fieldOffsetProductNum);			// PLU Number
		uiFieldList.push_back(fieldOffsetOrderNum);
		uiFieldList.push_back(fieldOffsetOrderProductId);		// Product ID per the online order db, not the PLU number
		uiFieldList.push_back(fieldOffsetOrderCustomerName);
		uiFieldList.push_back(fieldOffsetOrderPickupTime);
		uiFieldList.push_back(fieldOffsetByCount);
	}

}


void OnlineOrderProduct::updateOnlineOrderingData(OnlineOrderProduct *ooSourceProduct)
{
	Debug::trace("OnlineOrderProduct", "updateOnlineOrderingData() - this: " + ptrtostring(this) + " ooSourceProduct: " + ptrtostring(ooSourceProduct));
	if (ooSourceProduct)
	{
		setOrderNum(ooSourceProduct->getOrderNum());
		setOrderProductId(ooSourceProduct->getOrderProductId());
		setCustomerName(ooSourceProduct->getCustomerName());
		setPickupTime(ooSourceProduct->getPickupTime());
		// Can't handle this here
		//if (ooSourceProduct->isReceived(fieldOffsetByCount))
		//	setByCount(ooSourceProduct->getByCount());
	}
}


/*****************************************************************************************
*
* updateField: Update an attribute for the field indicated by the fieldOffset passed in.
*              The data for the field is passed in as a string.
*
****************************************************************************************/
bool OnlineOrderProduct::updateField(int fieldOffset, const HobString &newValue, bool linksMustExist, int formatType)
{
	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		case fieldOffsetOrderNum:
			setOrderNum(Long::valueOf(newValue));
			break;

		case fieldOffsetOrderProductId:
			setOrderProductId(Long::valueOf(newValue));
			break;

		case fieldOffsetOrderCustomerName:
			setCustomerName(newValue);
			break;

		case fieldOffsetOrderPickupTime:
			setPickupTime(newValue);
			break;

		default:
			return Product::updateField(fieldOffset, newValue, linksMustExist, formatType);
			break;
	}

	return true;
}

