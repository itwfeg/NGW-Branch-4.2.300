#ifndef PRODUCTTRANSACTION_H
#define PRODUCTTRANSACTION_H



#include "Transaction.h"
#include "MetSig.h"

#include "BarCodeNumber.h"
#include "FrequentShopper.h"

class Product;
class HobOperator;
class HfcWeigherStatus;
class ProductTransactionMaster;

#include "HobDateTime.h"



class ProductTransaction : public hfc::Transaction
{
	public:
		/* --- Types and subtypes --- */
		static const short TRANSACTIONSTATE_INITIAL		= 0;
		static const short TRANSACTIONSTATE_PRINTING	= 1;
		static const short TRANSACTIONSTATE_FINAL		= 2;


		/* --- Constructors and destructor --- */
		ProductTransaction(Product * product = 0);
		ProductTransaction(const ProductTransaction & prodTransaction);
		~ProductTransaction();


		/* --- Getters and setters --- */
		HobRecordNum getTransactionNum() const { return transactionNum; }

		short getTransactionState() const { return transactionState; }
		void setTransactionState(short state) { transactionState = state; }

		Product * getProduct() const { return myProduct; }
		void setProduct(Product * product) { myProduct = product; }

		int getByCount() const;        // Overrides method from Transaction

		HobRecordNum getProductNum() const;

		HobPrice getUnitPrice() const; // Overrides method from Transaction

		HobDateTime getShelfLife() const { setShelfLife(); return shelfLife; }

		HobDateTime getProductLife() const { setProductLife(); return productLife; }

		HobDateTime getPackedOn() const { setPackedOn(); return packedOn; }

		const HfcWeigherStatus *getWeigherStatus() const { return weigherStatus; }
		void setWeigherStatus(const HfcWeigherStatus *weigherStatus);

		HobWeight getTotalNetWeight() const { return getFinalNetWeight(); }
		void setTotalNetWeight(HobWeight totalNetWeight) { setFinalNetWeight(totalNetWeight); }

		HobWeight getTotalTare() const { return getFinalTare(); }
		void setTotalTare(HobWeight totalTare) { setFinalTare(totalTare); }

		HobValue64 getTotalPrice() const { return getFinalPrice(); }
		void setTotalPrice(HobValue64 totalPrice) { setFinalPrice(totalPrice); }

		int getReportPageNumber() const { return reportPageNumber; }
		void setReportPageNumber(int page) { reportPageNumber = page; }

		const BarCodeNumber & getBarCodeNum() const { return barCodeNumber; }
		void setBarCodeNum(BarCodeNumber & barCodeNumber);
		void buildBarcodeStrings();

		const FrequentShopper *getFrequentShopperData() const { return frequentShopperData; }
		void setFrequentShopperData(const FrequentShopper &frequentShopper);
		void calculateFSDiscounts(const Product *pProduct);

		HobPrice getDiscTotalPrice1()   const { return frequentShopperData ? frequentShopperData->getTotalPrice1() : 0; }
		HobPrice getDiscTotalPrice2()   const { return frequentShopperData ? frequentShopperData->getTotalPrice2() : 0; }
		HobPrice getDiscTotalPrice3()   const { return frequentShopperData ? frequentShopperData->getTotalPrice3() : 0; }
		HobPrice getDiscTotalSavings1() const { return frequentShopperData ? frequentShopperData->getTotalSavings1() : 0; }
		HobPrice getDiscTotalSavings2() const { return frequentShopperData ? frequentShopperData->getTotalSavings2() : 0; }
		HobPrice getDiscTotalSavings3() const { return frequentShopperData ? frequentShopperData->getTotalSavings3() : 0; }
		HobPrice getDiscUnitPrice1()    const { return frequentShopperData ? frequentShopperData->getUnitPrice1() : 0; }
		HobPrice getDiscUnitPrice2()    const { return frequentShopperData ? frequentShopperData->getUnitPrice2() : 0; }
		HobPrice getDiscUnitPrice3()    const { return frequentShopperData ? frequentShopperData->getUnitPrice3() : 0; }

		bool needsNoWeigherStatus() const;

		void addOperatorChange(Product::ProductFieldOffsets field, const std::string& oldValue, const std::string& newValue);
		void addOperatorChangeTotalPrice(long oldPrice, long newPrice);
		bool hasOperatorChangedData() const { return getOperatorChangeType() != 0; }

		bool doForcePreImage() const { return forcePreImage; }
		void setForcePreImage(bool force) const { forcePreImage = force; }

		int getBackDateDays() const { return backDateDays; }
		void setBackDateDays(int days) { backDateDays = days; }

		bool isProductionPlanningTransaction() const {
			return ((getProductionPlanningType() != Product::PRODUCTIONPLANNING_NO) ||
				(myProduct && myProduct->isMealBuilderItem())); }

		bool isUpdateProductionPlanning() const { return updateProductionPlanning_; }
		void setUpdateProductionPlanning(bool collect) { updateProductionPlanning_ = collect; }

