#include "CfgCurrency.h"


#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"

#include "MultiLanguageText.h"

#include "Debug.h"

#include "Integer.h"



#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgCurrency::dataType = 0;
std::string CfgCurrency::dataTypeName("CfgCurrency"); 
#endif



std::list<CfgFieldOffsets> CfgCurrency::fieldList_;


CfgCurrency::CfgCurrency()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}


CfgCurrency::CfgCurrency(const char * data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	HobString temp(data, len);
	std::list<CfgFieldOffsets>::iterator fieldIter;

	switch ( formatType )
	{
		case BrowserFormat:
		{
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			std::list<CfgFieldOffsets>::iterator fieldIter = fieldList_.begin();

			while ( tok.next(&token) != -1 && fieldIter != fieldList_.end() )
			{
				if ( token.size() )
				{
					if ( isFieldQuoted(*fieldIter) )
						token.unQuoteForCSV();

					updateField(*fieldIter, token);
				}

				++fieldIter;
			}

			break;
		}

/*		case NormalScaleOpFormat:
		{
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			fieldIter = normalScaleOpList.begin();
			while ((tok.next(&token) != -1) && (fieldIter != normalScaleOpList.end()))
			{
				if (token.size())
				{
					offset = *fieldIter;
					if (isFieldQuoted(offset))
						token.unQuoteForCSV();
					updateField(offset, token);
				}
				fieldIter++;
			}
			break;
		}
*/
		default:
			// todo - throw exception("invalid format");
			break;
	}
}


#if defined(LINUX_FIX)
CfgCurrency::CfgCurrency(const CfgCurrency & cfgScale)
    : HfcObject()
#else
CfgCurrency::CfgCurrency(const CfgCurrency & cfgScale)
#endif
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	decimalSeparator_               = cfgScale.getDecimalSeparator();
	monetaryRule_                   = cfgScale.getMonetaryRule();
	totalPriceTotalDigits_          = cfgScale.getTotalPriceTotalDigits();
	unitPriceTotalDigits_           = cfgScale.getUnitPriceTotalDigits();
	roundingFactor_                 = cfgScale.getRoundingFactor();
	roundingMethod_                 = cfgScale.getRoundingMethod();

	primaryMonetarySymbol_          = cfgScale.getPrimaryMonetarySymbol();
	primaryMonetaryDecimalDigits_   = cfgScale.getPrimaryMonetaryDecimalDigits();

	secondaryMonetarySymbol_        = cfgScale.getSecondaryMonetarySymbol();
	secondaryMonetaryDecimalDigits_ = cfgScale.getSecondaryMonetaryDecimalDigits();
}


CfgCurrency::~CfgCurrency()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


//private
void CfgCurrency::init()
{
	decimalSeparator_ = '.';
	monetaryRule_ = MonetaryRuleLeading;
	totalPriceTotalDigits_ = 5;
	unitPriceTotalDigits_ = 5;
	roundingFactor_ = 1;
	roundingMethod_ = Rounding::CONVENTIONAL_ROUNDING;

	primaryMonetarySymbol_ = "$";
	primaryMonetaryDecimalDigits_ = 2;

	secondaryMonetarySymbol_ = "";
	secondaryMonetaryDecimalDigits_ = 0;

	tempValue_ = "";

	if ( fieldList_.empty() )
		initFieldList();
}


