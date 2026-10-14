/* HLX Scale Project
 * COPYRIGHT (c) 2009.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobAllAccumulatedItems.h"
#include "HobList.h"
#include "Configuration.h"
#include "Product.h"
#include "ProductTransaction.h"

#include "Long.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobAllAccumulatedItems::dataType = 0;
std::string HobAllAccumulatedItems::dataTypeName("HobAllAccumulatedItems"); 
#endif

HobAllAccumulatedItems::HobAllAccumulatedItems()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

// private
void HobAllAccumulatedItems::init()
{
	positionInList = new HobList();
	pluNumList = new HobList();
	descList = new HobList();
	if (descList)
		descList->setQuotingElements(true);
	byCountWeightList = new HobList();
	unitPriceList = new HobList();
	totalPriceList = new HobList();

	totalPrice = 0;
}

HobAllAccumulatedItems::~HobAllAccumulatedItems()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (positionInList)
		delete positionInList;
	if (pluNumList)
		delete pluNumList;
	if (descList)
		delete descList;
	if (byCountWeightList)
		delete byCountWeightList;
	if (unitPriceList)
		delete unitPriceList;
	if (totalPriceList)
		delete totalPriceList;
}

const char *HobAllAccumulatedItems::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
// the following if is commented out to send even empty lists.
//			if (positionInList->size() && pluNumList->size() && descList->size() && byCountWeightList->size() && unitPriceList->size() && totalPriceList->size())
			{
				HobString totalPriceStr;
				Configuration::getInstance()->formatPrice(totalPrice, totalPriceStr);
				serializedData.append(totalPriceStr);
				serializedData.append(BrowserFormatSeparator);

				serializedData.append(positionInList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(pluNumList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(descList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(byCountWeightList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(unitPriceList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(totalPriceList->serialize(format));
			}
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}

void HobAllAccumulatedItems::addAccumulatedItem(const Product *prod, const ProductTransaction *trans)
{
	if (!prod || !trans)
		return;

	Configuration *pConfig = Configuration::getInstance();

	if (positionInList)
	{
		int num = positionInList->getListCount() + 1;
		positionInList->push_back(HobString(Integer::toString(num)));
	}
	if (pluNumList)
		pluNumList->push_back(HobString(Long::toString(prod->getProductNum())));
	if (descList)
	{
		HobString newDescription(prod->getDescription());
		newDescription.stripFontSequences();
		descList->push_back(newDescription);
	}
	if (byCountWeightList)
	{
		if ( (prod->isFixedWtOrFluidOz() || prod->isRandomWeight()) && trans->getTotalNetWeight())
		{
			HobString result;
			pConfig->formatWeight(trans->getTotalNetWeight(), result);
			byCountWeightList->push_back(result);
		}
		else
			byCountWeightList->push_back(HobString(Integer::toString(trans->getByCount())));
	}
	HobString result;
	if (unitPriceList)
	{
		pConfig->formatUnitPriceForPrinting(prod, trans, result);
		unitPriceList->push_back(result);
	}
	if (totalPriceList)
	{
		result = pConfig->formatHobPrice(trans->getTotalPrice(), false, (pConfig->toBoolCfg(fieldOffsetCfgPFCurrencySymbolPrinted) ? Configuration::UseMonetarySymbolPFFlag : 0));
		totalPriceList->push_back(result);
	}
}

void HobAllAccumulatedItems::addAccumulatedItem(HobRecordNum pluNum, HobString desc, HobString byCountOrWeight, HobPrice unitPrice, HobPrice totalPrice)
{
	if (positionInList)
	{
		int num = positionInList->getListCount() + 1;
		positionInList->push_back(HobString(Integer::toString(num)));
	}
	if (pluNumList)
		pluNumList->push_back(HobString(Long::toString(pluNum)));
	if (descList)
	{
		desc.stripFontSequences();
		descList->push_back(desc);
	}
	if (byCountWeightList)
		byCountWeightList->push_back(byCountOrWeight);
	HobString result;
	if (unitPriceList)
	{
		Configuration::getInstance()->formatPrice(unitPrice, result);
		unitPriceList->push_back(result);
	}
	if (totalPriceList)
	{
		result = "";
		Configuration::getInstance()->formatPrice(totalPrice, result);
		totalPriceList->push_back(result);
	}
}

HobValue64 HobAllAccumulatedItems::getTotalPrice() const
{
	return totalPrice;
}

void HobAllAccumulatedItems::setTotalPrice(const HobValue64 total)
{
	totalPrice = total;
}
