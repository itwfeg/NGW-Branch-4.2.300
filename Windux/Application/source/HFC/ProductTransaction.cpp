#include "ProductTransaction.h"


#include "Product.h"
#include "HobOperator.h"
#include "LabelType.h"
#include "Configuration.h"

#include "HfcWeigherStatus.h"
#include "ProductTransactionMaster.h"

#include "Integer.h"
#include "Long.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



#ifdef USE_ACCOUNTANT
	int ProductTransaction::dataType = 0;
	std::string ProductTransaction::dataTypeName("ProductTransaction"); 
#endif

// initialize statics
HobRecordNum ProductTransaction::nextTransactionNum = (HobRecordNum)0;


ProductTransaction::ProductTransaction(Product * product)
	: Transaction()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	//Debug::trace("ProductTransaction.Constructor", "START Default Constructor #1 Product Transaction: " + ptrtostring(this) );

	init();

	if ( product )
	{
		setProduct(product);

		/* There is no need to set the product fields in this transaction record at this time.
		 * The product will set them when the transaction is being sent to the RTC for printing.
		 * They are set in Product::printCurrentTransaction(). */
	}

	//Debug::trace("ProductTransaction.Constructor", "FINISH Default Constructor #1 Product Transaction: " + ptrtostring(this) );
}


ProductTransaction::ProductTransaction(const ProductTransaction & prodTransaction)
		: Transaction(prodTransaction), labelRotations(prodTransaction.labelRotations),
		barCodeNumber(prodTransaction.getBarCodeNum())
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	//Debug::trace("ProductTransaction.Constructor", "START Constructor #2 Product Transaction: " + ptrtostring(this) );

	myProduct = prodTransaction.myProduct;

	transactionNum = nextTransactionNum++;
	transactionNumberForBarcode = transactionNum;

	transactionState = prodTransaction.transactionState;

	shelfLife = prodTransaction.shelfLife;
	productLife = prodTransaction.productLife;
	packedOn = prodTransaction.packedOn;

	// labelRotations not coming from the previous ProductTransaction???

	// barCodeNumber = prodTransaction.barCodeNumber;  // was not coming from the previous ProductTransaction???

	if (prodTransaction.getWeigherStatus() == NULL)
		weigherStatus = (HfcWeigherStatus *)NULL;
	else
		weigherStatus = new HfcWeigherStatus(*prodTransaction.getWeigherStatus());

	if (prodTransaction.getFrequentShopperData() == NULL)
		frequentShopperData = (FrequentShopper *)NULL;
	else
		frequentShopperData = new FrequentShopper(*prodTransaction.getFrequentShopperData());

	// Page number for reports
	reportPageNumber = prodTransaction.reportPageNumber;

	forcePreImage = prodTransaction.forcePreImage;
	backDateDays = prodTransaction.backDateDays;

	numOfLabelsToPrint = prodTransaction.numOfLabelsToPrint;

	updateProductionPlanning_ = prodTransaction.updateProductionPlanning_;
	printLabels = prodTransaction.printLabels;
	cuttingTestNumber = prodTransaction.cuttingTestNumber;
	portions = prodTransaction.portions;
	
	totalItems = prodTransaction.totalItems;
	productTransactionMaster = 0;
	printedRunTotalsTransLabel = false;

	//Debug::trace("ProductTransaction.Constructor", "FINISH Constructor Product Transaction: " + ptrtostring(this) );
}


ProductTransaction::~ProductTransaction()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	
	//Debug::trace("ProductTransaction.Delete", "START Deconstructor Product Transaction: " + ptrtostring(this) );

	if ( weigherStatus )
	{
		delete weigherStatus;
		weigherStatus = NULL;
	}

	if ( frequentShopperData )
	{
		delete frequentShopperData;
		frequentShopperData = NULL;
	}
	
	//Debug::trace("ProductTransaction.Delete", "FINISH Deconstructor Product Transaction: " + ptrtostring(this) );
}