// static
void CfgCurrency::initFieldList()		//added ats 10012002
{
	if ( fieldList_.empty() )
	{
		fieldList_.push_back(fieldOffsetCfgCurrencyDecimalSeparator);
		fieldList_.push_back(fieldOffsetCfgCurrencyMonetaryRule);
		fieldList_.push_back(fieldOffsetCfgCurrencyTotalPriceTotalDigits);
		fieldList_.push_back(fieldOffsetCfgCurrencyUnitPriceTotalDigits);
		fieldList_.push_back(fieldOffsetCfgCurrencyRoundingFactor);
		fieldList_.push_back(fieldOffsetCfgCurrencyRoundingMethod);
		fieldList_.push_back(fieldOffsetCfgCurrencyPrimarySymbol);
		fieldList_.push_back(fieldOffsetCfgCurrencyPrimaryDecimalDigits);
		fieldList_.push_back(fieldOffsetCfgCurrencySecondarySymbol);
		fieldList_.push_back(fieldOffsetCfgCurrencySecondaryDecimalDigits);
	}


/*	if (synchronizationList.empty())
	{
// CfgCurrency
		synchronizationList.push_back(fieldOffsetCfgStoreId);
		synchronizationList.push_back(fieldOffsetCfgStoreName);
		synchronizationList.push_back(fieldOffsetCfgStoreGraphic);
		synchronizationList.push_back(fieldOffsetCfgDeptName_NOT_USED);

		synchronizationList.push_back(fieldOffsetCfgPrimaryCountryCode);
		synchronizationList.push_back(fieldOffsetCfgPrimaryLanguageCode);
		synchronizationList.push_back(fieldOffsetCfgSecondaryCountryCode);
		synchronizationList.push_back(fieldOffsetCfgSecondaryLanguageCode);
		synchronizationList.push_back(fieldOffsetCfgLengthUnits);
		synchronizationList.push_back(fieldOffsetCfgTemperatureUnits);
		synchronizationList.push_back(fieldOffsetCfgCurrencyUnits);
		synchronizationList.push_back(fieldOffsetCfgMonetarySymbol);
		synchronizationList.push_back(fieldOffsetCfgDateFormat);
		synchronizationList.push_back(fieldOffsetCfgTimeFormat);


		ConfigurationTags configurationTags((FieldOffsetsList *)&synchronizationList);
	} */
}


void CfgCurrency::load(DBAccess * dbAccess)
{
	HobString dbVal;


	if ( !dbAccess )
		return;

	if ( loadField(dbAccess, "decimalSeparator", dbVal) )
	{
		if ( dbVal.size() )
			decimalSeparator_ = dbVal[0];
	}
	if ( loadField(dbAccess, "monetaryRule", dbVal) )
		monetaryRule_ = (MonetaryRule)Integer::valueOf(dbVal);
	if ( loadField(dbAccess, "totalPriceTotalDigits", dbVal) )
		totalPriceTotalDigits_ = Integer::valueOf(dbVal);
	if ( loadField(dbAccess, "unitPriceTotalDigits", dbVal) )
		unitPriceTotalDigits_ = Integer::valueOf(dbVal);
	if ( loadField(dbAccess, "roundingFactor", dbVal) )
		roundingFactor_ = Integer::valueOf(dbVal);
	if ( loadField(dbAccess, "roundingMethod", dbVal) )
		roundingMethod_ = (Rounding::Method)Integer::valueOf(dbVal);

	if ( loadField(dbAccess, "primaryMonetarySymbol", dbVal) )
		primaryMonetarySymbol_ = dbVal;
	if ( loadField(dbAccess, "primaryMonetaryDecimalDigits", dbVal) )
		primaryMonetaryDecimalDigits_ = Integer::valueOf(dbVal);

	if ( loadField(dbAccess, "secondaryMonetarySymbol", dbVal) )
		secondaryMonetarySymbol_ = dbVal;
	if ( loadField(dbAccess, "secondaryMonDecimalDigits", dbVal) )
		secondaryMonetaryDecimalDigits_ = Integer::valueOf(dbVal);

	saveNeeded = false;
}


