
/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "MetSig.h"
#include "evolution.h"

#include "CfgItem.h"

#include "CfgScale.h"
#include "CfgGenericLabelText.h"
#include "CfgSelfService.h"
#include "CfgNetwork.h"
#include "CfgInternal.h"
#include "CfgEmail.h"
#include "CfgCommInterface.h"
#include "CfgCommScheduled.h"
#include "CfgMarquee.h"
#include "CfgCurrency.h"
#include "CfgSynchronization.h"

#include "FrequentShopper.h"
#include "Graphic.h"
#include "HobDateTime.h"
#include "ScaleModel.h"
#include "Printer.h"

#include "MessageFormat.h"
#include "Integer.h"
#include "Bool.h"
#include "Long.h"


#define CommaString ","
#define PeriodString "."
#define DefaultDecimalSeparator PeriodString

typedef enum RemoteAccessLoginMethods
{
	RemoteAccessLoginMethod_None,
	RemoteAccessLoginMethod_OperatorID,
	RemoteAccessLoginMethod_UsernamePassword
} RemoteAccessLoginMethods_t;

typedef enum WeightFormats
{
	DefaultPrecision = 1,	//	Use the Precision defined in configuration.
	PrecisionPlusOne = 2,	//	Use the Precision defined in configuration plus one more decimal place.
	AppendWeightText = 4,
	AppendPerWeightText = 8,
	FixedWeightText = 16,	// Use for Fixed Weight label form formatting
	DualRangeModeDown = 32,
	MetricRuleWeight = 64,
	OperateFixedWeightText = 128,	// Used to combine the Fixed Weight formats in Operate mode (i.e. "16!1 lb 0 oz") - vxWorks Access
	KeyboardNetWeight = 256
}   WeightFormats_t;


typedef enum RotationOverrideSettings
{
	ROSNone,             // No override of the PLU rotation settings.
	ROSSetRotationTo,    // Override the PLU rotation by setting to a specific value.
	ROSAdjustRotationBy  // Override the PLU rotation by adding this value to the current rotation.
}RotationOverrideSettings_t;


typedef enum UnitPricePrintMethods
{ 
	UnitPricePrintMethodUnknown = 0,
	ForText,                          // For
	NoFractionalCents,                // $0.33  when bc=3, price=0.99 => no fractional cents
	BCSlashPrice,                     // 3 / $1.00
	BCForPrice,                       // 3 For $1.00
	BlankUnitPrice,                   // 
	RoundedUnitPrice                  // $0.67  when bc=3, price=2.00
}UnitPricePrintMethods_t;


// For GCD

enum CustomerDisplayType
{
	CustomerDisplayVFD = 1,
	CustomerDisplayGCD
};

#define ENGLISHLANGNAME HobString("English")
#define FRENCHLANGNAME HobString("French")
#define SPANISHLANGNAME HobString("Spanish")

// Originally from Weigher.h, but no longer stored in the APC EEP.
enum GcdLanguage
{
	// language values
	GcdLanguage_English	= 0x04,	// 0000 0100
	GcdLanguage_French	= 0x08,	// 0000 1000
	GcdLanguage_Spanish	= 0x0C,	// 0000 1100
};
	



/** This class follows the singleton pattern. */
class Configuration
{
	public:

		static int _printToScreenBarcodeFilenameCtr_;

		static const HobRecordNum NO_TOTALTYPES = -1;		//	For printFirstLetterOfTotalType - Do not print
		static const HobRecordNum ALL_TOTALTYPES = 0;		//	For printFirstLetterOfTotalType - Print for all

		static const short FIXED_WEIGHT_IN_WHOLE_UNITS = 0;		//	Whole units - Avoir. and Metric
		static const short FIXED_WEIGHT_IN_TENTHS = 1;			//	Tenths of an ounce - Avoir. only.

		static const int OPERATOR_LOGIN_KEYBOARD_QWERTY = 1;
		static const int OPERATOR_LOGIN_KEYBOARD_NUMERIC = 2;

		static const short FIXED_WEIGHT_FORMAT_STANDARD = 0;	//	3 LB 2 OZ
		static const short FIXED_WEIGHT_FORMAT_DECIMAL = 1;		//	3.13 LB

		// --- Types and subtypes ---
		typedef enum
		{
			InvalidPFFlag,

			BasicFormatPFFlag        =    0,
			SecondaryCurrencyPFFlag  =    1,
			UseMonetarySymbolPFFlag  =    2,
			AppendPerWeightPFFlag    =    4,
			SignedValuePFFlag        =    8,
			UseOldStyle              = 1024,

			LastFlag
		} PriceFormattingFlags;


		// --- Constructors and destructor ---
		static Configuration *getInstance();


		// deprecated - the following date/time formatters should be removed; they were moved to HobDateTime
		void formatDate(const HobDateTime date, HobString &result, const short formatIndicator = FormatDate_MMDDYY, int usingWhichLocale = HobDateTime::_UsingCurrentLocale_) {result = date.formatDate(formatIndicator, usingWhichLocale);}
		void formatTime(const HobDateTime time, HobString &result, const short formatIndicator = FormatDate_MMDDYY) {result = time.formatTime(formatIndicator);}
		void formatDateTime(const HobDateTime datetime, HobString &result, const short formatIndicator = FormatDate_MMDDYY, int usingWhichLocale = HobDateTime::_UsingCurrentLocale_) {result = datetime.formatDateTime(formatIndicator, usingWhichLocale);}

		std::string formatHobPrice(const HobPrice& price, const bool useDefaultDecimalSeparator = true, int flags = BasicFormatPFFlag);
		void formatPrice(const HobPrice price, HobString &result, const bool useDefaultDecimalSeparator = true, const short formatIndicator = 0, bool isSigned = false);

		// Returns the print method ultimately used to format the unit price string.  For example, if NoFractionalCents is the user-configured method, and the division results in fractional cents,
		// then we go to the backup method (internal config parameter 'unitPricePrintExactCalcBackupMethod').  If this backup method is to use the traditional approach, the word 'For', we don't want
		// to display that on the UI, since it never has.  So checking the return value indicates that the traditional method was used, so we can blank out the UI value, but still print it on the label.
		UnitPricePrintMethods formatUnitPriceForPrinting(const Product *pCurrentProduct, const ProductTransaction *pCurrentProductTransaction, HobString &result);

		void formatValue(const HobValue value, HobString &result, const short formatIndicator, const bool useDefaultDecimalSeparator = true);        // Intentionally removed the default value for the formatIndicator parameter
		void formatWeight(const HobWeight weight, HobString &result, const bool useDefaultDecimalSeparator = true, const short formatIndicator = 0);
		void formatDecimalValue(const HobValue value, HobString &result, const short precision, const bool useDefaultDecimalSeparator = true, const bool isSigned = false, const bool leadingZero = true);

		void formatVolume (const HobWeight totalOunces, HobString &result, const short formatIndicator, const bool useDefaultDecimalSeparator = true, const bool maxAtPints = false);
		void formatFixedVolumeValue (const HobWeight totalOunces, HobString &result, const short formatIndicator, bool maxAtPints, const bool useDefaultDecimalSeparator = true);
		void formatFixedWeightValue (const HobWeight weight, HobString &result, const bool useDefaultDecimalSeparator = true);

		int determineHourForQuery() const;

		void getIPAndGatewayAddresses(std::string &ipAddressString, std::string &gatewayAddressString);

		std::string getLicensedPackages( bool recheck = false );
		bool isPackageLicensed(const string packageName);
		bool isInternationalRoundingLicensed();
		bool isProductionPlanningLicensed();
		bool isTransactionalGCDLicensed();
		bool isOnlineOrderingLicensesd();
		bool isCAT3AuditTrailLicensed();
		bool isRemotePrintingLicensed();
		bool isDigimarcLicensed();
		bool isThreeDigitTare();
		bool isDotWearLicensed();
		bool isDotWearEnabled();
		short digitsPrecision();

		const CfgInternal &getCfgInternal() const { return cfgInternal; }

		std::string toStringCfgI(const std::string& name) const {return cfgInternal.toString(name);}
		int toIntCfgI(const std::string name) const {return cfgInternal.toInt(name);}
		bool toBoolCfgI(const std::string name) const {return cfgInternal.toBool(name);}

		std::string toStringCfgI(int fieldCode) const {return cfgInternal.toString(fieldCode);}
		int toIntCfgI(int fieldCode) const {return cfgInternal.toInt(fieldCode);}
		bool toBoolCfgI(int fieldCode) const {return cfgInternal.toBool(fieldCode);}

		std::string getName(int fieldCode) const;

		void checkTracingCfgI() { cfgInternal.checkTracingCfgI(); }

		bool isGCDCategoryPromotion() const;

		LabelType* getRFIDVoidLabelType();


		// Read the configuration data from the database into memory.
		void loadCfgNetwork()  {cfgNetwork.load();}
		void loadCfgInternal() {cfgInternal.load();}
		void checkReloadOfCfgI() { cfgInternal.checkReloadOfCfgI(); }

		void load(DBAccess *dbAccess);
		bool loadField(DBAccess *dbAccess, const std::string &fieldName, std::string &value);

		void updateConfigurationFromUI(const list<CfgFieldOffsets> &cfgList, const string &receivedData);
		void updateField(int fieldCode, const string &value);

		bool isSaveNeeded(const std::list<CfgFieldOffsets> &cfgList) const;
		void save(DBAccess *dbAccess);
		void saveField(int fieldCode, DBAccess *dbAccess);
		void saveField(DBAccess *dbAccess, const std::string &fieldName, const std::string &value, bool batchIt = false);

		void deleteField(DBAccess *dbAccess, const std::string &fieldName);

		void writeSystemLangFile();