void ProductTransaction::init()
{
	myProduct = (Product *)NULL;

	transactionNum = nextTransactionNum++;
	transactionNumberForBarcode = transactionNum;

	transactionState = TRANSACTIONSTATE_INITIAL;

	// tare initializes itself
	// shelfLife initializes itself to 'today'
	// productLife initializes itself to 'today'
	// packedOn initializes itself to 'today'
	// std::vector<short> labelRotations initializes itself
	// barCodeNumber initializes itself

	weigherStatus = (HfcWeigherStatus *)NULL;

	frequentShopperData = new FrequentShopper();

	reportPageNumber = 0;
	forcePreImage = false;

	backDateDays = 0;

	numOfLabelsToPrint = 1;         // As per Larry

	updateProductionPlanning_ = true;
	printLabels = true;
	printedRunTotalsTransLabel = false;

	totalItems = 0;
	productTransactionMaster = 0;

	cuttingTestNumber = RECORD_NOT_FOUND;
	printASecurityLabel = false;

	portions = 0;				//	Portions per package
	avgPortionPrice = 0;		//	Price per portion
	avgFSPortionPrice = 0;		//	Frequent shopper price per portion
	avgPortionWeight = 0;		//	Price per unit of weight
}


int ProductTransaction::getByCount() const
{
	if ( transactionState == TRANSACTIONSTATE_INITIAL && myProduct )
		return myProduct->getByCount();
	else
		return Transaction::getByCount();
}

HobRecordNum ProductTransaction::getProductNum() const
{
        return ((myProduct) ? myProduct->getProductNum() : RECORD_NOT_FOUND);
}


HobPrice ProductTransaction::getUnitPrice() const
{
	if (transactionState == TRANSACTIONSTATE_INITIAL && myProduct)
		return myProduct->getUnitPrice();
	else
		return Transaction::getUnitPrice();
}


/*
 * Note: The shelfLifeDays and shelfLifeHours and shelfLifeMinutes need to have been set before calling this method.
 */
void ProductTransaction::setShelfLife() const
{
	shelfLife.setToCurrent();
	shelfLife.incrementDays(-backDateDays);
	shelfLife.incrementDays(getShelfLifeDays());
	shelfLife.incrementHours((int)getShelfLifeHours());
	shelfLife.incrementMinutes((int)getShelfLifeMinutes());		// added new

	if (Debug::isTracing("ProductTransaction.lifeDates"))
	{
		HobString msgDebug = "setShelfLife():  ";
		msgDebug.append("backDateDays = ").append(Integer::toString(backDateDays));
		msgDebug.append("  shelfLifeDays = ").append(Integer::toString(getShelfLifeDays()));
		msgDebug.append("  shelfLifeHours = ").append(Integer::toString(getShelfLifeHours()));
		msgDebug.append("  shelfLifeMinutes = ").append(Integer::toString(getShelfLifeMinutes()));
		msgDebug.append("  shelfLife = ").append(shelfLife.formatDate(FormatDate_MMDDYY));
		Debug::trace("ProductTransaction.lifeDates", msgDebug);
	}
}


/*
 * Note: The productLifeDays and productLifeHours need to have been set before calling this method.
 */
void ProductTransaction::setProductLife() const
{
	productLife.setToCurrent();
	productLife.incrementDays(-backDateDays);
	productLife.incrementDays(getProductLifeDays());
	productLife.incrementHours((int)getProductLifeHours());
	productLife.incrementMinutes((int)getProductLifeMinutes());		// added new

	if (Debug::isTracing("ProductTransaction.lifeDates"))
	{
		HobString msgDebug = "setProductLife():  ";
		msgDebug.append("backDateDays = ").append(Integer::toString(backDateDays));
		msgDebug.append("  productLifeDays = ").append(Integer::toString(getProductLifeDays()));
		msgDebug.append("  productLifeHours = ").append(Integer::toString(getProductLifeHours()));
		msgDebug.append("  productLifeMinutes = ").append(Integer::toString(getProductLifeMinutes()));
		msgDebug.append("  productLife = ").append(productLife.formatDate(FormatDate_MMDDYY));
		Debug::trace("ProductTransaction.lifeDates", msgDebug);
	}
}


