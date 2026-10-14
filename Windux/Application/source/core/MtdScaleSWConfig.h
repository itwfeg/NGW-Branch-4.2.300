/**
 * @file MtdScaleSWConfig.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief HTi serial flash software configuration section
 * Created on: Nov 2, 2012
 */

#ifndef MTDSCALESWCONFIG_H
#define MTDSCALESWCONFIG_H
#include <stdlib.h>
#include "HobString.h"


class MtdScaleSWConfig
{
	public:

	MtdScaleSWConfig();
	MtdScaleSWConfig(const std::string &s);
	MtdScaleSWConfig(MtdScaleSWConfig *pScaleData);
	~MtdScaleSWConfig();

	typedef enum
	{
		XML_SCALEMODEL,
		XML_PRHEADMAIN,
		XML_PERPRECISION,
		XML_MAXUNITPRICE,
		XML_MAXTOTALPRICE,
		XML_MAXLIFEDAYS,
		XML_MAXLIFEHOURS,
		XML_MAXBONUSPOINTS,
		XML_QPOUNDFOR,
		XML_HPOUNDFOR,
		XML_HGRAMSFOR,
		XML_MAXLABELNUM,
		XML_EXPLABELFAM,
		XML_LABELEXPLIMIT,
		XML_SERVICELOGINID,
		XML_SEVICEKEYTIMEOUT,
		XML_RFIDENABLED,
		XML_RFIDDEBOUNCETIME,
		XML_RFIDCOM,
		XML_CONSTATDELAY,
		XML_LOSTPERIPHTIME,
		XML_RANGELIMITAV,
		XML_RANGELIMITMETRIC,
		XML_LABELHORIZOFFSET,
		XML_UPDATEPRCONFIG,
		XML_LABELFEED,
		XML_TESTLABELTYPE,
		XML_INTXFERTIMEOUT,
		XML_BULKXFERTIMEOUT,
		XML_USBENABLEATBOOT,
		XML_SYNCENABLED,
		XML_GOODBEEPENABLED,
		XML_BADBEEPENABLED,
		XML_PINGFREQ,
		XML_PINGATTEMPTS,
		XML_METROLOGICALVERSION,
		XML_POUNDSFOR,
		XML_FRACTIONALPRICING,
		XML_PRIMARYCURRENCYSYMBOL,
		XML_UNITSEPARATOR,
		XML_CURRENCYSYMBOLPLACEMENT,
		XML_TOTALPRICEDIGITS,
		XML_UNITPRICEDIGITS,
		XML_PRIMARYPRECISIONDIGITS,
		XML_CURRENCYROUNDINGMETHOD,
		XML_EndSWElements

	}SW_XML_Elements;

	void setScaleSWConfigDefaults();

	unsigned int getScaleModel() const {return scaleModel_;}
	unsigned int getPrintHeadMaintenance() const {return printHeadMaintenance_;}
	unsigned int getPercentagePrecision() const {return percentagePrecision_;}

	unsigned long getMaxUnitPrice() const {return maxUnitPrice_;}
	unsigned long getMaxTotalPrice() const {return maxTotalPrice_;}
	unsigned short getMaxLifeDays() const {return maxLifeDays_;}
	unsigned short getMaxLifeHours() const {return maxLifeHours_;}
	unsigned long getMaxBonusPoints() const {return maxBonusPoints_;}
	unsigned short getQuarterPoundFor() const {return quarterPoundFor_;}
	unsigned short getHalfPoundFor() const {return halfPoundFor_;}
	unsigned short getHundredGramsFor() const {return hundredGramsFor_;}
	unsigned short getMaxLabelNumber() const {return maxLabelNumber_;}

	unsigned short getShelfLifeExpLabelFamily() const {return shelfLifeExpLabelFamily_;}
	unsigned short getExpirationLabelExpiredLimit() const {return expirationLabelExpiredLimit_;}

	unsigned short getServiceLoginId() const {return serviceLoginId_;}
	unsigned short getServiceKeyRemoveTimeout() const {return serviceKeyRemoveTimeout_;}

	bool getRFIDEnabled() const {return RFIDEnabled_;}
	unsigned short getRFIDdebounceTime() const {return RFIDdebounceTime_;}
	unsigned short getRFIDcomPort() const {return RFIDcomPort_;}

	unsigned short getContinuousStatisticsDelay() const {return continuousStatisticsDelay_;}
	unsigned short getLostPeripheralsCheckTime() const {return lostPeripheralsCheckTime_; }
	unsigned short getDualRangeLimitAvoir() const {return dualRangeLimitAvoir_;}
	unsigned short getDualRangeLimitMetric() const {return dualRangeLimitMetric_;}

	unsigned short getLabelHorizontalOffset() const {return labelHorizontalOffset_;}
	bool getUpdatePrinterConfig() const {return updatePrinterConfig_;}
	unsigned short getLabelFeedHeelFirst() const {return labelFeedHeelFirst_;}
	unsigned short getTestLabelType() const {return testLabelType_;}
	unsigned short getInterruptXferTimeout() const {return interruptXferTimeout_;}
	unsigned short getBulkXferTimeout() const {return bulkXferTimeout_;}

	bool getUsbEnabledAtBootup() const {return usbEnabledAtBootup_;}
	bool getSynchronizationEnabled() const {return synchronizationEnabled_;}
	bool getGoodBeepEnabled() const {return goodBeepEnabled_;}
	bool getbadBeepEnabled() const {return badBeepEnabled_;}