		bool weigherConfigExistsInDB(DBAccess *dbAccess);
		void deleteWeigherConfigFromDB(DBAccess *dbAccess);

		void checkTracingCfg();

		void show(); //const;


		std::string toStringCfg(int fieldCode) const;
		int toIntCfg(int fieldCode) const;
		bool toBoolCfg(int fieldCode) const;

		void setCfg(int fieldCode, const string &value);
		bool isModifiedCfg(int fieldCode) const;



		CfgGenericLabelText *getGenericLabelText()          {return &cfgGenericLabelText;}
		//void update(const CfgGenericLabelText *newValues) {cfgGenericLabelText.update(newValues);}
		//void save(const CfgGenericLabelText *newValues)   {cfgGenericLabelText.save(newValues);}

		const CfgSelfService &getCfgSelfService() const                {return cfgSelfService;}
		void update(const CfgSelfService *newValues)                   {cfgSelfService.update(newValues);}
		void save(DBAccess *dbAccess, const CfgSelfService *newValues) {cfgSelfService.save(dbAccess, newValues);}

		const CfgNetwork &getCfgNetwork() const                    {return cfgNetwork;}
		void update(const CfgNetwork *newValues)                   {cfgNetwork.update(newValues);}
		void save(DBAccess *dbAccess, const CfgNetwork *newValues) {cfgNetwork.save(dbAccess, newValues);}

		const CfgEmail &getCfgEmail() const                      {return cfgEmail;}
		void update(const CfgEmail *newValues)                   {cfgEmail.update(newValues);}
		void save(DBAccess *dbAccess, const CfgEmail *newValues) {cfgEmail.save(dbAccess, newValues);}

		const CfgCommInterface &getCfgCommInterface() const              {return cfgCommInterface;}
		void update(const CfgCommInterface *newValues)                   {cfgCommInterface.update(newValues);}
		void save(DBAccess *dbAccess, const CfgCommInterface *newValues) {cfgCommInterface.save(dbAccess, newValues);}

		const CfgMarquee &getCfgMarquee() const                    {return cfgMarquee;}
		void update(const CfgMarquee *newValues)                   {cfgMarquee.update(newValues);}
		void save(DBAccess *dbAccess, const CfgMarquee *newValues) {cfgMarquee.save(dbAccess, newValues);}

		const CfgCurrency &getCfgCurrency() const                   {return cfgCurrency;}
		void update(const CfgCurrency *newValues)                   {cfgCurrency.update(newValues);}
		void save(DBAccess *dbAccess, const CfgCurrency *newValues) {cfgCurrency.save(dbAccess, newValues);}


		void formatDriver();



/*****************************************************************************
 *  CfgScale - Configuration parameters applicable to the entire scale.
 ****************************************************************************/

		bool isConserveContinuousStockByNotSizingEnabled();

		bool isConserveLinerlessStock();	//SOF-3875

		void setStoreName(HobString name) { name.convertToEvolFontSequences(); setCfg(fieldOffsetCfgStoreName, name); }

		void setStoreGraphicName(HobString name) { if (name == Configuration::getInstance()->toStringCfgI(CfgInternal::NOGRAPHICSELECTEDNAME)) name = "";
		                                           setCfg(fieldOffsetCfgStoreGraphic, name);
		                                           loadStoreGraphic(); }

		const Graphic *getStoreGraphic() const { return &storeGraphic; }
		void loadStoreGraphic() { storeGraphic.setGraphicName(toStringCfg(fieldOffsetCfgStoreGraphic));
		                          if (toStringCfg(fieldOffsetCfgStoreGraphic).size())
		                          {
		                             storeGraphic.setType(Graphic::TYPE_LABELGRAPHIC);
		                             storeGraphic.load();
	                              }
								  else
								  {
		                             storeGraphic.unload();
								  }
		                        }

		// No longer used as of 9-19-17- claim back later - HobString getDeptName() const { return toStringCfg(fieldOffsetCfgDeptName); }

		HobString getPrimaryCountryCode() const { return toStringCfg(fieldOffsetCfgPrimaryCountryCode); }
		void setPrimaryCountryCode(HobString code) { setCfg(fieldOffsetCfgPrimaryCountryCode, code); }

		HobString getPrimaryLanguageCode() const { return toStringCfg(fieldOffsetCfgPrimaryLanguageCode); }
		void setPrimaryLanguageCode(HobString code) { setCfg(fieldOffsetCfgPrimaryLanguageCode, code); }

		HobString getSecondaryCountryCode() const { return toStringCfg(fieldOffsetCfgSecondaryCountryCode); }
		void setSecondaryCountryCode(const HobString &code) { setCfg(fieldOffsetCfgSecondaryCountryCode, code); }

		HobString getSecondaryLanguageCode() const { return toStringCfg(fieldOffsetCfgSecondaryLanguageCode); }
		void setSecondaryLanguageCode(const HobString &code) { setCfg(fieldOffsetCfgSecondaryLanguageCode, code); }

		bool isDualLanguageEnabled() const;

		short getLengthUnits() const { return (short) toIntCfg(fieldOffsetCfgLengthUnits); }		// 0 = U.S. (inches), 1 = metric (centimeters)
		void setLengthUnits(short units) { setCfg(fieldOffsetCfgLengthUnits, inttostring(units)); }

		short getTemperatureUnits() const { return (short) toIntCfg(fieldOffsetCfgTemperatureUnits); }		// 0 = Fahrenheit, 1 = Celsius
		void setTemperatureUnits(short units) { setCfg(fieldOffsetCfgTemperatureUnits, inttostring(units)); }

		//const HobString &getCurrencyUnits() const { return toStringCfg(); }		// Dollars, Pesos, Reels, ...
		//void setCurrencyUnits(const HobString &units) { setCfg(, units); }

//		const HobString &getMonetarySymbol() const { return toStringCfg(); }

		short getDateDisplayFormat() const { return (short) toIntCfg(fieldOffsetCfgDateFormat); }
		void setDateDisplayFormat(short format) { setCfg(fieldOffsetCfgDateFormat, inttostring(format)); }

		short getTimeDisplayFormat() const { return (short) toIntCfg(fieldOffsetCfgTimeFormat); }
		void setTimeDisplayFormat(short format) { setCfg(fieldOffsetCfgTimeFormat, inttostring(format)); }

		/*** Pricing-related parameters ***/

//		short getPricePrecision() const { return (short) toIntCfg(); }
//		void setPricePrecision(short precision) {  setCfg(, inttostring(precision)); }
//		double getPriceDivisor() const { return (double) toIntCfg(); }

		HobPrice getSecurityLblUnitPrice() const { return (HobPrice) toIntCfg(fieldOffsetCfgUnitPriceSecurityLabel); }
		void setSecurityLblUnitPrice(std::string price) { setCfg(fieldOffsetCfgUnitPriceSecurityLabel, longtostring( MetSig::determineImplicitDecimalPrice(price) )); }

		HobPrice getSecurityLblTtlPrice() const { return (HobPrice) toIntCfg(fieldOffsetCfgTotalPriceSecurityLabel); }
		void setSecurityLblTtlPrice(std::string price) { setCfg(fieldOffsetCfgTotalPriceSecurityLabel, longtostring( MetSig::determineImplicitDecimalPrice(price) )); }

		bool isFractionalPricingEnabled() const { return toBoolCfg(fieldOffsetCfgFractionalPricingEnabled); }
		void setFractionalPricingEnabled(bool enabled) { setCfg(fieldOffsetCfgFractionalPricingEnabled, Bool::toString(enabled)); }

		bool isPoundsForPricingEnabled() const { return toBoolCfg(fieldOffsetCfgPoundsForPricingEnabled); }
		void setPoundsForPricingEnabled(bool enabled) { setCfg(fieldOffsetCfgPoundsForPricingEnabled, Bool::toString(enabled)); }

		short getByCountCalculationMethod() const { return (short) toIntCfg(fieldOffsetCfgBycountCalcMethod); }
		void setByCountCalculationMethod(short method) { setCfg(fieldOffsetCfgBycountCalcMethod, inttostring(method)); }

		/*** Beep-related parameters ***/

		bool isSoundBeepOnStableWeight() const { return toBoolCfg(fieldOffsetCfgBeepOnStableWeight); }
		void setSoundBeepOnStableWeight(bool soundBeep) { setCfg(fieldOffsetCfgBeepOnStableWeight, Bool::toString(soundBeep)); }

		short getBeeperVolumeLevel() const { return (short) toIntCfg(fieldOffsetCfgBeeperVolumeLevel); }
		void setBeeperVolumeLevel(short level) { setCfg(fieldOffsetCfgBeeperVolumeLevel, inttostring(level)); }

		/*** Weight-related parameters ***/

		short getFixedWeightUnits() const { return (short) toIntCfg(fieldOffsetCfgFixedWeightUnits); }
		void setFixedWeightUnits(short units) { if (units == FIXED_WEIGHT_IN_TENTHS || units == FIXED_WEIGHT_IN_WHOLE_UNITS) setCfg(fieldOffsetCfgFixedWeightUnits, inttostring(units)); }
		bool isFixedWeightInTenths() { return (MetSig::isScaleAvoir() && (toIntCfg(fieldOffsetCfgFixedWeightUnits) == FIXED_WEIGHT_IN_TENTHS)) ? true : false; }

		bool isWeigherAttached() const { return toBoolCfg(fieldOffsetCfgWeigherAttached); }
		void setWeigherAttached(bool attached) { setCfg(fieldOffsetCfgWeigherAttached, Bool::toString(attached)); }

		HobWeight getTareSetback() const { return (HobWeight) toIntCfg(fieldOffsetCfgTareSetback); }
		void setTareSetback(std::string tare);	//	HA-73

