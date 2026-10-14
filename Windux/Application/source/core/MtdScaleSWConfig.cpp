/**
 * @file MtdScaleSWConfig.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief HTi serial flash software configuration section
 * Created on: Nov 2, 2012
 */

#include "MtdScaleSWConfig.h"
#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Debug.h"

MtdScaleSWConfig::MtdScaleSWConfig()
{
	init();
}

MtdScaleSWConfig::MtdScaleSWConfig( const std::string &s )
{
	std::string startTag( "<ScaleCFGSection>" );
	std::string endTag( "</ScaleCFGSection>" );

	init();

	// calculate the crc value of the section data
	std::size_t crcPos;

	if ( std::string::npos != (crcPos = s.find( startTag )) )
	{
		std::size_t crcEndPos( s.find( endTag, crcPos ) );

		if ( std::string::npos != crcEndPos )
		{
			const unsigned char *pSbuffer( &reinterpret_cast<const unsigned char*>( s.c_str() )[crcPos] );

			crcEndPos += endTag.size(); // include the section end tag into our calculation
			crcEndPos -= crcPos;

			crc_ = 0;

			while ( crcEndPos-- )
				crc_ += *pSbuffer++;
		}
	}

	// parse the xml data from the serial flash
	for( int i = 0; i < MtdScaleSWConfig::XML_EndSWElements; i++ )
	{
		switch(i)
		{
			case MtdScaleSWConfig::XML_SCALEMODEL:
			{
				startTag = 	"<Model>";
				endTag = 	"</Model>";
				break;
			}
			case MtdScaleSWConfig::XML_PRHEADMAIN:
			{
				startTag = 	"<PHMain>";
				endTag = 	"</PHMain>";
				break;
			}
			case MtdScaleSWConfig::XML_PERPRECISION:
			{
				startTag = 	"<Prec>";
				endTag = 	"</Prec>";
				break;
			}
			case MtdScaleSWConfig::XML_MAXUNITPRICE:
			{
				startTag = 	"<MaxUPrice>";
				endTag = 	"</MaxUPrice>";
				break;
			}
			case MtdScaleSWConfig::XML_MAXTOTALPRICE:
			{
				startTag = 	"<MaxTPrice>";
				endTag = 	"</MaxTPrice>";
				break;
			}
			case MtdScaleSWConfig::XML_MAXLIFEDAYS:
			{
				startTag = 	"<MaxLDays>";
				endTag = 	"</MaxLDays>";
				break;
			}
			case MtdScaleSWConfig::XML_MAXLIFEHOURS:
			{
				startTag = 	"<MaxLHour>";
				endTag = 	"</MaxLHour>";
				break;
			}
			case MtdScaleSWConfig::XML_MAXBONUSPOINTS:
			{
				startTag = 	"<BonusP>";
				endTag = 	"</BonusP>";
				break;
			}
			case MtdScaleSWConfig::XML_QPOUNDFOR:
			{
				startTag = 	"<QuarterP>";
				endTag = 	"</QuarterP>";
				break;
			}
			case MtdScaleSWConfig::XML_HPOUNDFOR:
			{
				startTag = 	"<HalfP>";
				endTag = 	"</HalfP>";
				break;
			}
			case MtdScaleSWConfig::XML_HGRAMSFOR:
			{
				startTag = 	"<GramsFor>";
				endTag = 	"</GramsFor>";
				break;
			}
			case MtdScaleSWConfig::XML_MAXLABELNUM:
			{
				startTag = 	"<MaxLabel>";
				endTag = 	"</MaxLabel>";
				break;
			}
			case MtdScaleSWConfig::XML_EXPLABELFAM:
			{
				startTag = 	"<ExpLabelFam>";
				endTag = 	"</ExpLabelFam>";
				break;
			}
			case MtdScaleSWConfig::XML_LABELEXPLIMIT:
			{
				startTag = 	"<ExpLabelExp>";
				endTag = 	"</ExpLabelExp>";
				break;
			}
			case MtdScaleSWConfig::XML_SERVICELOGINID:
			{
				startTag = 	"<SLoginId>";
				endTag = 	"</SLoginId>";
				break;
			}
			case MtdScaleSWConfig::XML_SEVICEKEYTIMEOUT:
			{
				startTag = 	"<SKeyTime>";
				endTag = 	"</SKeyTime>";
				break;
			}
			case MtdScaleSWConfig::XML_RFIDENABLED:
			{
				startTag = 	"<RFIDEn>";
				endTag = 	"</RFIDEn>";
				break;
			}
			case MtdScaleSWConfig::XML_RFIDDEBOUNCETIME:
			{
				startTag = 	"<RFIDdeb>";
				endTag = 	"</RFIDdeb>";
				break;
			}
			case MtdScaleSWConfig::XML_RFIDCOM:
			{
				startTag = 	"<RFIDcom>";
				endTag = 	"</RFIDcom>";
				break;
			}
			case MtdScaleSWConfig::XML_CONSTATDELAY:
			{
				startTag = 	"<ConStatDelay>";
				endTag = 	"</ConStatDelay>";
				break;
			}
			case MtdScaleSWConfig::XML_LOSTPERIPHTIME:
			{
				startTag = 	"<LostPer>";
				endTag = 	"</LostPer>";
				break;
			}
			case MtdScaleSWConfig::XML_RANGELIMITAV:
			{
				startTag = 	"<Avoir>";
				endTag = 	"</Avoir>";
				break;
			}
			case MtdScaleSWConfig::XML_RANGELIMITMETRIC:
			{
				startTag = 	"<Metric>";
				endTag = 	"</Metric>";
				break;
			}
			case MtdScaleSWConfig::XML_LABELHORIZOFFSET:
			{
				startTag = 	"<LbHOff>";
				endTag = 	"</LbHOff>";
				break;
			}
			case MtdScaleSWConfig::XML_UPDATEPRCONFIG:
			{
				startTag = 	"<UPConfig>";
				endTag = 	"</UPConfig>";
				break;
			}
			case MtdScaleSWConfig::XML_LABELFEED:
			{
				startTag = 	"<Heel>";
				endTag = 	"</Heel>";
				break;
			}
			case MtdScaleSWConfig::XML_TESTLABELTYPE:
			{
				startTag = 	"<TestLab>";
				endTag = 	"</TestLab>";
				break;
			}
			case MtdScaleSWConfig::XML_INTXFERTIMEOUT:
			{
				startTag = 	"<IXferT>";
				endTag = 	"</IXferT>";
				break;
			}
			case MtdScaleSWConfig::XML_BULKXFERTIMEOUT:
			{
				startTag = 	"<BXferT>";
				endTag = 	"</BXferT>";
				break;
			}
			case MtdScaleSWConfig::XML_USBENABLEATBOOT:
			{
				startTag = 	"<USBEn>";
				endTag = 	"</USBEn>";
				break;
			}
			case MtdScaleSWConfig::XML_SYNCENABLED:
			{
				startTag = 	"<SyncEn>";
				endTag = 	"</SyncEn>";
				break;
			}
			case MtdScaleSWConfig::XML_GOODBEEPENABLED:
			{
				startTag = 	"<GBeepEn>";
				endTag = 	"</GBeepEn>";
				break;
			}
			case MtdScaleSWConfig::XML_BADBEEPENABLED:
			{
				startTag = 	"<BBeepEn>";
				endTag = 	"</BBeepEn>";
				break;
			}
			case MtdScaleSWConfig::XML_PINGFREQ:
			{
				startTag = 	"<PingFreq>";
				endTag = 	"</PingFreq>";
				break;
			}
			case MtdScaleSWConfig::XML_PINGATTEMPTS:
			{
				startTag = 	"<PingAtt>";
				endTag = 	"</PingAtt>";
				break;
			}
			case MtdScaleSWConfig::XML_METROLOGICALVERSION:
			{
				startTag = 	"<MetroVersion>";
				endTag = 	"</MetroVersion>";
				break;
			}
			case XML_POUNDSFOR:
			{
				startTag = 	"<PoundsFor>";
				endTag = 	"</PoundsFor>";
				break;
			}
			case XML_FRACTIONALPRICING:
			{
				startTag = 	"<FractionalPrice>";
				endTag = 	"</FractionalPrice>";
				break;
			}
			case XML_PRIMARYCURRENCYSYMBOL:
			{
				startTag = 	"<PrimaryCurrencySymbol>";
				endTag = 	"</PrimaryCurrencySymbol>";
				break;
			}
			case XML_UNITSEPARATOR:
			{
				startTag = 	"<UnitSeparator>";
				endTag = 	"</UnitSeparator>";
				break;
			}
			case XML_CURRENCYSYMBOLPLACEMENT:
			{
				startTag = 	"<CurrencySymbolPlacement>";
				endTag = 	"</CurrencySymbolPlacement>";
				break;
			}
			case XML_TOTALPRICEDIGITS:
			{
				startTag = 	"<TotalPriceDigits>";
				endTag = 	"</TotalPriceDigits>";
				break;
			}
			case XML_UNITPRICEDIGITS:
			{
				startTag = 	"<UnitPriceDigits>";
				endTag = 	"</UnitPriceDigits>";
				break;
			}
			case XML_PRIMARYPRECISIONDIGITS:
			{
				startTag = 	"<PrimaryPrecisionDigits>";
				endTag = 	"</PrimaryPrecisionDigits>";
				break;
			}
			case XML_CURRENCYROUNDINGMETHOD:
			{
				startTag = 	"<CurrencyRoundingMethod>";
				endTag = 	"</CurrencyRoundingMethod>";
				break;
			}
			default:
			{
				Debug::trace("MtdScaleSWConfig", "i = " + Integer::toString(i));
				startTag = "";
				break;
			}
		}

		if(startTag.size())
		{
			std::size_t spos( s.find(startTag) );
			std::size_t epos( s.find(endTag) );

			if (( std::string::npos != spos ) && ( std::string::npos != epos ))
			{
				spos += startTag.size();

				if ( spos < epos )
				{
					const std::string t( s.substr(spos, epos-spos) );

					if( startTag == "<Model>" )
						scaleModel_ = strtol(t.data(), NULL, 10);
					else if( startTag == "<PHMain>" )
						printHeadMaintenance_ = strtol(t.data(), NULL, 10);
					else if(startTag == "<Prec>")
						percentagePrecision_ = strtol(t.data(), NULL, 10);
					else if(startTag == "<MaxUPrice>")
						maxUnitPrice_ = strtol(t.data(), NULL, 10);
					else if(startTag == "<MaxTPrice>")
						maxTotalPrice_ = strtol(t.data(), NULL, 10);
					else if(startTag == "<MaxLDays>")
						maxLifeDays_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<MaxLHour>")
						maxLifeHours_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<BonusP>")
						maxBonusPoints_ = strtol(t.data(), NULL, 10);
					else if(startTag == "<QuarterP>")
						quarterPoundFor_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<HalfP>")
						halfPoundFor_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<GramsFor>")
						hundredGramsFor_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<MaxLabel>")
						maxLabelNumber_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<ExpLabelFam>")
						shelfLifeExpLabelFamily_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<ExpLabelExp>")
						expirationLabelExpiredLimit_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<SLoginId>")
						serviceLoginId_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<SKeyTime>")
						serviceKeyRemoveTimeout_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<RFIDEn>")
						RFIDEnabled_ = strtol(t.data(), NULL, 10);
					else if(startTag == "<RFIDdeb>")
						RFIDdebounceTime_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<RFIDcom>")
						RFIDcomPort_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<ConStatDelay>")
						continuousStatisticsDelay_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<LostPer>")
						lostPeripheralsCheckTime_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<Avoir>")
						dualRangeLimitAvoir_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<Metric>")
						dualRangeLimitMetric_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<LbHOff>")
						labelHorizontalOffset_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<UPConfig>")
						updatePrinterConfig_ = strtol(t.data(), NULL, 10);
					else if(startTag == "<Heel>")
						labelFeedHeelFirst_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<TestLab>")
						testLabelType_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<IXferT>")
						interruptXferTimeout_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<BXferT>")
						bulkXferTimeout_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<USBEn>")
						usbEnabledAtBootup_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag == "<SyncEn>")
						synchronizationEnabled_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag == "<GBeepEn>")
						goodBeepEnabled_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag == "<BBeepEn>")
						badBeepEnabled_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag == "<PingFreq>")
						pingConnectivityFrequency_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag == "<PingAtt>")
						pingConnectivityAttempts_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag ==  "<MetroVersion>")
						metroLogicalVersion_  = strtol(t.data(), NULL, 10);
					else if(startTag ==	"<PoundsFor>")
						poundsForEnabled_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag ==	"<FractionalPrice>" )
						fractionalPricingEnabled_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag ==	"<PrimaryCurrencySymbol>" )
						primary_currency_symbol_ = t;//(bool)strtol(t.data(), NULL, 10);
					else if(startTag ==	"<UnitSeparator>" )
						unit_separator_ = (unsigned char)strtol(t.data(), NULL, 10);
					else if(startTag ==	"<CurrencySymbolPlacement>" )
						currency_symbol_placement_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag ==	"<TotalPriceDigits>" )
						total_price_digits_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag ==	"<UnitPriceDigits>" )
						unit_price_digits_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag ==	"<PrimaryPrecisionDigits>" )
						primary_precision_digits_ = (unsigned short) strtol(t.data(), NULL, 10);
					else if(startTag ==	"<CurrencyRoundingMethod>" )
						currency_rounding_method_ = (unsigned short) strtol(t.data(), NULL, 10);

				}
			}
			else
			{
				Debug::trace("MtdScaleSWConfig", "Starting tag could not be found!" + s);
			}
		}
		else
		{
			Debug::trace("MtdScaleSWConfig", "Unrecognized case!");
		}
	}
}