void ProductTransaction::setPackedOn() const
{
	packedOn.setToCurrent();
	packedOn.incrementDays(-backDateDays);
	if (Debug::isTracing("ProductTransaction.lifeDates"))
	{
		HobString msgDebug = "setPackedOn():  ";
		msgDebug.append("backDateDays = ").append(Integer::toString(backDateDays));
		msgDebug.append("  packedOn = ").append(packedOn.formatDate(FormatDate_MMDDYY));
		Debug::trace("ProductTransaction.lifeDates", msgDebug);
	}
}


void ProductTransaction::setWeigherStatus(const HfcWeigherStatus *weigherStatus)
{
	if (this->weigherStatus)
		delete this->weigherStatus;

	if (weigherStatus)
		this->weigherStatus = new HfcWeigherStatus(weigherStatus);
	else
		this->weigherStatus = (HfcWeigherStatus *) 0;
}


void ProductTransaction::setBarCodeNum(BarCodeNumber &barCodeNumber)
{
	this->barCodeNumber = barCodeNumber;
	setBarcodePrefix(util_string::stringtoint(barCodeNumber.getPrefix()));
}


void ProductTransaction::buildBarcodeStrings()
{
	barCodeNumber.buildBarcodeStrings(this, false);
	// LAP-BARCODE
	//setBarcodeNumber(barCodeNumber.getBarcodeString());

	barCodeNumber.buildBarcodeStrings(this);
}


void ProductTransaction::setFrequentShopperData(const FrequentShopper & frequentShopper)
{
	if (frequentShopperData)
		delete frequentShopperData;

	frequentShopperData = new FrequentShopper(frequentShopper);
}


void ProductTransaction::calculateFSDiscounts(const Product *pProduct)
{
	frequentShopperData->calculateDiscounts(pProduct);
}


bool ProductTransaction::needsNoWeigherStatus() const
{
	return (myProduct) ? myProduct->needsNoWeigherStatus() : false;
}