		HobWeight getMaxTare() const { return (HobWeight) toIntCfg(fieldOffsetCfgMaxTare); }
		void setMaxTare(std::string tare);	//	HA-73

		void setMinSweetheartWeight( std::string weight );

		bool isPlatterTareEnabled() const { return toBoolCfg(fieldOffsetCfgPlatterTareEnabled); }
		void setPlatterTareEnabled(bool enabled) { setCfg(fieldOffsetCfgPlatterTareEnabled, Bool::toString(enabled)); }

		bool isChangeZeroTareOnlyOnce() const { return toBoolCfg(fieldOffsetCfgChangeZeroTareOnlyOnce); }
		void setChangeZeroTareOnlyOnce(bool enabled) { setCfg(fieldOffsetCfgChangeZeroTareOnlyOnce, Bool::toString(enabled)); }

		bool isChangeForcedTareOnlyOnce() const { return toBoolCfg(fieldOffsetCfgChangeForcedTareOnlyOnce); }
		void setChangeForcedTareOnlyOnce(bool enabled) { setCfg(fieldOffsetCfgChangeForcedTareOnlyOnce, Bool::toString(enabled)); }

		// This is a copy of the minWeightToPrint stored in the Weigher EEP, and read via the WeigherInquire/Configuration data.
		// The setter should only be called by any code that processes the WeigherInquire/WeigherConfiguration data.
		HobWeight getMinWeightToPrint() const { return (HobWeight) toIntCfg(fieldOffsetCfgMinWeightToPrintLabel); }
		void setMinWeightToPrint(HobWeight weight) { setCfg(fieldOffsetCfgMinWeightToPrintLabel, Long::toString(weight)); }
		void setMinWeightToPrint(std::string weight) { double newWeight = Double::valueOf(weight);
		                                               if (weight.find(".") != std::string::npos)
		                                                  newWeight *= getWeightDivisor();
		                                               setCfg(fieldOffsetCfgMinWeightToPrintLabel, Long::toString((HobWeight)newWeight));
		                                             }

		// This is a copy of the weigherModel stored in the Weigher EEP, and read via the WeigherInquire/Configuration data.
		// The setter should only be called by any code that processes the WeigherInquire/WeigherConfiguration data.
		WeigherModel getWeigherModel() const { return (WeigherModel) toIntCfg(fieldOffsetCfgWeigherModel); }
		void setWeigherModel(int weigherModel) { setCfg(fieldOffsetCfgWeigherModel, (weigherModel > WmNone && weigherModel < endWeigherModel) ? inttostring(weigherModel) : inttostring(WmNone));
		                                         setWeigherAttached((toIntCfg(fieldOffsetCfgWeigherModel) == WmNone) ? false : true);
		                                       }

		bool getCustomerHooksEnabled() const { return !toBoolCfg(fieldOffsetCfgDisableHooks); }

		// This is a copy of the valueMaxEnabled stored in the Weigher EEP, and read via the WeigherInquire/Configuration data.
		// The setter should only be called by any code that processes the WeigherInquire/WeigherConfiguration data.
		int getValueMaxEnabled() const { return toIntCfg(fieldOffsetCfgValueMaxEnabled); }
		void setValueMaxEnabled(int isVMaxEnabled) { setCfg(fieldOffsetCfgValueMaxEnabled, inttostring(isVMaxEnabled)); }

		void setCutterEnabled(int isCutterEnabled) { setCfg(fieldOffsetCfgIsCutterEnabled, inttostring(isCutterEnabled)); }
	
		// This is a copy of the weighMode stored in the Weigher EEP, and read via the WeigherInquire/Configuration data.
		// The setter (moved to Configuration.cpp) should only be called by any code that processes the WeigherInquire/WeigherConfiguration data.
		WeighMode getScaleWeighMode() const { return MetSig::getScaleWeighMode(); }
		void setScaleWeighMode(WeighMode mode);



		UINT32 getEffectiveWeigherScaleFactor() const;
		UINT32 getWeigherScaleFactor() const;

		// SOF-789: Wrap-only
		bool isScaleModelWrapOnly() const;
		bool isWrapper() const;

		// This is a copy of the netWeightDualRangeLimitCheck stored in the Weigher EEP, and read via the WeigherInquire/Configuration data.
		// The setter should only be called by any code that processes the WeigherInquire/WeigherConfiguration data.
		bool isNetWeightDualRangeLimitCheck() const { return toBoolCfg(fieldOffsetCfgNetWeightDualRangeLimitCheck); }

		void setNetWeightDualRangeLimitCheck(bool usingNetWeight) { setCfg(fieldOffsetCfgNetWeightDualRangeLimitCheck, Bool::toString(usingNetWeight)); }

////		bool weigherConfigExistsInDB(DBAccess *dbAccess) {  }
////		void deleteWeigherConfigFromDB(DBAccess *dbAccess) {  }

		/**
		 * The number of digits to the right of the decimal point for weight values.
		 * The value returned depends on the setting of the scaleWeighMode.
		 */
		short getEffectiveWeightPrecision(short formatIndicator = 0) const;
		short getWeightPrecision(short formatIndicator = 0) const;
		short getKeyboardNetWeightPrecision() const;
		double getWeightDivisor(short formatIndicator = 0) const {return Power::power((double)10.0, (double)getWeightPrecision(formatIndicator));}


		bool isWeigherWakeupSensorAttached() const { return toBoolCfg(fieldOffsetCfgWeigherWakeupSensorAttached); }
		void setWeigherWakeupSensorAttached(bool sensorAttached) { setCfg(fieldOffsetCfgWeigherWakeupSensorAttached, Bool::toString(sensorAttached)); }

		bool isCodeCheckerEnabled() const { return toBoolCfg(fieldOffsetCfgIsCodeCheckerEnabled); }
		void setCodeCheckerEnabled(bool enabled) { setCfg(fieldOffsetCfgIsCodeCheckerEnabled, Bool::toString(enabled)); }

		bool allowSupervisorDotWear() const { return toBoolCfg(fieldOffsetCfgAllowSupervisorDotWear); }
		void setAllowSupervisorDotWear(bool enabled) { setCfg(fieldOffsetCfgAllowSupervisorDotWear, Bool::toString(enabled)); }

		bool allowOperatorDotWear() const { return toBoolCfg(fieldOffsetCfgAllowOperatorDotWear); }
		void setAllowOperatorDotWear(bool enabled) { setCfg(fieldOffsetCfgAllowOperatorDotWear, Bool::toString(enabled)); }

		/*** General config parameters ***/

		bool isLoginRequired() const { return toBoolCfg(fieldOffsetCfgOperateLoginRequired); }
		void setLoginRequired(bool required) { setCfg(fieldOffsetCfgOperateLoginRequired, Bool::toString(required)); }

		int getInactivityAutoLogoutTime() const { return toIntCfg(fieldOffsetCfgInactivityAutoLogoutTime); }
		void setInactivityAutoLogoutTime(int minutes) { setCfg(fieldOffsetCfgInactivityAutoLogoutTime, inttostring(minutes)); }

		bool isAutoLookupProduct() const { return toBoolCfg(fieldOffsetCfgAutoLookupProduct); }
		void setAutoLookupProduct(bool autoLookup) { setCfg(fieldOffsetCfgAutoLookupProduct, Bool::toString(autoLookup)); }

		short getAutoLookupNumOfDigits() const { return (short) toIntCfg(fieldOffsetCfgAutoLookupNumOfDigits); }
		void setAutoLookupNumOfDigits(short numDigits) { setCfg(fieldOffsetCfgAutoLookupNumOfDigits, inttostring(numDigits)); }

		bool isAutoDisplayProductNotes() const { return toBoolCfg(fieldOffsetCfgAutoDisplayProductNotes); }
		void setAutoDisplayProductNotes(bool autoDisplay) { setCfg(fieldOffsetCfgAutoDisplayProductNotes, Bool::toString(autoDisplay)); }

		int getDbBackupFrequency() const { return toIntCfg(fieldOffsetCfgDbBackupFrequency); }
		void setDbBackupFrequency(int frequency) { setCfg(fieldOffsetCfgDbBackupFrequency, inttostring(frequency)); }

		short getHorizLabelSkew(PrinterType prType) const;
		void setHorizLabelSkew(PrinterType prType, short labelSkew);

		short getHorizLabelSkew1() const { return (short) toIntCfg(fieldOffsetCfgHorizLabelSkew1); }
		void setHorizLabelSkew1(short labelSkew) { setCfg(fieldOffsetCfgHorizLabelSkew1, inttostring(labelSkew)); }
		short getHorizLabelSkew2() const { return (short) toIntCfg(fieldOffsetCfgHorizLabelSkew2); }
		void setHorizLabelSkew2(short labelSkew) { setCfg(fieldOffsetCfgHorizLabelSkew2, inttostring(labelSkew)); }
		short getHorizLabelSkew3() const { return (short) toIntCfg(fieldOffsetCfgHorizLabelSkew3); }
		void setHorizLabelSkew3(short labelSkew) { setCfg(fieldOffsetCfgHorizLabelSkew3, inttostring(labelSkew)); }

		bool isOpAccessToBrowserMode() const { return toBoolCfg(fieldOffsetCfgOpAccessToBrowserMode); }
		void setOpAccessToBrowserMode(bool allowAccess) { setCfg(fieldOffsetCfgOpAccessToBrowserMode, Bool::toString(allowAccess)); }

		bool isAutoLoadOldFirmwareVersions() const { return toBoolCfg(fieldOffsetCfgAutoLoadOldFirmwareVersions); }
		void setAutoLoadOldFirmwareVersions(bool autoLoad) { setCfg(fieldOffsetCfgAutoLoadOldFirmwareVersions, Bool::toString(autoLoad)); }