MtdScaleSWConfig::~MtdScaleSWConfig()
{


}

MtdScaleSWConfig::MtdScaleSWConfig(MtdScaleSWConfig *pScaleData)
{
	scaleModel_					= pScaleData->getScaleModel();
	printHeadMaintenance_		= pScaleData->getPrintHeadMaintenance();
	percentagePrecision_		= pScaleData->getPercentagePrecision();

	maxUnitPrice_				= pScaleData->getMaxUnitPrice();
	maxTotalPrice_				= pScaleData->getMaxTotalPrice();
	maxLifeDays_				= pScaleData->getMaxLifeDays();
	maxLifeHours_				= pScaleData->getMaxLifeHours();
	maxBonusPoints_				= pScaleData->getMaxBonusPoints();
	quarterPoundFor_			= pScaleData->getQuarterPoundFor();
	halfPoundFor_				= pScaleData->getHalfPoundFor();
	hundredGramsFor_			= pScaleData->getHundredGramsFor();
	maxLabelNumber_				= pScaleData->getMaxLabelNumber();

	shelfLifeExpLabelFamily_	= pScaleData->getShelfLifeExpLabelFamily();
	expirationLabelExpiredLimit_= pScaleData->getExpirationLabelExpiredLimit();

	serviceLoginId_				= pScaleData->getServiceLoginId();
	serviceKeyRemoveTimeout_	= pScaleData->getServiceKeyRemoveTimeout();

	RFIDEnabled_				= pScaleData->getRFIDEnabled();
	RFIDdebounceTime_			= pScaleData->getRFIDdebounceTime();
	RFIDcomPort_				= pScaleData->getRFIDcomPort();

	continuousStatisticsDelay_	= pScaleData->getContinuousStatisticsDelay();
	lostPeripheralsCheckTime_	= pScaleData->getLostPeripheralsCheckTime();

	dualRangeLimitAvoir_		= pScaleData->getDualRangeLimitAvoir();
	dualRangeLimitMetric_		= pScaleData->getDualRangeLimitMetric();

	labelHorizontalOffset_		= pScaleData->getLabelHorizontalOffset();
	updatePrinterConfig_		= pScaleData->getUpdatePrinterConfig();
	labelFeedHeelFirst_			= pScaleData->getLabelFeedHeelFirst();
	testLabelType_				= pScaleData->getTestLabelType();
	interruptXferTimeout_		= pScaleData->getInterruptXferTimeout();
	bulkXferTimeout_			= pScaleData->getBulkXferTimeout();

	usbEnabledAtBootup_			= pScaleData->getUsbEnabledAtBootup();
	synchronizationEnabled_		= pScaleData->getSynchronizationEnabled();

	goodBeepEnabled_			= pScaleData->getGoodBeepEnabled();
	badBeepEnabled_				= pScaleData->getbadBeepEnabled();

	pingConnectivityFrequency_	= pScaleData->getPingConnectivityFrequency();
	pingConnectivityAttempts_	= pScaleData->getPingConnectivityAttempts();

	metroLogicalVersion_ 		= pScaleData->getMetrologicalVersion();
	poundsForEnabled_			= pScaleData->getPoundsForEnabled();
	fractionalPricingEnabled_ 	= pScaleData->getFractionalPricingEnabled();

	primary_currency_symbol_	= pScaleData->getPrimaryCurrencySymbol();
	unit_separator_				= pScaleData->getUnitSeparator();
	currency_symbol_placement_	= pScaleData->getCurrencySymbolPlacement();
	total_price_digits_			= pScaleData->getTotalPriceDigits();
	unit_price_digits_			= pScaleData->getUnitPriceDigits();
	primary_precision_digits_	= pScaleData->getPrimaryPrecisionDigits();
	currency_rounding_method_	= pScaleData->getCurrencyRoundingMethod();


	crc_						= pScaleData->getCrc();

}