void ProductTransaction::addOperatorChange(Product::ProductFieldOffsets field, const std::string& UNUSED(ov), const std::string& nv)
{
	switch ( field )
	{
		case Product::fieldOffsetUnitPrice:
		{
			if ( isOperatorChangeType(hfc::OperatorChange::OC_UnitPrice) )
			{
				if ( nv == serializeField(fieldOffsetProductOldUnitPrice) )
					clearOperatorChangeType(hfc::OperatorChange::OC_UnitPrice);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_UnitPrice);

			break;
		}
		case Product::fieldOffsetTareAValue:
		{
			if ( isOperatorChangeType(hfc::OperatorChange::OC_Tare) )
			{
				if ( nv == serializeField(fieldOffsetProductOldTare) )
					clearOperatorChangeType(hfc::OperatorChange::OC_Tare);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_Tare);

			break;
		}
		case Product::fieldOffsetTareBValue:
		{
			if ( isOperatorChangeType(hfc::OperatorChange::OC_TareB) )
			{
				if ( nv == serializeField(fieldOffsetProductOldTare) )
					clearOperatorChangeType(hfc::OperatorChange::OC_Tare);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_TareB);

			break;
		}
		case Product::fieldOffsetByCount:
		{
			if ( isOperatorChangeType(hfc::OperatorChange::OC_ByCount) )
			{
				if ( util_string::stringtolong(nv) == getOldByCount() )
					clearOperatorChangeType(hfc::OperatorChange::OC_ByCount);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_ByCount);
				
			break;
		}
		case Product::fieldOffsetSellByDate:
		{
			HobDateTime dateNow, newDate;
			newDate.setDateParts(nv);

			if ( isOperatorChangeType(hfc::OperatorChange::OC_ShelfLifeDays) )
			{
				if ( newDate.getJulianDay() - dateNow.getJulianDay() == getOldShelfLifeDays() )
					clearOperatorChangeType(hfc::OperatorChange::OC_ShelfLifeDays);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_ShelfLifeDays);
				
			break;
		}
		case Product::fieldOffsetShelfLifeDays:
		{
			if ( isOperatorChangeType(hfc::OperatorChange::OC_ShelfLifeDays) )
			{
				if ( util_string::stringtoint(nv) == getOldShelfLifeDays() )
					clearOperatorChangeType(hfc::OperatorChange::OC_ShelfLifeDays);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_ShelfLifeDays);
				
			break;
		}
		case Product::fieldOffsetBestBeforeDate:
		{
			HobDateTime dateNow, newDate;
			newDate.setDateParts(nv);

			if ( isOperatorChangeType(hfc::OperatorChange::OC_ProductLifeDays) )
			{
				if ( newDate.getJulianDay() - dateNow.getJulianDay() == getOldProductLifeDays() )
					clearOperatorChangeType(hfc::OperatorChange::OC_ProductLifeDays);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_ProductLifeDays);
				
			break;
		}
		case Product::fieldOffsetProductLifeDays:
		{
			if ( isOperatorChangeType(hfc::OperatorChange::OC_ProductLifeDays) )
			{
				if ( util_string::stringtoint(nv) == getOldProductLifeDays() )
					clearOperatorChangeType(hfc::OperatorChange::OC_ProductLifeDays);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_ProductLifeDays);
				
			break;
		}
		case Product::fieldOffsetLabelTypeNum1:
		{
			// TODO - Larry - not sure what the rest of this looks like
			// TODO - Larry - not sure if these cases should be stand alones
			// TODO - Larry - or should they be shoehorned into the regular label cases
			// TODO - Larry - single vector representing the 3 printers is no longer going to work. Vector of Victors?
			if ( isOperatorChangeType(hfc::OperatorChange::OC_PrimaryLabel) )
			{
				if ( util_string::stringtolong(nv) == getOldLabel1() )
					clearOperatorChangeType(hfc::OperatorChange::OC_PrimaryLabel);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_PrimaryLabel);
				
			break;
		}
		case Product::fieldOffsetLabelTypeNum2:
		{
			// TODO - Larry - not sure what the rest of this looks like
			// TODO - Larry - not sure if these cases should be stand alones
			// TODO - Larry - or should they be shoehorned into the regular label cases
			// TODO - Larry - single vector representing the 3 printers is no longer going to work. Vector of Victors?
			if ( isOperatorChangeType(hfc::OperatorChange::OC_SecondLabel) )
			{
				if ( util_string::stringtolong(nv) == getOldLabel2() )
					clearOperatorChangeType(hfc::OperatorChange::OC_SecondLabel);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_SecondLabel);

			break;
		}
		case Product::fieldOffsetFixedWeight:
		{
			if ( isOperatorChangeType(hfc::OperatorChange::OC_FixedWeight) )
			{
				if ( util_string::stringtolong(nv) == getOldFixedWeight() )
					clearOperatorChangeType(hfc::OperatorChange::OC_FixedWeight);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_FixedWeight);
				
			break;
		}

		case Product::fieldOffsetBarcodeProductNum:
		case Product::fieldOffsetBarcodeVendorNum:    // change to the upc digit comes in this field
		{
			if ( isOperatorChangeType(hfc::OperatorChange::OC_UpcDigit) )
			{
				if ( util_string::stringtoint(nv.substr(0,1)) == getOldBarcodeDigit() )
					clearOperatorChangeType(hfc::OperatorChange::OC_UpcDigit);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_UpcDigit);
				
			break;
		}

/*      No ProductFieldOffsets for Total price.
		case xx:                             changedField_ = hfc::OperatorChange::OC_TotalPrice; break; */

		case Product::fieldOffsetGraphicName1:
		{
			if ( isOperatorChangeType(hfc::OperatorChange::OC_Graphic1) )
			{
				if ( nv == getOldGraphicName1() )
					clearOperatorChangeType(hfc::OperatorChange::OC_Graphic1);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_Graphic1);
				
			break;
		}
		case Product::fieldOffsetGraphicName2:
		{
			if ( isOperatorChangeType(hfc::OperatorChange::OC_Graphic2) )
			{
				if ( nv == getOldGraphicName2() )
					clearOperatorChangeType(hfc::OperatorChange::OC_Graphic2);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_Graphic2);
				
			break;
		}
		case Product::fieldOffsetGraphicName3:
		{
			if ( isOperatorChangeType(hfc::OperatorChange::OC_Graphic3) )
			{
				if ( nv == getOldGraphicName3() )
					clearOperatorChangeType(hfc::OperatorChange::OC_Graphic3);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_Graphic3);
				
			break;
		}
		case Product::fieldOffsetGraphicName4:
		{
			if ( isOperatorChangeType(hfc::OperatorChange::OC_Graphic4) )
			{
				if ( nv == getOldGraphicName4() )
					clearOperatorChangeType(hfc::OperatorChange::OC_Graphic4);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_Graphic4);
				
			break;
		}
		case Product::fieldOffsetLabelTypeNum3:
		{
			if ( isOperatorChangeType(hfc::OperatorChange::OC_ThirdLabel) )
			{
				if ( util_string::stringtolong(nv) == getOldLabel3() )
					clearOperatorChangeType(hfc::OperatorChange::OC_ThirdLabel);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_ThirdLabel);
				
			break;
		}

		case Product::fieldOffsetShelfLifeHours:
		{
			if ( isOperatorChangeType(hfc::OperatorChange::OC_ShelfLifeHours) )
			{
				if ( util_string::stringtoint(nv) == getOldShelfLifeHours() )
					clearOperatorChangeType(hfc::OperatorChange::OC_ShelfLifeHours);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_ShelfLifeHours);
				
			break;
		}
		
		case Product::fieldOffsetProductLifeHours:
		{
			if ( isOperatorChangeType(hfc::OperatorChange::OC_ProductLifeHours) )
			{
				if ( util_string::stringtoint(nv) == getOldProductLifeHours() )
					clearOperatorChangeType(hfc::OperatorChange::OC_ProductLifeHours);
			}
			else
				addOperatorChangeType(hfc::OperatorChange::OC_ProductLifeHours);

			break;
		}

		case Product::fieldOffsetShelfLifeMinutes:
		case Product::fieldOffsetProductLifeMinutes:
			// SOF-4904 Add shelfLifeMinutes to product and transactions - not supporting oeprator changes at this time, yet
			break;

		default:
			break;
	}
}