		bool isAutoSearchNewFirmwareVersions() const { return toBoolCfg(fieldOffsetCfgAutoSearchNewFirmwareVersions); }
		void setAutoSearchNewFirmwareVersions(bool autoSearch) { setCfg(fieldOffsetCfgAutoSearchNewFirmwareVersions, Bool::toString(autoSearch)); }

		HobString getNewFirmwareVersionSearchTime() const { return toStringCfg(fieldOffsetCfgNewFirmwareVersionSearchTime); }
		void setNewFirmwareVersionSearchTime(HobString searchTime) { setCfg(fieldOffsetCfgNewFirmwareVersionSearchTime, searchTime); }

        const HobString getUpdateNowSearchTime() const { return toStringCfg(fieldOffsetCfgFTPstartTime); }
		void setUpdateNowSearchTime(HobString updateNowSearchTime) { setCfg(fieldOffsetCfgFTPstartTime, updateNowSearchTime); }

		bool isForceUniqueProdClassNums() const { return toBoolCfg(fieldOffsetCfgForceUniqueProdClassNums); }
		void setForceUniqueProdClassNums(bool forceUnique) { setCfg(fieldOffsetCfgForceUniqueProdClassNums, Bool::toString(forceUnique)); }

		bool isEnablePowerSaverMode() const { return toBoolCfg(fieldOffsetCfgEnablePowerSaverMode); }
		void setEnablePowerSaverMode(bool enable) { setCfg(fieldOffsetCfgEnablePowerSaverMode, Bool::toString(enable)); }

		short getPowerSaverModeTimeout() const { return (short) toIntCfg(fieldOffsetCfgPowerSaverModeTimeout); }
		void setPowerSaverModeTimeout(short timeout) { setCfg(fieldOffsetCfgPowerSaverModeTimeout, inttostring(timeout)); }

		HobString getDefaultBrowserHomePage() const { return toStringCfg(fieldOffsetCfgDefaultBrowserHomePage); }
		void setDefaultBrowserHomePage(HobString address) { setCfg(fieldOffsetCfgDefaultBrowserHomePage, address); }

		Product::ProductType getDefaultProductRecordType() const { return (Product::ProductType) toIntCfg(fieldOffsetCfgDefaultProductRecordType); }
		void setDefaultProductRecordType(int defaultType) { if (defaultType > Product::PRODUCTTYPE_UNKNOWN && defaultType < Product::PRODUCTTYPE_INVALID) setCfg(fieldOffsetCfgDefaultProductRecordType, inttostring(defaultType)); }

		short getDefaultFilmWidth() const { return (short) toIntCfg(fieldOffsetCfgDefWrFilmWidth); }
		void setDefaultFilmWidth(short filmWidth) { setCfg(fieldOffsetCfgDefWrFilmWidth, (filmWidth == WidePackage || filmWidth == FilmWidthIdle || filmWidth == AutoWidthSelect) ? inttostring((int)filmWidth) : inttostring(NarrowPackage)); }

		short getDefaultFilmTension() const { return (short) toIntCfg(fieldOffsetCfgDefWrFilmTension); }
		void setDefaultFilmTension(short filmTension) { setCfg(fieldOffsetCfgDefWrFilmTension, (filmTension == LowFilmTension || filmTension == MediumFilmTension || filmTension == HighFilmTension) ? inttostring((int)filmTension) : inttostring((int)MediumFilmTension)); }

		short getDefaultFilmTensionFrontToBack() const { return (short) toIntCfg(fieldOffsetCfgDefWrFilmTensionFrontToBack); }
		void setDefaultFilmTensionFrontToBack(short filmTension) { if (filmTension >= -5 && filmTension <= 5) setCfg(fieldOffsetCfgDefWrFilmTensionFrontToBack, inttostring((int)filmTension)); }

		short getDefaultFilmTensionLeftToRight() const { return (short) toIntCfg(fieldOffsetCfgDefWrFilmTensionLeftToRight); }
		void setDefaultFilmTensionLeftToRight(short filmTension) { if (filmTension >= -5 && filmTension <= 5) setCfg(fieldOffsetCfgDefWrFilmTensionLeftToRight, inttostring((int)filmTension)); }

		short getDefaultIntakeMode() const { return (short) toIntCfg(fieldOffsetCfgDefWrIntakeMode); }
		void setDefaultIntakeMode(short intakeMode) { setCfg(fieldOffsetCfgDefWrIntakeMode, (intakeMode == WrapperIntakeMode::NormalIntake || intakeMode == WrapperIntakeMode::GentleIntake) ? inttostring((int)intakeMode) : inttostring(WrapperIntakeMode::NormalIntake)); }

		short getDefaultLabelPlacement() const { return (short) toIntCfg(fieldOffsetCfgDefWrLabelPlacement); }
		void setDefaultLabelPlacement(short labelPlacement) { setCfg(fieldOffsetCfgDefWrLabelPlacement, inttostring(labelPlacement)); }

		short getSecDefaultLabelPlacement() const { return (short) toIntCfg(fieldOffsetCfgDefWrSecLabelPlacement); }
		void setSecDefaultLabelPlacement(short labelPlacement) { setCfg(fieldOffsetCfgDefWrSecLabelPlacement, inttostring(labelPlacement)); }

		short getDefaultApplierRotation() const { return (short) toIntCfg(fieldOffsetCfgDefWrApplierRotation); }
		void setDefaultApplierRotation(short applierRotation) { setCfg(fieldOffsetCfgDefWrApplierRotation, inttostring(applierRotation)); }

		short getSecDefaultApplierRotation() const { return (short) toIntCfg(fieldOffsetCfgDefWrSecApplierRotation); }
		void setSecDefaultApplierRotation(short applierRotation) { setCfg(fieldOffsetCfgDefWrSecApplierRotation, inttostring(applierRotation)); }

		void setSealerBeltTime(int fieldOffset, const std::string &time) { setCfg(fieldOffset, ((time == toStringCfgI(CfgInternal::NOGRAPHICSELECTEDNAME)) ? "" : time)); }


		bool isAutoCompressDB() const { return toBoolCfg(fieldOffsetCfgAutoCompressDB); }
		void setAutoCompressDB(bool autoCompress) { setCfg(fieldOffsetCfgAutoCompressDB, Bool::toString(autoCompress)); }

		HobString getCompressDBTime() const { return toStringCfg(fieldOffsetCfgCompressDBTime); }
		void setCompressDBTime(HobString compressTime) { setCfg(fieldOffsetCfgCompressDBTime, compressTime); }

		HobString getAutoLookupOverrideDigit() const { return toStringCfg(fieldOffsetCfgAutoLookupOverrideDigit); }
		void setAutoLookupOverrideDigit(HobString digit) { setCfg(fieldOffsetCfgAutoLookupOverrideDigit, digit); }

		bool isCOOLTrackingNumberEnabled() const { return toBoolCfg(fieldOffsetCfgCOOLTrackingNumberEnabled); }
		void setCOOLTrackingNumberEnabled(bool enabled) { setCfg(fieldOffsetCfgCOOLTrackingNumberEnabled, Bool::toString(enabled)); }

		bool isPopupKeypadEnabled() const { if (isServiceCounterScale()) return true; else return toBoolCfg(fieldOffsetCfgPopupKeypadEnabled); }
		void setPopupKeypadEnabled(bool enabled) { if (isServiceCounterScale()) setCfg(fieldOffsetCfgPopupKeypadEnabled, Bool::toString(true));
		                                           else setCfg(fieldOffsetCfgPopupKeypadEnabled, Bool::toString(enabled)); }

		short getExternalWeightPrecision() { return (MetSig::isScaleMetric()) ? 3 : (short) toIntCfg(fieldOffsetCfgExternalWeightPrecision); }
		void setExternalWeightPrecision(short precision) { setCfg(fieldOffsetCfgExternalWeightPrecision, inttostring(precision)); }

		short getTareEntryPrecision() { validateTareEntryPrecision(); return (short) toIntCfg(fieldOffsetCfgTareEntryPrecision); }
		void validateTareEntryPrecision();

		HobString getPrimaryExpirationPageNote() const { return toStringCfg(fieldOffsetCfgPrimaryExpirationPageNote); }
		void setPrimaryExpirationPageNote(const HobString &note) { setCfg(fieldOffsetCfgPrimaryExpirationPageNote, note); }

		HobString getSecondExpirationPageNote() const { return toStringCfg(fieldOffsetCfgSecondExpirationPageNote); }
		void setSecondExpirationPageNote(const HobString &note) { setCfg(fieldOffsetCfgSecondExpirationPageNote, note); }
		
		bool isOpAccessToDesktopMode() const { return toBoolCfg(fieldOffsetCfgOpAccessToDesktopMode); }
		void setOpAccessToDesktopMode(bool allowAccess) { setCfg(fieldOffsetCfgOpAccessToDesktopMode, Bool::toString(allowAccess)); }

		bool isClearPluAfterPrinting() const;
		void setClearPluAfterPrinting(bool clearPlu) { setCfg(fieldOffsetCfgClearPluAfterPrinting, Bool::toString(clearPlu)); }

		HobString getCommunicationHistory() const { return toStringCfg(fieldOffsetCfgCommunicationHistory); }
		void setCommunicationHistory(HobString commHistory) { setCfg(fieldOffsetCfgCommunicationHistory, commHistory); }

		// SOF-4769: Scheduled Merge
		HobString getMergeHistory() const { return toStringCfg(fieldOffsetCfgCommScheduledMergeHistory); }
		void setMergeHistory(HobString mergeHistory) { setCfg(fieldOffsetCfgCommScheduledMergeHistory, mergeHistory); }

		HobString getCommunicationItemReceived() const { return toStringCfg(fieldOffsetCfgCommunicationItemReceived); }
		void setCommunicationItemReceived(HobString commItemReceived) { setCfg(fieldOffsetCfgCommunicationItemReceived, commItemReceived); }