void CfgCurrency::updateField(int fieldCode, const HobString & token)
{
	if ( fieldCode <= 0 )
		return;


	Debug::trace("Configuration.CfgCurrency",
			"updateField() fieldCode = " + util_string::inttostring(fieldCode) + " token = '" + token + "'");


	bool foundField = true;
	switch ( fieldCode )
	{
		case fieldOffsetCfgCurrencyDecimalSeparator:
			if ( token.size() )
			{
				switch ( token[0] )
				{
					case ',' : setDecimalSeparator(','); break;
					case '0' : setDecimalSeparator('.'); break;
					case '1' : setDecimalSeparator(','); break;
					case '.' :
					default  : setDecimalSeparator('.'); break;
				}
			}
			break;
		case fieldOffsetCfgCurrencyMonetaryRule:
		{
			setMonetaryRule((MonetaryRule)util_string::stringtoint(token));
			break;
		}
		case fieldOffsetCfgCurrencyTotalPriceTotalDigits:
			setTotalPriceTotalDigits((short)util_string::stringtoint(token));
			break;
		case fieldOffsetCfgCurrencyUnitPriceTotalDigits:
			setUnitPriceTotalDigits((short)util_string::stringtoint(token));
			break;
		case fieldOffsetCfgCurrencyRoundingFactor:
			setRoundingFactor((short)util_string::stringtoint(token));
			break;
		case fieldOffsetCfgCurrencyRoundingMethod:
			setRoundingMethod((Rounding::Method)util_string::stringtoint(token));
			break;
		case fieldOffsetCfgCurrencyPrimarySymbol:
			setPrimaryMonetarySymbol(token);
			break;
		case fieldOffsetCfgCurrencyPrimaryDecimalDigits:
			setPrimaryMonetaryDecimalDigits((short)util_string::stringtoint(token));
			break;
		case fieldOffsetCfgCurrencySecondarySymbol:
			setSecondaryMonetarySymbol(token);
			break;
		case fieldOffsetCfgCurrencySecondaryDecimalDigits:
			setSecondaryMonetaryDecimalDigits((short)util_string::stringtoint(token));
			break;
		default:
		{
			Debug::trace("Configuration.CfgCurrency", " Don't recognize this fieldCode!");

			foundField = false;

			break;
		}
	}

	if ( foundField )
		setReceived(fieldCode, true);
}


void CfgCurrency::update(const CfgBase *newValues, bool scaleOnlyFields)
{
	Debug::trace("Configuration.CfgCurrency", "update() newValues = " + ptrtostring(newValues));

	const CfgCurrency *myNewValues = dynamic_cast<const CfgCurrency *>(newValues);
	if ( !myNewValues )
		return;


	int loopCnt = fieldOffsetCfgCurrencyDecimalSeparator;
	while ( loopCnt < endCfgCurrencyFieldOffsets )
	{
		if ( myNewValues->isReceived(loopCnt) && (!scaleOnlyFields || isFieldScaleOnly(loopCnt)) )
		{
			switch (loopCnt)
			{
				case fieldOffsetCfgCurrencyDecimalSeparator:
					setDecimalSeparator(myNewValues->getDecimalSeparator());
					break;
				case fieldOffsetCfgCurrencyMonetaryRule:
					setMonetaryRule(myNewValues->getMonetaryRule());
					break;
				case fieldOffsetCfgCurrencyTotalPriceTotalDigits:
					setTotalPriceTotalDigits(myNewValues->getTotalPriceTotalDigits());
					break;
				case fieldOffsetCfgCurrencyUnitPriceTotalDigits:
					setUnitPriceTotalDigits(myNewValues->getUnitPriceTotalDigits());
					break;
				case fieldOffsetCfgCurrencyRoundingFactor:
					setRoundingFactor(myNewValues->getRoundingFactor());
					break;
				case fieldOffsetCfgCurrencyRoundingMethod:
					setRoundingMethod(myNewValues->getRoundingMethod());
					break;
				case fieldOffsetCfgCurrencyPrimarySymbol:
					setPrimaryMonetarySymbol(myNewValues->getPrimaryMonetarySymbol());
					break;
				case fieldOffsetCfgCurrencyPrimaryDecimalDigits:
					setPrimaryMonetaryDecimalDigits(myNewValues->getPrimaryMonetaryDecimalDigits());
					break;
				case fieldOffsetCfgCurrencySecondarySymbol:
					setSecondaryMonetarySymbol(myNewValues->getSecondaryMonetarySymbol());
					break;
				case fieldOffsetCfgCurrencySecondaryDecimalDigits:
					setSecondaryMonetaryDecimalDigits(myNewValues->getSecondaryMonetaryDecimalDigits());
					break;
				default:
					break;
			}
			if ( Debug::isTracing("Configuration.CfgCurrency") )
				Debug::trace("Configuration.CfgCurrency", "update() field received: " + util_string::inttostring(loopCnt));
		}
		else
			if ( Debug::isTracing("Configuration.CfgCurrency") )
				Debug::trace("Configuration.CfgCurrency", "update() field not received: " + util_string::inttostring(loopCnt));

		++loopCnt;
	}
}