	unsigned short getPingConnectivityFrequency() const {return pingConnectivityFrequency_;}
	unsigned short getPingConnectivityAttempts() const {return pingConnectivityAttempts_;}

	unsigned long getMetrologicalVersion() const {return metroLogicalVersion_;}
	void setMetrologicalVersion(unsigned long version_ ) {metroLogicalVersion_ = version_;}
	void setMetrologicalVersion(std::string version_) {metroLogicalVersion_ = strtol(version_.data(), NULL, 10);}

	bool getPoundsForEnabled() const { return poundsForEnabled_; }
	void setPoundForEnabled(std::string pounds_) { poundsForEnabled_ = (bool)strtol(pounds_.data(), NULL, 10); }

	bool getFractionalPricingEnabled() const { return fractionalPricingEnabled_; }
	void setFractionalPricingEnabled(std::string fractional_) {fractionalPricingEnabled_ = (bool)strtol(fractional_.data(), NULL, 10);}

	std::string getPrimaryCurrencySymbol() const { return primary_currency_symbol_; }
	void setPrimaryCurrencySymbol(std::string symbol) {primary_currency_symbol_ = symbol;}

	unsigned char getUnitSeparator() const { return unit_separator_; }
	void setUnitSeparator(char symbol) {unit_separator_ =  symbol;}

	unsigned short getCurrencySymbolPlacement() const { return currency_symbol_placement_; }
	void setCurrencySymbolPlacement(std::string placement) {currency_symbol_placement_ =  (short)strtol(placement.data(), NULL, 10);}

	unsigned short getTotalPriceDigits() const { return total_price_digits_; }
	void setTotalPriceDigits(std::string digits) {total_price_digits_ =  (short)strtol(digits.data(), NULL, 10);}

	unsigned short getUnitPriceDigits() const { return unit_price_digits_; }
	void setUnitPriceDigits(std::string digits) {unit_price_digits_ =  (short)strtol(digits.data(), NULL, 10);}

	unsigned short getPrimaryPrecisionDigits() const { return primary_precision_digits_; }
	void setPrimaryPrecisionDigits(std::string digits) {primary_precision_digits_ =  (short)strtol(digits.data(), NULL, 10);}

	unsigned short getCurrencyRoundingMethod() const { return currency_rounding_method_; }
	void setCurrencyRoundingMethod(std::string method) {currency_rounding_method_ = (short)strtol(method.data(), NULL, 10);}

	unsigned long getCrc() {return crc_;}

	unsigned long calculateCrc();

	void show();

	const std::string serializeXml() const;

	static int dataOffset();
	static int dataLen();

	// /dev/mtd5
	// this section of flash is 0x40000 bytes
	//   first half is bitmaps
	//     0x10000 for diagnostics bitmap
	//     0x10000 for no-disk bitmap
	//   second half is software config
	static const int SIZEOF_UBOOTBITMAPS = 0x20000;

private:

	static const int SIZEOF_SECTION = 0x20000;
	static const int SIZEOF_SECTION_ARK = 0x10000;

	unsigned int 	scaleModel_;
	unsigned int 	printHeadMaintenance_;		//count for maintenance warning
	unsigned int 	percentagePrecision_;

	unsigned long 	maxUnitPrice_;
	unsigned long 	maxTotalPrice_;
	unsigned short 	maxLifeDays_;
	unsigned short 	maxLifeHours_;
	unsigned long 	maxBonusPoints_;
	unsigned short 	quarterPoundFor_;
	unsigned short 	halfPoundFor_;
	unsigned short 	hundredGramsFor_;
	unsigned short 	maxLabelNumber_;

	unsigned short 	shelfLifeExpLabelFamily_;
	unsigned short 	expirationLabelExpiredLimit_;

	unsigned short 	serviceLoginId_;
	unsigned short 	serviceKeyRemoveTimeout_;

	bool 			RFIDEnabled_;
	unsigned short 	RFIDdebounceTime_;
	unsigned short 	RFIDcomPort_;

	unsigned short 	continuousStatisticsDelay_;
	unsigned short 	lostPeripheralsCheckTime_;

	unsigned short 	dualRangeLimitAvoir_;
	unsigned short 	dualRangeLimitMetric_;

	unsigned short 	labelHorizontalOffset_;
	bool 			updatePrinterConfig_;
	unsigned short 	labelFeedHeelFirst_;
	unsigned short 	testLabelType_;
	unsigned short 	interruptXferTimeout_;
	unsigned short 	bulkXferTimeout_;

	bool 			usbEnabledAtBootup_;
	bool 			synchronizationEnabled_;

	bool			goodBeepEnabled_;
	bool 			badBeepEnabled_;

	unsigned short 	pingConnectivityFrequency_;
	unsigned short 	pingConnectivityAttempts_;

	unsigned long	metroLogicalVersion_;
	bool			poundsForEnabled_;
	bool			fractionalPricingEnabled_;
	
	std::string	primary_currency_symbol_;
	unsigned char	unit_separator_;
	unsigned short 	currency_symbol_placement_;
	unsigned short	total_price_digits_;
	unsigned short	unit_price_digits_;
	unsigned short	primary_precision_digits_;
	unsigned short 	currency_rounding_method_;

	mutable unsigned long crc_;
	mutable std::string serializedData_;

	void init();

};

#endif