		int getPopupKeypadStyle() const { return toIntCfg(fieldOffsetCfgPopupKeypadStyle); }
		void setPopupKeypadStyle(int popupstyle) { if (popupstyle == 1 /* Telephone style */ || popupstyle == 2 /* Keyboard style */ ) setCfg(fieldOffsetCfgPopupKeypadStyle, inttostring(popupstyle)); }

		int getOperatorLoginKeyboardStyle() const { return toIntCfg(fieldOffsetCfgOperatorLoginKeyboardStyle); }
		void setOperatorLoginKeyboardStyle(int keyboardStyle) { setCfg(fieldOffsetCfgOperatorLoginKeyboardStyle, inttostring(keyboardStyle)); }
		bool isOperatorLoginKeyboardQWERTY() const { return (toIntCfg(fieldOffsetCfgOperatorLoginKeyboardStyle) == OPERATOR_LOGIN_KEYBOARD_QWERTY); }
		bool isOperatorLoginKeyboardNumeric() const { return (toIntCfg(fieldOffsetCfgOperatorLoginKeyboardStyle) == OPERATOR_LOGIN_KEYBOARD_NUMERIC); }

		bool isRebootUnprotectedAllowed() const { return toBoolCfg(fieldOffsetCfgRebootUnprotectedAllowed); }
		void setRebootUnprotectedAllowed(bool rebootAllowed) { setCfg(fieldOffsetCfgRebootUnprotectedAllowed, Bool::toString(rebootAllowed)); }

		bool isAutoCreateRFIDOperator() const { return toBoolCfg(fieldOffsetCfgAutoCreateRFIDOperator); }
		void setAutoCreateRFIDOperator(bool autoCreate) { setCfg(fieldOffsetCfgAutoCreateRFIDOperator, Bool::toString(autoCreate));}
		

		int getUserRezeroWeightLimit() const { return toIntCfg(fieldOffsetCfgUserRezeroWeightLimit); }
		void setUserRezeroWeightLimit(std::string limit) { double newWeight = Double::valueOf(limit);
		                                                   if (limit.find(".") != std::string::npos)
		                                                      newWeight *= getWeightDivisor();
		                                                   bool okToSet = true;
		                                                   if (MetSig::isScaleAvoir())
		                                                   {
		                                                      if (newWeight > toIntCfgI(CfgInternal::REZEROWEIGHTLIMITAVOIR))
		                                                         okToSet = false;
		                                                   }
		                                                   else  // is metric
		                                                   {
		                                                      if (newWeight > toIntCfgI(CfgInternal::REZEROWEIGHTLIMITMETRIC))
		                                                         okToSet = false;
		                                                   }
		                                                   if (okToSet)
		                                                      setCfg(fieldOffsetCfgUserRezeroWeightLimit, Long::toString((HobWeight)newWeight));
		                                                 }
		
		bool isShowDeleteAllFiles() const { return toBoolCfg(fieldOffsetCfgShowDeleteAllFiles); }
		void setShowDeleteAllFiles(bool value) { setCfg(fieldOffsetCfgShowDeleteAllFiles, Bool::toString(value)); }

		bool isShowPrinterSettings() const { return toBoolCfg(fieldOffsetCfgShowPrinterSettings); }
		void setShowPrinterSettings(bool value) { setCfg(fieldOffsetCfgShowPrinterSettings, Bool::toString(value)); }

		bool isRemoteAccessPermitted() const { return toBoolCfg(fieldOffsetCfgRemoteAccessPermitted); }
		void setRemoteAccessPermitted(bool value) { setCfg(fieldOffsetCfgRemoteAccessPermitted, Bool::toString(value)); }

		bool isBackdateByDays() const { return toBoolCfg(fieldOffsetCfgBackdateByDays); }
		void setBackdateByDays(bool value) { setCfg(fieldOffsetCfgBackdateByDays, Bool::toString(value)); }

		bool isIgnoreUltimaFormatLabelTypes() const { return toBoolCfg(fieldOffsetCfgIgnoreUltimaFormatLabelTypes); }
		void setIgnoreUltimaFormatLabelTypes(bool value) { setCfg(fieldOffsetCfgIgnoreUltimaFormatLabelTypes, Bool::toString(value)); }

		bool getPingTarget() const { return toBoolCfg(fieldOffsetCfgPingTarget); }
		void setPingTarget(bool check) { setCfg(fieldOffsetCfgPingTarget, Bool::toString(check)); } 

		HobIpAddress getPingTargetIPAddress() const { HobIpAddress target(toStringCfg(fieldOffsetCfgPingTargetIPAddress)); return target; }
		void setPingTargetIPAddress(HobString target) { setCfg(fieldOffsetCfgPingTargetIPAddress, target); }

		HobString getPingInterval() const { return toStringCfg(fieldOffsetCfgPingInterval); }
		void setPingInterval(HobString value) { setCfg(fieldOffsetCfgPingInterval, value); }
		unsigned int getPingIntervalValue() const { // Convert the HH:MM string into a total number of minutes.
		                                            int minutes = 0;
		                                            string pingInterval = toStringCfg(fieldOffsetCfgPingInterval);
		                                            string::size_type thePos = pingInterval.find(":");
		                                            if (thePos != std::string::npos)
		                                            {
		                                               minutes = Integer::valueOf(pingInterval.substr(0, thePos));
		                                               minutes *= 60;
		                                               minutes += Integer::valueOf(pingInterval.substr(thePos + 1));
	                                               }
		                                            return minutes;
		                                          }

		bool isDontChangeCurrentCOOLViaComm() const { return toBoolCfg(fieldOffsetCfgDontChangeCurrentCOOL); }
		void setDontChangeCurrentCOOLViaComm(bool value) { setCfg(fieldOffsetCfgDontChangeCurrentCOOL, Bool::toString(value)); }
		bool isEnableUPTagPrefix() const { return toBoolCfg(fieldOffsetCfgEnableUPTagPrefix); }
		void setEnableUPTagPrefix(bool value) { setCfg(fieldOffsetCfgEnableUPTagPrefix, Bool::toString(value)); }

		unsigned int getReportsOffsetTop() const { return (unsigned int) toIntCfg(fieldOffsetCfgReportsOffsetTop); }
		void setReportsOffsetTop(unsigned int offset) { setCfg(fieldOffsetCfgReportsOffsetTop, inttostring(offset)); }

		unsigned int getReportsOffsetBottom() const { return (unsigned int) toIntCfg(fieldOffsetCfgReportsOffsetBottom); }
		void setReportsOffsetBottom(unsigned int offset) { setCfg(fieldOffsetCfgReportsOffsetBottom, inttostring(offset)); }

		bool isOpModeStreamingLabelTestEnabled() const { return false; }

		HobString getExcludedTagList() const { return toStringCfg(fieldOffsetCfgFieldExclusions); }
		void setExcludedTagList(unsigned int offset) { setCfg(fieldOffsetCfgFieldExclusions, inttostring(offset)); }

/*****************************************************************************
 *  CfgGenericLabelText
 ****************************************************************************/

		short getMaxLabelTexts() const {return cfgGenericLabelText.getMaxLabelTexts();}
		// This was commented out when the code was changed to pass in the dbAccess to save each config record; and I don't have that in this class,
		// and it was needed within this call.  No one ever called this, so for now, it is commented out.
		//void setMaxLabelTexts(short max) {cfgGenericLabelText.setMaxLabelTexts(max);}

		HobString getLabelText(int labelTextNum) const {return cfgGenericLabelText.getLabelText(labelTextNum);}
		void setLabelText(int labelTextNum, GenericLabelText* labelText) {cfgGenericLabelText.setLabelText(labelTextNum, labelText);}



/*****************************************************************************
 *  CfgFrequentShopper
 ****************************************************************************/

		void setDiscountMethod(short method) { if (method >= RII_DISCOUNT_METHOD && method < INVALID_DISCOUNT_METHOD) setCfg(fieldOffsetCfgFSDiscountMethod, inttostring((int)method)); }

		//	perScale can be RII_DISCOUNT_METHOD through FLEX_DISCOUNT_METHOD_PER_SCALE
		bool isPerScale() const { return (toIntCfg(fieldOffsetCfgFSDiscountMethod) != FLEX_DISCOUNT_METHOD_PER_PLU) ? true : false; }
		void setPerScale() { setCfg(fieldOffsetCfgFSDiscountMethod, inttostring(FLEX_DISCOUNT_METHOD_PER_SCALE)); }

		bool isPerPlu() const { return (toIntCfg(fieldOffsetCfgFSDiscountMethod) == FLEX_DISCOUNT_METHOD_PER_PLU) ? true : false; }
		void setPerPlu() { setCfg(fieldOffsetCfgFSDiscountMethod, inttostring(FLEX_DISCOUNT_METHOD_PER_PLU)); }

		void setRoundingMethod(short newRoundingMethod) { setCfg(fieldOffsetCfgFSRoundingMethod, (newRoundingMethod <= Rounding::RoundingMethodInvalid || newRoundingMethod >= Rounding::RoundingMethodLast) ? inttostring((int)Rounding::CONVENTIONAL_ROUNDING) : inttostring(newRoundingMethod)); }

		int getDefaultDiscountType() const { int ddiscType = toIntCfg(fieldOffsetCfgFSDiscountType); return (ddiscType > Default_dept_type && ddiscType < INVALID_DISCOUNT_TYPE) ? ddiscType : Discount_price_by_weight; }
		void setDefaultDiscountType(int newDiscountType) { if (newDiscountType > Default_dept_type && newDiscountType < INVALID_DISCOUNT_TYPE) setCfg(fieldOffsetCfgFSDiscountType, inttostring(newDiscountType)); }