void MtdScaleSWConfig::init()
{
	scaleModel_ 			= 0;
	printHeadMaintenance_ 	= 16000;		//count for maintenance warning.  1 case of 3" labels assumed.
	percentagePrecision_ 	= 0;

	maxUnitPrice_ 	= 0;
	maxTotalPrice_	= 0;
	maxLifeDays_ 	= 0;
	maxLifeHours_ 	= 0;
	maxBonusPoints_ = 0;
	quarterPoundFor_= 0;
	halfPoundFor_ 	= 0;
	hundredGramsFor_= 0;
	maxLabelNumber_ = 0;

	shelfLifeExpLabelFamily_ 	= 0;
	expirationLabelExpiredLimit_= 0;

	serviceLoginId_ 			= 0;
	serviceKeyRemoveTimeout_ 	= 0;

	RFIDEnabled_ 		= false;
	RFIDdebounceTime_ 	= 0;
	RFIDcomPort_ 		= 0;

	continuousStatisticsDelay_ 	= 0;
	lostPeripheralsCheckTime_ 	= 0;

	dualRangeLimitAvoir_ 	= 0;
	dualRangeLimitMetric_	= 0;

	labelHorizontalOffset_ 	= 0;
	updatePrinterConfig_ 	= false;
	labelFeedHeelFirst_ 	= 0;
	testLabelType_ 			= 0;
	interruptXferTimeout_ 	= 0;
	bulkXferTimeout_ 		= 0;

	usbEnabledAtBootup_ 	= false;
	synchronizationEnabled_ = false;

	goodBeepEnabled_ 	= false;
	badBeepEnabled_ 	= false;

	pingConnectivityFrequency_ 	= 0;
	pingConnectivityAttempts_ 	= 0;

	metroLogicalVersion_ = 0;

	poundsForEnabled_ = false;
	fractionalPricingEnabled_ = false;

	primary_currency_symbol_	= "";
	unit_separator_				= '.';
	currency_symbol_placement_	= 0;
	total_price_digits_			= 0;
	unit_price_digits_			= 0;
	primary_precision_digits_	= 0;
	currency_rounding_method_	= 0;

	crc_ = 0;
}