void CfgCurrency::save(DBAccess *dbAccess, const CfgBase *newValues, bool scaleOnlyFields)
{
	if ( newValues )
		update(newValues, scaleOnlyFields);

	if ( !dbAccess )
	{
		Debug::trace("Configuration.CfgCurrency", "save() - no dbAccess passed in");

		return;
	}

	Debug::trace("Configuration.CfgCurrency", "save():   saveNeeded = " + util_string::booltostring(saveNeeded));

	if ( saveNeeded )
	{
		Debug::trace("Configuration.CfgCurrency", "saving data ...");


		saveField(dbAccess, "decimalSeparator",                std::string(1, decimalSeparator_));
		saveField(dbAccess, "monetaryRule",                    Integer::toString(monetaryRule_));
		saveField(dbAccess, "totalPriceTotalDigits",           Integer::toString(totalPriceTotalDigits_));
		saveField(dbAccess, "unitPriceTotalDigits",            Integer::toString(unitPriceTotalDigits_));
		saveField(dbAccess, "roundingFactor",                  Integer::toString(roundingFactor_));
		saveField(dbAccess, "roundingMethod",                  Integer::toString(roundingMethod_));

		saveField(dbAccess, "primaryMonetarySymbol",           primaryMonetarySymbol_);
		saveField(dbAccess, "primaryMonetaryDecimalDigits",    Integer::toString(primaryMonetaryDecimalDigits_));

		saveField(dbAccess, "secondaryMonetarySymbol",         secondaryMonetarySymbol_);
		saveField(dbAccess, "secondaryMonDecimalDigits",  Integer::toString(secondaryMonetaryDecimalDigits_));


		saveNeeded = false;
	}
	else
		Debug::trace("Configuration.CfgCurrency", "save method called, but no fields modified, save skipped");
}


CfgCurrency& CfgCurrency::operator=(const CfgCurrency & cfgCurrency)
{
	decimalSeparator_                    = cfgCurrency.getDecimalSeparator();
	monetaryRule_                        = cfgCurrency.getMonetaryRule();
	totalPriceTotalDigits_               = cfgCurrency.getTotalPriceTotalDigits();
	unitPriceTotalDigits_                = cfgCurrency.getUnitPriceTotalDigits();
	roundingFactor_                      = cfgCurrency.getRoundingFactor();
	roundingMethod_                      = cfgCurrency.getRoundingMethod();

	primaryMonetarySymbol_               = cfgCurrency.getPrimaryMonetarySymbol();
	primaryMonetaryDecimalDigits_        = cfgCurrency.getPrimaryMonetaryDecimalDigits();

	secondaryMonetarySymbol_             = cfgCurrency.getSecondaryMonetarySymbol();
	secondaryMonetaryDecimalDigits_      = cfgCurrency.getSecondaryMonetaryDecimalDigits();

	return *this;
}