		int getDefaultDiscountFormula() const { int discFormula = toIntCfg(fieldOffsetCfgFSDiscountFormula); return (discFormula >= RII_FS_FORMULA && discFormula < INVALID_DISCOUNT_FORMULA) ? discFormula : IBM_FS_FORMULA; }
		void setDefaultDiscountFormula(int newDiscountFormula) { if (newDiscountFormula >= RII_FS_FORMULA && newDiscountFormula < INVALID_DISCOUNT_FORMULA) setCfg(fieldOffsetCfgFSDiscountFormula, inttostring(newDiscountFormula)); }


/*****************************************************************************
 *  CfgPrintedFields
 ****************************************************************************/

		void setWeightOnByCountPrinted(bool printed) { setCfg(fieldOffsetCfgPFWeightOnByCountPrinted, Bool::toString(printed)); if (toBoolCfg(fieldOffsetCfgPFWeightOnByCountPrinted) && toBoolCfg(fieldOffsetCfgPFOneOnByCountPrinted)) setCfg(fieldOffsetCfgPFOneOnByCountPrinted, Bool::toString(false));}

		void setOneOnByCountPrinted(bool printed) { setCfg(fieldOffsetCfgPFOneOnByCountPrinted, Bool::toString(printed)); if (toBoolCfg(fieldOffsetCfgPFOneOnByCountPrinted) && toBoolCfg(fieldOffsetCfgPFWeightOnByCountPrinted)) setCfg(fieldOffsetCfgPFWeightOnByCountPrinted, Bool::toString(false));}
		

/*****************************************************************************
 *  CfgSelfService
 ****************************************************************************/

		int getGridXElements() const {return cfgSelfService.getGridXElements();}
		void setGridXElements(int num) {cfgSelfService.setGridXElements(num);}

		int getGridYElements() const {return cfgSelfService.getGridYElements();}
		void setGridYElements(int num) {cfgSelfService.setGridYElements(num);}

		int getNumOfMostCommonCategories() const {return cfgSelfService.getNumOfMostCommonCategories();}
		void setNumOfMostCommonCategories(int num) {cfgSelfService.setNumOfMostCommonCategories(num);}

		int getImageTypeOnStartScreen() const {return cfgSelfService.getImageTypeOnStartScreen();}
		void setImageTypeOnStartScreen(int type) {cfgSelfService.setImageTypeOnStartScreen(type);}

		HobRecordNum getInitialStreamingGraphicNum() const {return cfgSelfService.getInitialStreamingGraphicNum();}
		void setInitialStreamingGraphicNum(HobRecordNum num) {cfgSelfService.setInitialStreamingGraphicNum(num);}

		int getProductStatsHoursMethod() const {return cfgSelfService.getProductStatsHoursMethod();}
		void setProductStatsHoursMethod(int method) {cfgSelfService.setProductStatsHoursMethod(method);}

		int getSelfServeInactivityTimeout() const {return cfgSelfService.getSelfServeInactivityTimeout();}
		void setSelfServeInactivityTimeout(int timeout) {cfgSelfService.setSelfServeInactivityTimeout(timeout);}
		
		const HobString &getLogoGraphicFileName() const {return cfgSelfService.getLogoGraphicFileName();}
		void setLogoGraphicFileName(const HobString &name) {cfgSelfService.setLogoGraphicFileName(name);}

		const HobString &getNoGraphicAvailableFileName1() const {return cfgSelfService.getNoGraphicAvailableFileName1();}
		void setNoGraphicAvailableFileName1(const HobString &name) {cfgSelfService.setNoGraphicAvailableFileName1(name);}

		const HobString &getNoGraphicAvailableFileName2() const {return cfgSelfService.getNoGraphicAvailableFileName2();}
		void setNoGraphicAvailableFileName2(const HobString &name) {cfgSelfService.setNoGraphicAvailableFileName2(name);}

		const HobString &getPreviousGraphicFileName1() const {return cfgSelfService.getPreviousGraphicFileName1();}
		void setPreviousGraphicFileName1(const HobString &name) {cfgSelfService.setPreviousGraphicFileName1(name);}

		const HobString &getPreviousGraphicFileName2() const {return cfgSelfService.getPreviousGraphicFileName2();}
		void setPreviousGraphicFileName2(const HobString &name) {cfgSelfService.setPreviousGraphicFileName2(name);}

		const HobString &getNextGraphicFileName1() const {return cfgSelfService.getNextGraphicFileName1();}
		void setNextGraphicFileName1(const HobString &name) {cfgSelfService.setNextGraphicFileName1(name);}

		const HobString &getNextGraphicFileName2() const {return cfgSelfService.getNextGraphicFileName2();}
		void setNextGraphicFileName2(const HobString &name) {cfgSelfService.setNextGraphicFileName2(name);}


/*****************************************************************************
 *  CfgNetwork
 ****************************************************************************/

		int getScaleId() const {return cfgNetwork.getScaleId();}
		void setScaleId(int id) {cfgNetwork.setScaleId(id);}
		const HobIpAddress &getIpAddress() const {return cfgNetwork.getIpAddress();}
		int getWellKnownPort() const {return cfgNetwork.getWellKnownPort();}
		void setWellKnownPort(int port) {cfgNetwork.setWellKnownPort(port);}
		const HobString &getDnsHostName() const {return cfgNetwork.getDnsHostName();}
		int getExternalCommSetting() const {return cfgNetwork.getExternalCommSetting();}
		void setExternalCommSetting(int value) {cfgNetwork.setExternalCommSetting(value);}
		const HobIpAddress &getServerIpAddress() const {return cfgNetwork.getServerIpAddress();}
		void setServerIpAddress(HobString address) {cfgNetwork.setIPAddress(fieldOffsetCfgNetServerIpAddress, address);}
		const HobString &getServerHostName() const {return cfgNetwork.getServerHostName();}
		void setServerHostName(HobString name) {cfgNetwork.setServerHostName(name);}
		const HobIpAddress &getServerAppIpAddress() const {return cfgNetwork.getServerAppIpAddress();}
		void setServerAppIpAddress(HobString address) {cfgNetwork.setIPAddress(fieldOffsetCfgNetServerAppIpAddress, address);}
		const HobString &getServerAppHostName() const {return cfgNetwork.getServerAppHostName();}
		void setServerAppHostName(HobString name) {cfgNetwork.setServerAppHostName(name);}


/*****************************************************************************
 *  CfgInternal
 ****************************************************************************/

		double getPercentageDivisor() const {return cfgInternal.getPercentageDivisor();}

		HobString getWebPagesDir() const {return cfgInternal.getWebPagesDir();}


		bool isRFIDHWPresent() const {return cfgInternal.isRFIDHWPresent();}
		void setRFIDHWPresent(bool value) {cfgInternal.setRFIDHWPresent(value);}


		// ScaleModel is defined in scaleModel.h
		ScaleModel getScaleModel() const {return (ScaleModel)cfgInternal.toInt(CfgInternal::SCALEMODEL);}
		bool isSelfServiceScale() const;
		bool isPrepackScale() const;
		bool isServiceCounterScale() const {return cfgInternal.isServiceCounterScale();}

		void setScaleModel(ScaleModel model) {cfgInternal.setScaleModel(model);}


		short getPercentagePrecision() const {return (short)cfgInternal.toInt(CfgInternal::PERCENTAGEPRECISION);}
		short getMaxLifeDays() const {return (short)cfgInternal.toInt(CfgInternal::MAXLIFEDAYS);}
		short getMaxLifeHours() const {return (short)cfgInternal.toInt(CfgInternal::MAXLIFEHOURS);}
		short getMaxLifeMinutes() const { return (short)cfgInternal.toInt(CfgInternal::MAXLIFEMINUTES); }
		int getMaxBonusPoints() const {return cfgInternal.toInt(CfgInternal::MAXBONUSPOINTS);}
		short getQuarterPoundFor() const {return (short)cfgInternal.toInt(CfgInternal::QUARTERPOUNDFOR);}
		short getHalfPoundFor() const {return (short)cfgInternal.toInt(CfgInternal::HALFPOUNDFOR);}
		short getHundredGramsFor() const {return (short)cfgInternal.toInt(CfgInternal::HUNDREDGRAMSFOR);}
		int getMaxLabelNumber() const {return cfgInternal.toInt(CfgInternal::MAXLABELNUMBER);}

		HobString getServiceLoginId() const {return cfgInternal.toString(CfgInternal::SERVICELOGINID);}
		int getServiceKeyRemoveTimeout() const {return cfgInternal.toInt(CfgInternal::SERVICEKEYREMOVETIMEOUT);}
		HobString getServiceKeyFile() const {return cfgInternal.toString(CfgInternal::SERVICEKEYFILE);}
		HobString getSuperKeyFile() const {return cfgInternal.toString(CfgInternal::SUPERKEYFILE);}
		HobString getSelfServeKeyFile() const {return cfgInternal.toString(CfgInternal::SELFSERVEKEYFILE);}

		HobString getDirectorySeparator() const {return cfgInternal.toString(CfgInternal::DIRECTORYSEPARATOR);}
		HobString getDbLocation() const {return cfgInternal.toString(CfgInternal::DBLOCATION);}
		HobString getDbDefaultsFile() const {return cfgInternal.toString(CfgInternal::DBDEFAULTSFILE);}
		HobString getDbBackupLocation() const {return cfgInternal.toString(CfgInternal::DBBACKUPLOCATION);}
		bool isDbCheckBeforeShutdown() const {return cfgInternal.toBool(CfgInternal::DBCHECKBEFORESHUTDOWN);}

		int getMonitorMemoryInterval() const {return cfgInternal.toInt(CfgInternal::MONITORMEMORYINTERVAL);}