unsigned long MtdScaleSWConfig::calculateCrc()
{
	crc_ = 0;
	serializeXml();
	return crc_;
}

const std::string MtdScaleSWConfig::serializeXml() const
{
	std::string endTag = "</ScaleCFGSection>";
	serializedData_.clear();
	crc_ = 0;

	serializedData_ = "<ScaleCFGSection>";
	serializedData_ += "<Model>" + Integer::toString(scaleModel_) + "</Model>" + "<PHMain>" +  Integer::toString(printHeadMaintenance_) + "</PHMain>" +
	"<Prec>" + Integer::toString(percentagePrecision_) + "</Prec>" + "<MaxUPrice>" + Integer::toString(maxUnitPrice_) + "</MaxUPrice>"	+
	"<MaxTPrice>" + Integer::toString(maxTotalPrice_) + "</MaxTPrice>" + "<MaxLDays>" + Integer::toString(maxLifeDays_) + "</MaxLDays>" + "<MaxLHour>" +
	Integer::toString(maxLifeHours_) + "</MaxLHour>" + "<BonusP>" + Integer::toString(maxBonusPoints_) + "</BonusP>" + "<QuarterP>" +
	Integer::toString(quarterPoundFor_) + "</QuarterP>" + "<HalfP>" + Integer::toString(halfPoundFor_) + "</HalfP>" + "<GramsFor>" +
	Integer::toString(hundredGramsFor_) + "</GramsFor>" + "<MaxLabel>" + Integer::toString(maxLabelNumber_) + "</MaxLabel>" +
	"<ExpLabelFam>" + Integer::toString(shelfLifeExpLabelFamily_) + "</ExpLabelFam>" + "<ExpLabelExp>" + Integer::toString(expirationLabelExpiredLimit_) +
	"</ExpLabelExp>" + "<SLoginId>" + Integer::toString(serviceLoginId_) + "</SLoginId>" + "<SKeyTime>" + Integer::toString(serviceKeyRemoveTimeout_) +
	"</SKeyTime>" + "<RFIDEn>" + Bool::toString(RFIDEnabled_) + "</RFIDEn>" + "<RFIDdeb>" + Integer::toString(RFIDdebounceTime_) + "</RFIDdeb>" +
	"<RFIDcom>" + Integer::toString(RFIDcomPort_) + "</RFIDcom>" + "<ConStatDelay>" + Integer::toString(continuousStatisticsDelay_) + "</ConStatDelay>" +
	"<LostPer>" + Integer::toString(lostPeripheralsCheckTime_) + "</LostPer>" + "<Avoir>" + Integer::toString(dualRangeLimitAvoir_) + "</Avoir>" +
	"<Metric>" + Integer::toString(dualRangeLimitMetric_) + "</Metric>" + "<LbHOff>" + Integer::toString(labelHorizontalOffset_) + "</LbHOff>" +
	"<UPConfig>" + Integer::toString(updatePrinterConfig_) + "</UPConfig>" + "<Heel>" + Integer::toString(labelFeedHeelFirst_) + "</Heel>" +
	"<TestLab>" + Integer::toString(testLabelType_) + "</TestLab>" + "<IXferT>" + Integer::toString(interruptXferTimeout_) + "</IXferT>" +
	"<BXferT>" + Integer::toString(bulkXferTimeout_) + "</BXferT>" + "<USBEn>" + Bool::toString(usbEnabledAtBootup_) + "</USBEn>" +
	"<SyncEn>" + Integer::toString(synchronizationEnabled_) + "</SyncEn>" + "<GBeepEn>" + Bool::toString(goodBeepEnabled_) +
	"</GBeepEn>" + "<BBeepEn>" + Bool::toString(badBeepEnabled_) + "</BBeepEn>" + "<PingFreq>" + Integer::toString(pingConnectivityFrequency_) +
	"</PingFreq>" + "<PingAtt>" + Integer::toString(pingConnectivityAttempts_) + "</PingAtt>" +
	"<MetroVersion>" + Long::toString(metroLogicalVersion_) + "</MetroVersion>" + "<PoundsFor>" + Bool::toString(poundsForEnabled_) + "</PoundsFor>" +
	"<FractionalPrice>" + Bool::toString(fractionalPricingEnabled_) + "</FractionalPrice>" + 
	"<PrimaryCurrencySymbol>" + primary_currency_symbol_ + "</PrimaryCurrencySymbol>" + 
	"<UnitSeparator>" + Integer::toString((int)unit_separator_) + "</UnitSeparator>" + 
	"<CurrencySymbolPlacement>" + Integer::toString(currency_symbol_placement_) + "</CurrencySymbolPlacement>" + 
	"<TotalPriceDigits>" + Integer::toString(total_price_digits_) + "</TotalPriceDigits>" + 
	"<UnitPriceDigits>" + Integer::toString(unit_price_digits_) + "</UnitPriceDigits>" + 
	"<PrimaryPrecisionDigits>" + Integer::toString(primary_precision_digits_) + "</PrimaryPrecisionDigits>" + 
	"<CurrencyRoundingMethod>" + Integer::toString(currency_rounding_method_) + "</CurrencyRoundingMethod>" + 
	"</ScaleCFGSection>";


	std::size_t crcEndPos = serializedData_.find(endTag);	/*chop off the junk at the end of the string */
	crcEndPos += endTag.size();								/*include the section end tag into our calculation*/

	/*calculate our crc value for the serialized data*/
	const unsigned char *pSbuffer = (const unsigned char *)serializedData_.data();
	for( int i = 0; i < crcEndPos; i++ )
	{
		crc_ +=	*pSbuffer++;
	}

	Debug::trace("MtdScaleSWConfig", "serializeXml() Crc data length: " + Integer::toString(crcEndPos));
	Debug::trace("MtdScaleSWConfig", "serializeXml() Crc: " + Integer::toString(crc_));
	Debug::trace("MtdScaleSWConfig", "serializeXml() Crc data: " + serializedData_);
	return serializedData_;
}