void ProductTransaction::addOperatorChangeTotalPrice(long oldPrice, long newPrice)
{
	if ( isOperatorChangeType(hfc::OperatorChange::OC_TotalPrice) )
	{
		if ( newPrice == getOldFinalPrice() )
			clearOperatorChangeType(hfc::OperatorChange::OC_TotalPrice);
		else
			setFinalPrice(newPrice);
	}
	else
	{
		addOperatorChangeType(hfc::OperatorChange::OC_TotalPrice);
		setFinalPrice(newPrice);
		setOldFinalPrice(oldPrice);
	}
}



HobRecordNum ProductTransaction::getCuttingTestNumber() const
{
	return cuttingTestNumber;
}
void ProductTransaction::setCuttingTestNumber(HobRecordNum number)
{
	cuttingTestNumber = number;
}

bool ProductTransaction::printSecurityLabel() const
{
	Debug::trace("ProductTransaction.SecurityLabel", "printSecurityLabel():  printASecurityLabel = " + booltostring(printASecurityLabel));
	return printASecurityLabel;
}
void ProductTransaction::setPrintSecurityLabel(bool print)
{
	printASecurityLabel = print;
	Debug::trace("ProductTransaction.SecurityLabel", "setPrintSecurityLabel():  printASecurityLabel = " + booltostring(printASecurityLabel));
}