void CfgCurrency::getFieldAsString(CfgFieldOffsets fieldCode, HobString &result, int format) const
{
//	Right now we don't do anything with format but keep it in case we need it.
	result.clear();

	switch ( fieldCode )
	{
		case fieldOffsetCfgCurrencyDecimalSeparator:
			if ( format == BrowserFormat )
				result = std::string(1, decimalSeparator_);
			else
				result = Integer::toString(decimalSeparator_ == '.' ? 0 : 1);
			break;
		case fieldOffsetCfgCurrencyMonetaryRule:
			result = Integer::toString(monetaryRule_);
			break;
		case fieldOffsetCfgCurrencyTotalPriceTotalDigits:
			result = Integer::toString(totalPriceTotalDigits_);
			break;
		case fieldOffsetCfgCurrencyUnitPriceTotalDigits:
			result = Integer::toString(unitPriceTotalDigits_);
			break;
		case fieldOffsetCfgCurrencyRoundingFactor:
			result = Integer::toString(roundingFactor_);
			break;
		case fieldOffsetCfgCurrencyRoundingMethod:
			result = Integer::toString(roundingMethod_);
			break;
		case fieldOffsetCfgCurrencyPrimarySymbol:
			result = primaryMonetarySymbol_;
			break;
		case fieldOffsetCfgCurrencyPrimaryDecimalDigits:
			result = Integer::toString(primaryMonetaryDecimalDigits_);
			break;
		case fieldOffsetCfgCurrencySecondarySymbol:
			result = secondaryMonetarySymbol_;
			break;
		case fieldOffsetCfgCurrencySecondaryDecimalDigits:
			result = Integer::toString(secondaryMonetaryDecimalDigits_);
			break;
		default:
			break;
	}

	Debug::trace("Configuration.CfgCurrency",
			"  getFieldAsString():  fieldCode = " + util_string::inttostring(fieldCode) + "  result = " + result);
}


const HobString& CfgCurrency::getField(CfgFieldOffsets fieldCode, int format) const
{
	getFieldAsString(fieldCode, tempValue_, format);

	return tempValue_;
}


