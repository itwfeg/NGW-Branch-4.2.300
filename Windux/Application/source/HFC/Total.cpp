#include "Total.h"


#include "TotalsTags.h"


#include "Configuration.h"
#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "Integer.h"
#include "Long.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
	int Total::dataType = 0;
	std::string Total::dataTypeName("hfc::Total"); 
#endif


	Total::Total()
	{
	#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
	#endif

		init();
	}


	Total::~Total()
	{
	#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
	#endif
	}


	void Total::init()
	{
		errorCode_    = RECORD_NOT_FOUND;
		flags_        = 0;
		weight_       = 0;
		price_        = 0;
		packages_     = 0;
		runs_         = 0;
		items_        = 0;
		transactions_ = 0;
		flagsSet_     = false;
	}


	void Total::update(const Total *totalRec)
	{
		if (totalRec->getErrorCode() == RECORD_OK)
		{
			Debug::trace("Total", "update() BEFORE");
			show();

			weight_	 += totalRec->getWeight();
			price_	 += totalRec->getPrice();
			packages_ += totalRec->getPackages();
			runs_	 += totalRec->getRuns();
			items_	 += totalRec->getItems();
			if (getErrorCode() != RECORD_OK)
				setErrorCode(RECORD_OK);

			Debug::trace("Total", "update() AFTER");
			show();
		}
	}


	void Total::voidTotal(const Total *totalRec)
	{
		weight_ -= totalRec->getWeight();
		if (weight_ < 0)
			weight_ = 0;

		price_ -= totalRec->getPrice();
		if (price_ < 0)
			price_ = 0;

		packages_ -= totalRec->getPackages();
		if (packages_ < 0)
			packages_ = 0;

		runs_ -= totalRec->getRuns();
		if (runs_ < 0)
			runs_ = 0;

		items_ -= totalRec->getItems();
		if (items_ < 0)
			items_ = 0;
	}


	void Total::updateField(int fieldCode, const HobString &token)
	{
		if (fieldCode <= 0)
			return;

		bool foundField = true;
		switch (fieldCode)
		{
			case fieldOffsetTotalWeight:
			{
				setWeight( MetSig::determineImplicitDecimalWeight(token) );
				break;
			}

			case fieldOffsetTotalPrice:
			{
				setPrice( MetSig::determineImplicitDecimalPrice(token) );
				break;
			}

			case fieldOffsetTotalPackages:
				setPackages(Integer::valueOf(token));
				break;

			case fieldOffsetTotalRuns:
				setRuns(Integer::valueOf(token));
				break;

			case fieldOffsetTotalItems:
				setItems(Integer::valueOf(token));
				break;

			case fieldOffsetTotalFlags:
				setFlags(Integer::valueOf(token));
				break;

			default:
				foundField = false;
				break;
		}

		if (foundField)
			setReceived(fieldCode, true);
	}


	void Total::getFieldAsString(int fieldCode, HobString &result, int format) const
	{
		result = "";
		Configuration *config = Configuration::getInstance();
		if (!config)
			return;

		switch (fieldCode)
		{
			case fieldOffsetTotalWeight:
				switch (format)
				{
					case UltimaTaggedFormat:
					case EvolutionTaggedFormat:
						if ( 2 == config->getExternalWeightPrecision() )
							result = Long::toString( Rounding::roundWeightToTwoImpliedDecimalPlaces(weight_) );
						else
							result = Long::toString(weight_);
						break;
					case DBIndependentFormatFormat:
						result = Long::toString(weight_);
						break;
					case BrowserFormat:
					case BrowserFormatPLUOperateMode:
						config->formatWeight(weight_, result);
						break;
					default:
						config->formatWeight(weight_, result, false);
						break;
				}
				break;
			case fieldOffsetTotalPrice:
				switch (format)
				{
					case UltimaTaggedFormat:
					case EvolutionTaggedFormat:
					case DBIndependentFormatFormat:
						result = Long::toString(price_);
						break;

					case BrowserFormat:
					case BrowserFormatPLUOperateMode:
						result = config->formatHobPrice(price_);
						break;

					default:
						result = config->formatHobPrice(price_, false);
						break;
				}
				break;
			case fieldOffsetTotalPackages:
				result = Integer::toString(packages_);
				break;
			case fieldOffsetTotalRuns:
				result = Integer::toString(runs_);
				break;
			case fieldOffsetTotalItems:
				result = Integer::toString(items_);
				break;
			case fieldOffsetTotalFlags:
				result = Integer::toString(flags_);
				break;

			default:
				break;
		}
	}


	const char *Total::serialize(int format) const
	{
		// do not initialize serializedData in here; it was initialized by the parent's class serialize method

		switch (format)
		{
			case BrowserFormat:
				for (int loopCnt = 1; loopCnt < endTotalFieldOffsets; loopCnt++)
				{
					if (loopCnt > 1)
						serializedData.append(BrowserFormatSeparator);
				}
				break;

			default:
				serializedData.append("Total::serialize - unknown format:").append(Integer::toString(format));
				// todo - throw exception("invalid format");
				break;
		}
		return serializedData.data();
	}


	void Total::show()
	{
		if (Debug::isTracing("Total"))
		{
			HobString myDebug("*** Total parameters ***");
			myDebug.append("\n errorCode = ").append(Integer::toString(errorCode_));
			myDebug.append("\n flags     = ").append(Integer::toString(flags_));
			myDebug.append("\n weight    = ").append(Integer::toString(weight_));
			myDebug.append("\n price     = ").append(Integer::toString(price_));
			myDebug.append("\n packages  = ").append(Integer::toString(packages_));
			myDebug.append("\n runs      = ").append(Integer::toString(runs_));
			myDebug.append("\n items     = ").append(Integer::toString(items_));
			Debug::trace("Total", myDebug);
		}
	}


}