		bool isPrintLabels() const { return printLabels; }
		void setPrintLabels(bool print) { printLabels = print; }

		// Used for Production Entry mode.
		int getNumOfLabelsToPrint() const { return numOfLabelsToPrint; }
		void setNumOfLabelsToPrint(int num) { numOfLabelsToPrint = num; }

		int getNumOfLabelsPrinted() const { return getNumberPackages(); }
		void incrementNumOfLabelsPrinted() { incrementNumberPackages(); }
		void setNumOfLabelsPrinted(int num) { setNumberPackages(num); }

		// Used for Printing and knowing if we printed a Run Totals report.
		bool getPrintedRunTotalsTransLabel() const { return printedRunTotalsTransLabel; }
		void setPrintedRunTotalsTransLabel(bool val) { printedRunTotalsTransLabel = val; }

		HobPrice getFixedWeightUnitPrice() const
				{ return myProduct ? MetSig::calcFixedWeightPricePerUnit(getTotalPrice(), myProduct->getFixedWeight()) : 0; }
		HobPrice getFixedWeightDiscountUnitPrice1() const
				{ return myProduct ? MetSig::calcFixedWeightPricePerUnit(getDiscTotalPrice1(), myProduct->getFixedWeight()) : 0; }
		HobPrice getFixedWeightDiscountUnitPrice2() const
				{ return myProduct ? MetSig::calcFixedWeightPricePerUnit(getDiscTotalPrice2(), myProduct->getFixedWeight()) : 0; }
		HobPrice getFixedWeightDiscountUnitPrice3() const
				{ return myProduct ? MetSig::calcFixedWeightPricePerUnit(getDiscTotalPrice3(), myProduct->getFixedWeight()) : 0; }

		bool operator==(const ProductTransaction & trans) const
				{ return getTransactionNum() == trans.getTransactionNum(); }

		HobDateTime getExpirationLabelDateTime() const { return expirationLabelDateTime; }
		void setExpirationLabelDateTime(const HobDateTime &expDateTime) { expirationLabelDateTime = expDateTime; }
				
		UINT32 getTransactionNumberForBarcode() const { return transactionNumberForBarcode; }
		void setTransactionNumberForBarcode(UINT32 transNum) { transactionNumberForBarcode = transNum; }

		int getTotalItems() const {return totalItems;}		// Used for Accumulated Items
		void setTotalItems(int numOfItems) {totalItems = numOfItems;}
		void incrementTotalItems() {totalItems++;}
		void decrementTotalItems() {totalItems--;}

		ProductTransactionMaster *getProductTransactionMaster() const {return productTransactionMaster;}
		void setProductTransactionMaster(ProductTransactionMaster *ptm) {productTransactionMaster = ptm;}
				
		HobRecordNum getCuttingTestNumber() const;
		void setCuttingTestNumber(HobRecordNum number);

		bool printSecurityLabel() const;
		void setPrintSecurityLabel(bool print);

		short getPortions() const;
		void setPortions(short value);

		void calculatePrintableData(const Product *pProduct);
		/*   Moved to MetSig.cpp */
		void calculateAvgPortionData();

		HobPrice getAvgPortionPrice() const;
		HobPrice getAvgFreqShopPortionPrice() const;
		HobWeight getAvgPortionWeight() const;

	private:

		Product * myProduct;
		HobRecordNum transactionNum;
		short transactionState;
		mutable HobDateTime shelfLife;
		mutable HobDateTime productLife;
		mutable HobDateTime packedOn;
		std::vector<short> labelRotations;
		HfcWeigherStatus *weigherStatus;

		// Page number for reports
		int reportPageNumber;
		BarCodeNumber barCodeNumber;
		FrequentShopper *frequentShopperData;
		mutable bool forcePreImage;
		int backDateDays;		//	Number of days to back date a product. Used in Operate mode only.
		bool updateProductionPlanning_;
		bool printLabels;
		bool printedRunTotalsTransLabel;
		int numOfLabelsToPrint;		// Used for Production Entry mode.

		mutable HobDateTime expirationLabelDateTime;
		
		UINT32 transactionNumberForBarcode;
		int totalItems;		// Used for Accumulated Items
		ProductTransactionMaster *productTransactionMaster;
		
		HobRecordNum	cuttingTestNumber;

		bool printASecurityLabel;

		short portions;					//	Portions per package
		HobPrice avgPortionPrice;		//	Price per portion
		HobPrice avgFSPortionPrice;		//	Frequent shopper price per portion
		HobWeight avgPortionWeight;		//	Price per unit of weight

		/* --- Instance function members --- */
		void init();
		// Not your typical setter, since it is set based on the shelf life days and hours.
		void setShelfLife() const;
		// Not your typical setter, since it is set based on the product life days and hours.
		void setProductLife() const;
		// Not your typical setter, since it is set based on the current date and time.
		void setPackedOn() const;


		/* --- Class data members --- */
		static HobRecordNum nextTransactionNum;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif //PRODUCTTRANSACTION_H