		int getPrimaryConsoleSessionPriority() const {return cfgInternal.toInt(CfgInternal::PRIMARYCONSOLESESSIONPRIORITY);}
		int getRemoteConsoleSessionPriority() const {return cfgInternal.toInt(CfgInternal::REMOTECONSOLESESSIONPRIORITY);}
		int getMaxRemoteConsoleSessions() const {return cfgInternal.toInt(CfgInternal::MAXREMOTECONSOLESESSIONS);}
		short getMaxConsecutiveRTCMessages() const {return (short)cfgInternal.toInt(CfgInternal::MAXCONSECUTIVERTCMESSAGES);}

		bool isEventManagerEnabled() const {return cfgInternal.toBool(CfgInternal::EVENTMANAGERENABLED);}
		bool isSessionManagerEnabled() const {return cfgInternal.toBool(CfgInternal::SESSIONMANAGERENABLED);}
		bool isRealTimeControllerEnabled() const {return cfgInternal.toBool(CfgInternal::REALTIMECONTROLLERENABLED);}
		bool isTimerManagerEnabled() const {return cfgInternal.toBool(CfgInternal::TIMERMANAGERENABLED);}
		bool isVersionManagerEnabled() const {return cfgInternal.toBool(CfgInternal::VERSIONMANAGERENABLED);}
		bool isFakeWeigherManagerEnabled() const {return cfgInternal.toBool(CfgInternal::FAKEWEIGHERMANAGERENABLED);}
		int getFakeWeigherFrequency() const {return cfgInternal.toInt(CfgInternal::FAKEWEIGHERFREQUENCY);}
		bool isFakePrinterManagerEnabled() const {return cfgInternal.toBool(CfgInternal::FAKEPRINTERMANAGERENABLED);}
		bool isFakeCLAManagerEnabled() const {return cfgInternal.toBool(CfgInternal::FAKECLAMANAGERENABLED);}
		bool isFakeWrapperManagerEnabled() const {return cfgInternal.toBool(CfgInternal::FAKEWRAPPERMANAGERENABLED);}
		bool isTestBrowserEnabled() const {return cfgInternal.toBool(CfgInternal::TESTBROWSERENABLED);}
		int getTestBrowserMsgDelay() const {return cfgInternal.toInt(CfgInternal::TESTBROWSERMSGDELAY);}
		bool isTestMessagesEnabled() const {return cfgInternal.toBool(CfgInternal::TESTMESSAGESENABLED);}
		bool isDiagnosticsEnabled() const {return cfgInternal.toBool(CfgInternal::DIAGNOSTICSMANAGERENABLED);}

		short getLabelHorizontalOffset() const {return (short)cfgInternal.toInt(CfgInternal::LABELHORIZONTALOFFSET);}
		unsigned long getPrinterProbeTime() const {return (unsigned long)cfgInternal.toInt(CfgInternal::PRINTERPROBETIME);}
		bool isUpdatePrinterConfig() const {return cfgInternal.toBool(CfgInternal::UPDATEPRINTERCONFIG);}
		bool isLabelFeedHeelFirst() const {return cfgInternal.toBool(CfgInternal::LABELFEEDHEELFIRST);}

		unsigned long getInterruptXferTimeout() const {return (unsigned long)cfgInternal.toInt(CfgInternal::INTERRUPTXFERTIMEOUT);}
		unsigned long getBulkXferTimeout() const {return (unsigned long)cfgInternal.toInt(CfgInternal::BULKXFERTIMEOUT);}
		unsigned long getWeigherProbeTime() const {return (unsigned long)cfgInternal.toInt(CfgInternal::WEIGHERPROBETIME);}
		unsigned long getCustDispProbeTime() const {return (unsigned long)cfgInternal.toInt(CfgInternal::CUSTDISPPROBETIME);}
		unsigned long getLabelApplierProbeTime() const {return (unsigned long)cfgInternal.toInt(CfgInternal::LABELAPPLIERPROBETIME);}
		unsigned long getWrapperProbeTime() const {return (unsigned long)cfgInternal.toInt(CfgInternal::WRAPPERPROBETIME);}

		unsigned long getRFIDDebounceTime() const {return (unsigned long)cfgInternal.toInt(CfgInternal::RFIDDEBOUNCETIME);}
		unsigned long getRFIDComPort() const {return (unsigned long)cfgInternal.toInt(CfgInternal::RFIDCOMPORT);}

		bool doIgnoreLabelFit() const {return cfgInternal.toBool(CfgInternal::IGNORELABELFIT);}
		bool getBulkDrivesOnlyMode() const {return cfgInternal.toBool(CfgInternal::USBBULKDRIVEONLYMODE);}

		HobString getCompactFlashDriveName() const {return cfgInternal.toString(CfgInternal::COMPACTFLASHDRIVENAME);}
		HobString getUSBBulkDriveName() const {return cfgInternal.toString(CfgInternal::USBBULKDRIVENAME);}
		HobString getPermanentRamDriveName() const {return cfgInternal.toString(CfgInternal::PERMANENTRAMDRIVENAME);}
		HobString getBootParametersFileName() const {return cfgInternal.toString(CfgInternal::BOOTPARAMETERSFILENAME);}
		HobString getWebPagesRootDir() const {return cfgInternal.toString(CfgInternal::WEBPAGESROOTDIR);}
		HobString getVersionIndependentBackupFileName() const {return cfgInternal.toString(CfgInternal::VERSIONINDEPENDENTBACKUPFILE);}
		bool isAutoLoadVersionIndependentData() const {return cfgInternal.toBool(CfgInternal::AUTOLOADVERSIONINDEPENDENTDATA);}

		int getCameraSystemPredictWaitTime() const { return cfgInternal.toInt(CfgInternal::CAMERASYSTEMPREDICTWAITTIME); }
		int getCameraSystemPredictLoops() const { return cfgInternal.toInt(CfgInternal::CAMERASYSTEMPREDICTLOOPS); }
		bool getCameraSystemAllowedInOperate() const { return cfgInternal.toBool(CfgInternal::CAMERASYSTEMALLOWEDOPERATE); }
		bool getCameraSystemAllowedInItemScreen() const { return cfgInternal.toBool(CfgInternal::CAMERASYSTEMALLOWEDITEMSCREEN); }

/*****************************************************************************
 *  CfgEmail
 ****************************************************************************/

		const HobString &getFrom() const {return cfgEmail.getFrom();}
		void setFrom(const HobString &val) {cfgEmail.setFrom(val);}

		const HobString &getToList() const {return cfgEmail.getToList();}
		void setToList(const HobString &val) {cfgEmail.setToList(val);}

		const HobIpAddress &getSmtpIpAddrOrDnsName() const {return cfgEmail.getSmtpIpAddrOrDnsName();}
		void setSmtpIpAddrOrDnsName(const HobString &val) {cfgEmail.setSmtpIpAddrOrDnsName(val);}


/*****************************************************************************
 *  CfgSynchronization
 ****************************************************************************/

		ScaleSynchronizationType getScaleSynchronizationType() const { return (ScaleSynchronizationType)toIntCfg(fieldOffsetCfgScaleSynchronizationType); }
		void setScaleSynchronizationType(ScaleSynchronizationType type) { setCfg(fieldOffsetCfgScaleSynchronizationType, inttostring((int)type)); }

		HobIpAddress getPrimaryScale() const { return HobIpAddress(toStringCfg(fieldOffsetCfgPrimaryScale)); }
		void setPrimaryScale(const std::string& primary) { setCfg(fieldOffsetCfgPrimaryScale, primary); }
		void setPrimaryScale(const HobIpAddress& ip) { std::string primary; ip.getAsString(primary); setCfg(fieldOffsetCfgPrimaryScale, primary); }


/*****************************************************************************
 *  CfgFTP
 ****************************************************************************/

		const HobIpAddress getFTPServerIPorHostName()
		{
			HobString tempAddress = toStringCfg(fieldOffsetCfgFTPServerIporHostName);
			HobIpAddress newAddress(tempAddress);
			return newAddress;
		}



/*****************************************************************************
 *  CfgCommInterface
 ****************************************************************************/

		bool isClearProductClassTotalsClearsTransactions() const { return cfgCommInterface.isClearProductClassTotalsClearsTransactions(); }
		void setClearProductClassTotalsClearsTransaction(bool set) { cfgCommInterface.setClearProductClassTotalsClearsTransaction(set); }

		bool isClearHourlyTotalsClearsTransactions() const { return cfgCommInterface.isClearHourlyTotalsClearsTransactions(); }
		void setClearHourlyTotalsClearsTransaction(bool set) { cfgCommInterface.setClearHourlyTotalsClearsTransaction(set); }

		bool isClearOperatorTotalsClearsTransactions() const { return cfgCommInterface.isClearOperatorTotalsClearsTransactions(); }
		void setClearOperatorTotalsClearsTransaction(bool set) { cfgCommInterface.setClearOperatorTotalsClearsTransaction(set); }

		bool isClearCOOLTotalsClearsTransactions() const { return cfgCommInterface.isClearCOOLTotalsClearsTransactions(); }
		void setClearCOOLTotalsClearsTransactions(bool set) { cfgCommInterface.setClearCOOLTotalsClearsTransactions(set); }

		bool isClearOperatorChangesClearsTransactions() const { return cfgCommInterface.isClearOperatorChangesClearsTransactions(); }
		void setClearOperatorChangesClearsTransactions(bool set) { cfgCommInterface.setClearOperatorChangesClearsTransactions(set); }



/*****************************************************************************
 *  CfgPrepack
 ****************************************************************************/

		void setPluSearchRange(int range) { if ( range >= 0 ) setCfg(fieldOffsetCfgPluSearchRange, inttostring(range)); }