/*  CANADA OVERWAITEA PORTION CONTROL PRICING  */
short ProductTransaction::getPortions() const
{
	Debug::trace("ProductTransaction.Portions", "getPortions() = " + inttostring(portions));
	return portions;
}
void ProductTransaction::setPortions(short value)
{
	portions = value;
	Debug::trace("ProductTransaction.Portions", "setPortions(" + inttostring(portions) + ")");
}

//METSIG??  NO, Not moved to MetSig.cpp because all of the calculations are performed in divideAndRoundPortions(), which was moved to MetSig.
void ProductTransaction::calculateAvgPortionData()
{
	bool traceOnPortions = Debug::isTracing("ProductTransaction.Portions");

	Debug::trace("ProductTransaction", "calculateAvgPortionData()");
	if (traceOnPortions)
		Debug::trace("ProductTransaction.Portions", HobString("calculateAvgPortionData():  Portions = ") + Integer::toString(portions));
	if (portions <= 0)
	{
		if (traceOnPortions)
			Debug::trace("ProductTransaction.Portions", "calculateAvgPortionData():  No calculations will be made.");
		avgPortionPrice = 0;		//	Price per portion
		avgFSPortionPrice = 0;		//	Frequent shopper price per portion
		avgPortionWeight = 0;		//	Price per unit of weight
	}
	else
	{
		HobString msgDebug;
		bool traceOn = Debug::isTracing("ProductTransaction.Portions");
		if (traceOn)
			msgDebug = "calculateAvgPortionData():  ";
		int pricePrecision = Configuration::getInstance()->getPricePrecision();
		int weightPrecision = Configuration::getInstance()->getWeightPrecision();

		//	Calculate avgPortionWeight.
		if (myProduct->isFixedWeight() || myProduct->isFluidOunce())
			avgPortionWeight = getFixedWeight();
		else if (myProduct->isByCount())
			avgPortionWeight = 0;
		else
			avgPortionWeight = getTotalNetWeight();
		if (traceOn)
			Debug::trace("ProductTransaction.Portions", msgDebug + "Step 1: avgPortionWeight = " + Long::toString(avgPortionWeight));

		if (avgPortionWeight > 0)
			avgPortionWeight = MetSig::divideAndRoundPortions(avgPortionWeight, portions, weightPrecision);
		if (traceOn)
			Debug::trace("ProductTransaction.Portions", msgDebug + "Step 2: avgPortionWeight = " + Long::toString(avgPortionWeight));

		//	Calculate avgPortionPrice.
		if (getTotalPrice() > 0)
			avgPortionPrice = MetSig::divideAndRoundPortions(getTotalPrice(), portions, pricePrecision);
		else
			avgPortionPrice = 0;
		if (traceOn)
			Debug::trace("ProductTransaction.Portions", msgDebug + "avgPortionPrice = " + Long::toString(avgPortionPrice));

		//	Calculate avgFSPortionPrice.
		if (getDiscTotalPrice1() > 0)
			avgFSPortionPrice = MetSig::divideAndRoundPortions(getDiscTotalPrice1(), portions, pricePrecision);
		else
			avgFSPortionPrice = 0;
		if (traceOn)
			Debug::trace("ProductTransaction.Portions", msgDebug + "avgPortionPrice = " + Long::toString(avgPortionPrice));
	}
}


HobPrice ProductTransaction::getAvgPortionPrice() const
{
	return avgPortionPrice;
}
HobPrice ProductTransaction::getAvgFreqShopPortionPrice() const
{
	return avgFSPortionPrice;
}
HobWeight ProductTransaction::getAvgPortionWeight() const
{
	return avgPortionWeight;
}


void ProductTransaction::calculatePrintableData(const Product *pProduct)
{
	Debug::trace("ProductTransaction", "calculatePrintableData()");
	calculateFSDiscounts(pProduct);
	calculateAvgPortionData();			//	avgFSPortionPrice is dependent on the FS discount price.
}