void MtdScaleSWConfig::setScaleSWConfigDefaults()
{
	/*!set the scale's software configuration to it's default values */

	scaleModel_ 			= 2;
	printHeadMaintenance_ 	= 16000;		//TBD count for maintenance warning
	percentagePrecision_ 	= 2;

	maxUnitPrice_ 		= 99999;
	maxTotalPrice_ 		= 99999;
	maxLifeDays_ 		= 998;
	maxLifeHours_ 		= 23;
	maxBonusPoints_ 	= 9999;
	quarterPoundFor_ 	= 25;
	halfPoundFor_ 		= 50;
	hundredGramsFor_ 	= 100;
	maxLabelNumber_ 	= 9999;

	shelfLifeExpLabelFamily_ 		= 9901;
	expirationLabelExpiredLimit_ 	= 2;

	serviceLoginId_ 				= 7776;
	serviceKeyRemoveTimeout_ 		= 30;

	RFIDEnabled_ 		= true;
	RFIDdebounceTime_ 	= 5;
	RFIDcomPort_ 		= 2;

	continuousStatisticsDelay_ 	= 15;
	lostPeripheralsCheckTime_ 	= 30;

	dualRangeLimitAvoir_ 	= 12000;
	dualRangeLimitMetric_ 	= 6000;

	labelHorizontalOffset_ 	= 0;
	updatePrinterConfig_ 	= false;
	labelFeedHeelFirst_ 	= 1;
	testLabelType_ 			= 2;
	interruptXferTimeout_ 	= 1000;
	bulkXferTimeout_ 		= 1000;

	usbEnabledAtBootup_ 		= true;
	synchronizationEnabled_ 	= true;

	goodBeepEnabled_	 		= false;
	badBeepEnabled_ 			= false;

	pingConnectivityFrequency_ 	= 3;
	pingConnectivityAttempts_ 	= 1;

	metroLogicalVersion_ = 0;
	poundsForEnabled_ = false;
	fractionalPricingEnabled_ = false;

	primary_currency_symbol_	= "";
	unit_separator_				= 0;
	currency_symbol_placement_	= 0;
	total_price_digits_			= 0;
	unit_price_digits_			= 0;
	primary_precision_digits_	= 0;
	currency_rounding_method_	= 0;
}