		//	HA-79:  Add the Rotation override settings to Configuration.
		RotationOverrideSettings getRotationOverrideSetting1() const { return (RotationOverrideSettings) toIntCfg(fieldOffsetCfgRotationOverrideSetting1); }
		void setRotationOverrideSetting1(RotationOverrideSettings value) { setCfg(fieldOffsetCfgRotationOverrideSetting1, inttostring(value)); }
		void setRotationOverrideSetting1(const string &value)  { setCfg(fieldOffsetCfgRotationOverrideSetting1, value); }

		RotationOverrideSettings getRotationOverrideSetting2() const { return (RotationOverrideSettings) toIntCfg(fieldOffsetCfgRotationOverrideSetting2); }
		void setRotationOverrideSetting2(RotationOverrideSettings value) { setCfg(fieldOffsetCfgRotationOverrideSetting2, inttostring(value)); }
		void setRotationOverrideSetting2(const string &value)  { setCfg(fieldOffsetCfgRotationOverrideSetting2, value); }

		RotationOverrideSettings getRotationOverrideSetting3() const { return (RotationOverrideSettings) toIntCfg(fieldOffsetCfgRotationOverrideSetting3); }
		void setRotationOverrideSetting3(RotationOverrideSettings value) { setCfg(fieldOffsetCfgRotationOverrideSetting3, inttostring(value)); }
		void setRotationOverrideSetting3(const string &value)  { setCfg(fieldOffsetCfgRotationOverrideSetting3, value); }

		short getRotationOverrideValue1() const { return (short) toIntCfg(fieldOffsetCfgRotationOverrideValue1); }
		void setRotationOverrideValue1(short value) { setCfg(fieldOffsetCfgRotationOverrideValue1, inttostring(value)); }
		void setRotationOverrideValue1(const string &value) { setCfg(fieldOffsetCfgRotationOverrideValue1, value); }

		short getRotationOverrideValue2() const { return (short) toIntCfg(fieldOffsetCfgRotationOverrideValue2); }
		void setRotationOverrideValue2(short value) { setCfg(fieldOffsetCfgRotationOverrideValue2, inttostring(value)); }
		void setRotationOverrideValue2(const string &value) { setCfg(fieldOffsetCfgRotationOverrideValue2, value); }

		short getRotationOverrideValue3() const { return (short) toIntCfg(fieldOffsetCfgRotationOverrideValue3); }
		void setRotationOverrideValue3(short value) { setCfg(fieldOffsetCfgRotationOverrideValue3, inttostring(value)); }
		void setRotationOverrideValue3(const string &value) { setCfg(fieldOffsetCfgRotationOverrideValue3, value); }

		short getPrimaryLabelPrePrint() const { return (short)toIntCfg(fieldOffsetCfgPrimaryLabelStockRotated); }
		void setPrimaryLabelPrePrint(short value) { setCfg(fieldOffsetCfgPrimaryLabelStockRotated, inttostring(value)); }
		void setPrimaryLabelPrePrint(const string &value) { setCfg(fieldOffsetCfgPrimaryLabelStockRotated, value); }

		short getSecondaryLabelPrePrint() const { return (short)toIntCfg(fieldOffsetCfgSecondaryLabelStockRotated); }
		void setSecondaryLabelPrePrint(short value) { setCfg(fieldOffsetCfgSecondaryLabelStockRotated, inttostring(value)); }
		void setSecondaryLabelPrePrint(const string &value) { setCfg(fieldOffsetCfgSecondaryLabelStockRotated, value); }


/*****************************************************************************
 *  CfgMarquee
 ****************************************************************************/

		HobRecordNum getPermanentMarqueeNumber() const {return cfgMarquee.getPermanentMarqueeNumber();}
		void setPermanentMarqueeNumber(HobRecordNum marqueeNumber) {cfgMarquee.setPermanentMarqueeNumber(marqueeNumber);}

		bool isPermanentMarqueeDisplayed() const {return cfgMarquee.isPermanentMarqueeDisplayed();}
		void setPermanentMarqueeDisplayed(bool shouldDisplay) {cfgMarquee.setPermanentMarqueeDisplayed(shouldDisplay);}

		bool isProductMarqueeDisplayed() const {return cfgMarquee.isProductMarqueeDisplayed();}
		void setProductMarqueeDisplayed(bool shouldDisplay) {cfgMarquee.setProductMarqueeDisplayed(shouldDisplay);}

		short getProductDescMarqueeCounter() const {return cfgMarquee.getProductDescMarqueeCounter();}
		void setProductDescMarqueeCounter(short counter) {cfgMarquee.setProductDescMarqueeCounter(counter);}


/*****************************************************************************
 *  CfgCurrency
 ****************************************************************************/

		char getDecimalSeparator() const { return cfgCurrency.getDecimalSeparator(); }
		void setDecimalSeparator(char separator) { cfgCurrency.setDecimalSeparator(separator); }

		CfgCurrency::MonetaryRule getMonetaryRule() const { return cfgCurrency.getMonetaryRule(); }
		void setMonetaryRule(CfgCurrency::MonetaryRule rule) { cfgCurrency.setMonetaryRule(rule); }

		short getTotalPriceTotalDigits() const { return cfgCurrency.getTotalPriceTotalDigits(); }
		void setTotalPriceTotalDigits(short digits) { cfgCurrency.setTotalPriceTotalDigits(digits); }

		short getUnitPriceTotalDigits() const { return cfgCurrency.getUnitPriceTotalDigits(); }
		void setUnitPriceTotalDigits(short digits) { cfgCurrency.setUnitPriceTotalDigits(digits); }

		short getRoundingFactor() const { return cfgCurrency.getRoundingFactor(); }
		void setRoundingFactor(short factor) { cfgCurrency.setRoundingFactor(factor); }

		Rounding::Method getCurrencyRoundingMethod() const { return cfgCurrency.getRoundingMethod(); }
		void setCurrencyRoundingMethod(Rounding::Method method) { cfgCurrency.setRoundingMethod(method); }

		const HobString & getPrimaryMonetarySymbol() const { return cfgCurrency.getPrimaryMonetarySymbol(); }
		void setPrimaryMonetarySymbol(const HobString & symbol) { cfgCurrency.setPrimaryMonetarySymbol(symbol); }

		short getPrimaryMonetaryDecimalDigits() const { return cfgCurrency.getPrimaryMonetaryDecimalDigits(); }
		void setPrimaryMonetaryDecimalDigits(short digits) { cfgCurrency.setPrimaryMonetaryDecimalDigits(digits); }

		const HobString & getSecondaryMonetarySymbol() const { return cfgCurrency.getSecondaryMonetarySymbol(); }
		void setSecondaryMonetarySymbol(const HobString & symbol) { cfgCurrency.setSecondaryMonetarySymbol(symbol); }

		short getSecondaryMonetaryDecimalDigits() const { return cfgCurrency.getSecondaryMonetaryDecimalDigits(); }
		void setSecondaryMonetaryDecimalDigits(short digits) { cfgCurrency.setSecondaryMonetaryDecimalDigits(digits); }

		short getPricePrecision(bool primary = true) const { return cfgCurrency.getPricePrecision(primary); }
		double getPriceDivisor(bool primary = true) const { return cfgCurrency.getPriceDivisor(primary); }

		const HobString &getMonetarySymbol(bool primary = true) const { return cfgCurrency.getMonetarySymbol(primary); }

		HobPrice getMaxUnitPrice() const {return cfgCurrency.getMaxUnitPrice();}
		HobPrice getMaxTotalPrice() const {return cfgCurrency.getMaxTotalPrice();}

		bool isCombinedVirtualEnabled() const { return toBoolCfg(fieldOffsetCfgVirtualCombinePrint); }
		void setCombinedVirtualEnabled(bool enabled) { setCfg(fieldOffsetCfgVirtualCombinePrint, Bool::toString(enabled)); }

		bool isConserveLinerLessEnabled() const { return toBoolCfg(fieldOffsetCfgIsConserveLinerLess); }	//SOF-3875
		void setConserveLinerLessEnabled(bool enabled) { setCfg(fieldOffsetCfgIsConserveLinerLess, Bool::toString(enabled)); } //SOF-3875

		string buildPrintToScreenBarcodeFilename(int filenameCounter) const;

		bool isIgnoreVendorAndProductNumTagsWhenUpcNumTag() const;
		string readProductFieldExclusions();

		void testCfgI();




	protected:

		Configuration();
		~Configuration();


	private:

		/**
		 * @link
		 * @shapeType PatternLink
		 * @pattern <{Singleton}>
		 * @supplierRole Singleton factory
		 */
	    /*# Configuration _configuration; */

		std::vector<CfgItem *> cfgItems_;

		std::map<std::string, int> cfgItemNameMap_;

		bool traceCfg_;

		CfgScale                cfgScale;
		CfgGenericLabelText		cfgGenericLabelText;
		CfgSelfService			cfgSelfService;
		CfgNetwork				cfgNetwork;
		CfgInternal				cfgInternal;
		CfgEmail				cfgEmail;
		CfgCommInterface        cfgCommInterface;
		CfgMarquee				cfgMarquee;
		CfgCurrency             cfgCurrency;

		mutable HobString tempValue_;

		mutable bool updatedPrimaryLocale_;
		mutable bool updatedSecondaryLocale_;

		Util::CSmartCriticalSection  cfgLoadCS_;		
		Util::CSmartCriticalSection  cfgSaveCS_;		
		Util::CSmartCriticalSection  cfgLicensingCS_;

		Graphic storeGraphic;

		LabelType* rfidVoidLabelType;

		bool licensedPackagesInit_;
		std::string licensedPackages_;


		void init();

		void setupLanguages();

		void sendRebootMessage();

		static Configuration * instance;

		static const short TARESELECTION_TARE_A = 1;
		static const short TARESELECTION_TARE_B = 2;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CONFIGURATION_H