const char *CfgCurrency::serialize(int format) const
{
	serializedData.clear();

	if ( getFormatType() != UnknownFormat )
		format = getFormatType();

	std::list<CfgFieldOffsets>::iterator fieldIter;
#if defined(LINUX_FIX)
    // appendSeparator: unused variable
#else
	bool appendSeparator = false;
#endif
	HobString fieldData = "";

	switch ( format )
	{
		case BrowserFormat:
		{
			bool appendSeparator = false;
			HobString fieldData;
			std::list<CfgFieldOffsets>::iterator fieldIter = fieldList_.begin();

			while ( fieldIter != fieldList_.end() )
			{
				if ( appendSeparator )
					serializedData.append(BrowserFormatSeparator);
				else
					appendSeparator = true;

				getFieldAsString(*fieldIter, fieldData, format);
				if ( isFieldQuoted(*fieldIter) )
					fieldData.quoteForCSV();

				serializedData.append(fieldData);

				++fieldIter;
			}

			break;
		}

		case LocaleReportFormat:
		{
			hfc::MultiLanguageText internationalize;
			ConfigurationTags configTags;

			serializedData.assign(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCurrencyPrimarySymbol))).append(1, '\t').
					append(getField(fieldOffsetCfgCurrencyPrimarySymbol, EvolutionTaggedFormat)).append(1, '\n');
			serializedData.append(4, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCurrencyPrimaryDecimalDigits))).append(1, '\t').
					append(getField(fieldOffsetCfgCurrencyPrimaryDecimalDigits, EvolutionTaggedFormat)).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCurrencySecondarySymbol))).append(1, '\t').
					append(getField(fieldOffsetCfgCurrencySecondarySymbol, EvolutionTaggedFormat)).append(1, '\n');
			serializedData.append(4, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCurrencySecondaryDecimalDigits))).append(1, '\t').
					append(getField(fieldOffsetCfgCurrencySecondaryDecimalDigits, EvolutionTaggedFormat)).append(2, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCurrencyDecimalSeparator))).append(1, '\t').
					append(getField(fieldOffsetCfgCurrencyDecimalSeparator, EvolutionTaggedFormat)).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCurrencyMonetaryRule))).append(1, '\t').
					append(getField(fieldOffsetCfgCurrencyMonetaryRule, EvolutionTaggedFormat)).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCurrencyRoundingFactor))).append(1, '\t').
					append(getField(fieldOffsetCfgCurrencyRoundingFactor, EvolutionTaggedFormat)).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCurrencyRoundingMethod))).append(1, '\t').
					append(getField(fieldOffsetCfgCurrencyRoundingMethod, EvolutionTaggedFormat)).append(2, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCurrencyTotalPriceTotalDigits))).append(1, '\t').
					append(getField(fieldOffsetCfgCurrencyTotalPriceTotalDigits, EvolutionTaggedFormat)).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCurrencyUnitPriceTotalDigits))).append(1, '\t').
					append(getField(fieldOffsetCfgCurrencyUnitPriceTotalDigits, EvolutionTaggedFormat)).append(1, '\n');

			break;
		}

		case SynchronizationTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = fieldList_.begin();
			while ( fieldIter != fieldList_.end() )
			{
				tag = configTags.getTag(*fieldIter);
				if ( tag.size() )
				{
					fieldData = "";
					getFieldAsString(*fieldIter, fieldData, SynchronizationTaggedFormat);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		default:
			serializedData = HobString("CfgCurrency::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}

	return serializedData.c_str();
}


bool CfgCurrency::isChanged()
{
	Debug::trace("Configuration.CfgCurrency", "isChanged:" + util_string::booltostring(saveNeeded));

	return saveNeeded;
}


void CfgCurrency::show()
{
	if ( Debug::isTracing("Configuration") )
	{
		Debug::trace("Configuration", "*** CfgCurrency parameters ***");
		Debug::trace("Configuration", "decimalSeparator:" + std::string(1, decimalSeparator_));
		Debug::trace("Configuration", "monetaryRule:" + util_string::inttostring(monetaryRule_));
		Debug::trace("Configuration", "totalPriceTotalDigits:" + util_string::inttostring(totalPriceTotalDigits_));
		Debug::trace("Configuration", "unitPriceTotalDigits:" + util_string::inttostring(unitPriceTotalDigits_));
		Debug::trace("Configuration", "roundingFactor:" + util_string::inttostring(roundingFactor_));
		Debug::trace("Configuration", "roundingMethod:" + util_string::inttostring(roundingMethod_));

		Debug::trace("Configuration", "primaryMonetarySymbol:" + primaryMonetarySymbol_);
		Debug::trace("Configuration", "primaryMonetaryDecimalDigits:" + util_string::inttostring(primaryMonetaryDecimalDigits_));

		Debug::trace("Configuration", "secondaryMonetarySymbol_:" + secondaryMonetarySymbol_);
		Debug::trace("Configuration", "secondaryMonetaryDecimalDigits:" + util_string::inttostring(secondaryMonetaryDecimalDigits_));
	}
}


bool CfgCurrency::isFieldQuoted(CfgFieldOffsets fieldOffsetCfg) const
{
	switch ( fieldOffsetCfg )
	{
		case fieldOffsetCfgCurrencyDecimalSeparator:
		case fieldOffsetCfgCurrencyPrimarySymbol:
		case fieldOffsetCfgCurrencySecondarySymbol:
			return true;

		default:
			return false;
	}
}


HobPrice CfgCurrency::getMaxPrice(short digits) const
{
	HobPrice price = 0;
	switch (digits)
	{
		case 3:
			price = 999;
			break;
		case 4:
			price = 9999;
			break;
		case 5:
			price = 99999;
			break;
		case 6:
			price = 999999;
			break;
		case 7:
			price = 9999999;
			break;
		case 8:
			price = 99999999;
			break;
		case 9:
			price = 999999999;
			break;
		default:
			price = 99999;
			break;
	}
	return price;
}

HobPrice CfgCurrency::getMaxUnitPrice() const
{
	return getMaxPrice(unitPriceTotalDigits_);
}

HobPrice CfgCurrency::getMaxTotalPrice() const
{
	return getMaxPrice(totalPriceTotalDigits_);
}