void MtdScaleSWConfig::show()
{
	Debug::trace("MtdScaleSWConfig", "scaleModel_ " + Integer::toString(scaleModel_));
	Debug::trace("MtdScaleSWConfig", "printHeadMaintenance_ " + Integer::toString(printHeadMaintenance_));
	Debug::trace("MtdScaleSWConfig", "percentagePrecision_ " + Integer::toString(percentagePrecision_));
	Debug::trace("MtdScaleSWConfig", "maxUnitPrice_ " + Integer::toString(maxUnitPrice_));
	Debug::trace("MtdScaleSWConfig", "maxTotalPrice_ " + Integer::toString(maxTotalPrice_));
	Debug::trace("MtdScaleSWConfig", "maxLifeDays_ " + Integer::toString(maxLifeDays_));
	Debug::trace("MtdScaleSWConfig", "maxLifeHours_ " + Integer::toString(maxLifeHours_));
	Debug::trace("MtdScaleSWConfig", "maxBonusPoints_ " + Integer::toString(maxBonusPoints_));
	Debug::trace("MtdScaleSWConfig", "quarterPoundFor_ " + Integer::toString(quarterPoundFor_));
	Debug::trace("MtdScaleSWConfig", "halfPoundFor_ " + Integer::toString(halfPoundFor_));
	Debug::trace("MtdScaleSWConfig", "hundredGramsFor_ " + Integer::toString(hundredGramsFor_));
	Debug::trace("MtdScaleSWConfig", "maxLabelNumber_ " + Integer::toString(maxLabelNumber_));
	Debug::trace("MtdScaleSWConfig", "shelfLifeExpLabelFamily_ " + Integer::toString(shelfLifeExpLabelFamily_));
	Debug::trace("MtdScaleSWConfig", "expirationLabelExpiredLimit_ " + Integer::toString(expirationLabelExpiredLimit_));
	Debug::trace("MtdScaleSWConfig", "serviceLoginId_ " + Integer::toString(serviceLoginId_));
	Debug::trace("MtdScaleSWConfig", "serviceKeyRemoveTimeout_ " + Integer::toString(serviceKeyRemoveTimeout_));
	Debug::trace("MtdScaleSWConfig", "RFIDEnabled_ " + Integer::toString(RFIDEnabled_));
	Debug::trace("MtdScaleSWConfig", "RFIDdebounceTime_ " + Integer::toString(RFIDdebounceTime_));
	Debug::trace("MtdScaleSWConfig", "RFIDcomPort_ " + Integer::toString(RFIDcomPort_));
	Debug::trace("MtdScaleSWConfig", "continuousStatisticsDelay_ " + Integer::toString(continuousStatisticsDelay_));
	Debug::trace("MtdScaleSWConfig", "lostPeripheralsCheckTime_ " + Integer::toString(lostPeripheralsCheckTime_));
	Debug::trace("MtdScaleSWConfig", "dualRangeLimitAvoir_ " + Integer::toString(dualRangeLimitAvoir_));
	Debug::trace("MtdScaleSWConfig", "dualRangeLimitMetric_ " + Integer::toString(dualRangeLimitMetric_));
	Debug::trace("MtdScaleSWConfig", "labelHorizontalOffset_ " + Integer::toString(labelHorizontalOffset_));
	Debug::trace("MtdScaleSWConfig", "updatePrinterConfig_ " + Integer::toString(updatePrinterConfig_));
	Debug::trace("MtdScaleSWConfig", "labelFeedHeelFirst_ " + Integer::toString(labelFeedHeelFirst_));
	Debug::trace("MtdScaleSWConfig", "testLabelType_ " + Integer::toString(testLabelType_));
	Debug::trace("MtdScaleSWConfig", "interruptXferTimeout_ " + Integer::toString(interruptXferTimeout_));
	Debug::trace("MtdScaleSWConfig", "bulkXferTimeout_  " + Integer::toString(bulkXferTimeout_));
	Debug::trace("MtdScaleSWConfig", "usbEnabledAtBootup_ " + Integer::toString(usbEnabledAtBootup_));
	Debug::trace("MtdScaleSWConfig", "synchronizationEnabled_ " + Integer::toString(synchronizationEnabled_));
	Debug::trace("MtdScaleSWConfig", "goodBeepEnabled_ " + Integer::toString(goodBeepEnabled_));
	Debug::trace("MtdScaleSWConfig", "badBeepEnabled_ " + Integer::toString(badBeepEnabled_));
	Debug::trace("MtdScaleSWConfig", "pingConnectivityFrequency_ " + Integer::toString(pingConnectivityFrequency_));
	Debug::trace("MtdScaleSWConfig", "pingConnectivityAttempts_ " + Integer::toString(pingConnectivityAttempts_));
	Debug::trace("MtdScaleSWConfig", "metroLogicalVersion_ " + Long::toString(metroLogicalVersion_));
	Debug::trace("MtdScaleSWConfig", "poundsForEnabled_" + Integer::toString(poundsForEnabled_) );
	Debug::trace("MtdScaleSWConfig", "fractionalPricingEnabled_" + Integer::toString(fractionalPricingEnabled_));
	Debug::trace("MtdScaleSWConfig", "primary_currency_symbol_" + primary_currency_symbol_);
	Debug::trace("MtdScaleSWConfig", "unit_separator_" + Integer::toString(unit_separator_));
	Debug::trace("MtdScaleSWConfig", "currency_symbol_placement_" + Integer::toString(currency_symbol_placement_));
	Debug::trace("MtdScaleSWConfig", "total_price_digits_" + Integer::toString(total_price_digits_));
	Debug::trace("MtdScaleSWConfig", "unit_price_digits_" + Integer::toString(unit_price_digits_));
	Debug::trace("MtdScaleSWConfig", "primary_precision_digits_" + Integer::toString(primary_precision_digits_));
	Debug::trace("MtdScaleSWConfig", "currency_rounding_method_" + Integer::toString(currency_rounding_method_));

	//return SIZEOF_UBOOTBITMAPS;
}

int MtdScaleSWConfig::dataOffset()
{
	struct stat buffer;

	// ARK
	// we have dedicated partition in ARK so check for exist
	if (( 0 == stat( "/dev/mtd10", &buffer ) ) ||
		( 0 == stat( "/dev/mtd11", &buffer ) ))
		return 0;

	return SIZEOF_UBOOTBITMAPS;
}

int MtdScaleSWConfig::dataLen()
{
	struct stat buffer;

	// ARK
	// we have dedicated partition in ARK so check for exist
	if (( 0 == stat( "/dev/mtd10", &buffer ) ) ||
		( 0 == stat( "/dev/mtd11", &buffer ) ))
		return SIZEOF_SECTION_ARK;

	return SIZEOF_SECTION;
}

