#include "CfgCommInterface.h"


#include "Configuration.h"
#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"
#include "DBQuery.h"
#include "DBConfigurationsRM.h"

#include "MultiLanguageText.h"

#include "Bool.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif



#ifdef USE_ACCOUNTANT
int CfgCommInterface::dataType = 0;
std::string CfgCommInterface::dataTypeName("CfgCommInterface"); 
#endif


FieldOffsetsList CfgCommInterface::_commConfigOffsets_;


CfgCommInterface::CfgCommInterface()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}


#if defined(LINUX_FIX)
CfgCommInterface::CfgCommInterface(const CfgCommInterface& cfgComm)
    : HfcObject()
#else
CfgCommInterface::CfgCommInterface(const CfgCommInterface& cfgComm)
#endif
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	clearProductClassTotalsClearsTransactions_ = cfgComm.isClearProductClassTotalsClearsTransactions();
	clearHourlyTotalsClearsTransactions_       = cfgComm.isClearHourlyTotalsClearsTransactions();
	clearOperatorTotalsClearsTransactions_     = cfgComm.isClearOperatorTotalsClearsTransactions();
	clearCOOLTotalsClearsTransactions_         = cfgComm.isClearCOOLTotalsClearsTransactions();
	clearOperatorChangesClearsTransactions_    = cfgComm.isClearOperatorChangesClearsTransactions();
}


CfgCommInterface::CfgCommInterface(const char * data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			FieldOffsetsList::iterator fieldIter = _commConfigOffsets_.begin();

			while ( tok.next(&token) != -1 && fieldIter != _commConfigOffsets_.end() )
			{
				if ( token.length() )
					/*int result =*/ updateField(*fieldIter, token);

				++fieldIter;
			}

			break;
		}
		default:
			// todo - throw exception("invalid format");
			Debug::trace("Configuration.CfgCommInterface", "CfgCommInterface():  No support for this format! " + HobString(Integer::toString(formatType)));
			break;
	}
}


CfgCommInterface::~CfgCommInterface()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


void CfgCommInterface::init()
{
	clearProductClassTotalsClearsTransactions_ = false;
	clearHourlyTotalsClearsTransactions_ = false;
	clearOperatorTotalsClearsTransactions_ = false;
	clearCOOLTotalsClearsTransactions_ = false;
	clearOperatorChangesClearsTransactions_ = false;

	initFieldList();
}


void CfgCommInterface::initFieldList()
{
	if ( _commConfigOffsets_.empty() )
	{
		_commConfigOffsets_.push_back(fieldOffsetCfgCommProductClassTotalsClearTransactions);
		_commConfigOffsets_.push_back(fieldOffsetCfgCommHourlyTotalsClearTransactions);
		_commConfigOffsets_.push_back(fieldOffsetCfgCommOperatorTotalsClearTransactions);
		_commConfigOffsets_.push_back(fieldOffsetCfgCommCOOLTotalsClearTransactions);
		_commConfigOffsets_.push_back(fieldOffsetCfgCommOperatorChangesClearTransactions);

		ConfigurationTags configurationTags(&_commConfigOffsets_);
	}
}


void CfgCommInterface::load(DBAccess * dbAccess)
{
	std::string dbVal;

	if ( !dbAccess ) return;


	if ( loadField(dbAccess, "ClearProdClassTotsClearsTrans", dbVal) )
		clearProductClassTotalsClearsTransactions_ = Bool::valueOf(dbVal);

	if ( loadField(dbAccess, "ClearHourlyTotsClearsTrans", dbVal) )
		clearHourlyTotalsClearsTransactions_ = Bool::valueOf(dbVal);

	if ( loadField(dbAccess, "ClearOperatorTotsClearsTrans", dbVal) )
		clearOperatorTotalsClearsTransactions_ = Bool::valueOf(dbVal);

	if ( loadField(dbAccess, "ClearCOOLTotsClearsTrans", dbVal) )
		clearCOOLTotalsClearsTransactions_ = Bool::valueOf(dbVal);

	if ( loadField(dbAccess, "ClearOperChangesClearsTrans", dbVal) )
		clearOperatorChangesClearsTransactions_ = Bool::valueOf(dbVal);

	saveNeeded = false;
}


void CfgCommInterface::updateField(int fieldCode, const HobString & token)
{
	bool foundField = true;

	if ( fieldCode <= 0 ) return;


	if ( Debug::isTracing("Configuration.CfgCommInterface") )
		Debug::trace("Configuration.CfgCommInterface", "updateField() fieldCode = " + util_string::inttostring(fieldCode) +
				" token = " + token + "*");

	switch ( fieldCode )
	{
		case fieldOffsetCfgCommProductClassTotalsClearTransactions:
			clearProductClassTotalsClearsTransactions_ = Bool::valueOf(token);
			break;

		case fieldOffsetCfgCommHourlyTotalsClearTransactions:
			clearHourlyTotalsClearsTransactions_ = Bool::valueOf(token);
			break;

		case fieldOffsetCfgCommOperatorTotalsClearTransactions:
			clearOperatorTotalsClearsTransactions_ = Bool::valueOf(token);
			break;

		case fieldOffsetCfgCommCOOLTotalsClearTransactions:
			clearCOOLTotalsClearsTransactions_ = Bool::valueOf(token);
			break;

		case fieldOffsetCfgCommOperatorChangesClearTransactions:
			clearOperatorChangesClearsTransactions_ = Bool::valueOf(token);
			break;

		default:
			foundField = false;
			break;
	}

	if ( Debug::isTracing("Configuration.CfgCommInterface") )
		Debug::trace("Configuration.CfgCommInterface", "updateField() foundField = " + util_string::booltostring(foundField));

	if ( foundField )
		setReceived(fieldCode, true);
}


void CfgCommInterface::update(const CfgBase * newValues, bool scaleOnlyFields)
{
	Debug::trace("Configuration.CfgCommInterface", "update() newValues = " + ptrtostring(newValues));

	const CfgCommInterface * myNewValues = dynamic_cast<const CfgCommInterface *>(newValues);
	if ( !myNewValues ) return;


	short loopCnt = fieldOffsetCfgCommProductClassTotalsClearTransactions;
	while ( loopCnt < endCfgCommFieldOffsets )
	{
		if ( myNewValues->isReceived(loopCnt) && (!scaleOnlyFields || isFieldScaleOnly(loopCnt)) )
		{
			switch ( loopCnt )
			{

				case fieldOffsetCfgCommProductClassTotalsClearTransactions:
					setClearProductClassTotalsClearsTransaction(myNewValues->isClearProductClassTotalsClearsTransactions());
					break;

				case fieldOffsetCfgCommHourlyTotalsClearTransactions:
					setClearHourlyTotalsClearsTransaction(myNewValues->isClearHourlyTotalsClearsTransactions());
					break;

				case fieldOffsetCfgCommOperatorTotalsClearTransactions:
					setClearOperatorTotalsClearsTransaction(myNewValues->isClearOperatorTotalsClearsTransactions());
					break;

				case fieldOffsetCfgCommCOOLTotalsClearTransactions:
					setClearCOOLTotalsClearsTransactions(myNewValues->isClearCOOLTotalsClearsTransactions());
					break;

				case fieldOffsetCfgCommOperatorChangesClearTransactions:
					setClearOperatorChangesClearsTransactions(myNewValues->isClearOperatorChangesClearsTransactions());
					break;

				default:
					break;
			}
		}

		loopCnt++;
	}
}


void CfgCommInterface::save(DBAccess * dbAccess, const CfgBase * newValues, bool scaleOnlyFields)
{
	bool tracing = Debug::isTracing("Configuration.CfgCommInterface");

	if ( newValues )
		update(newValues, scaleOnlyFields);


	if ( tracing )
		Debug::trace("Configuration.CfgCommInterface", "save():   saveNeeded = " + util_string::booltostring(saveNeeded));
	
	if ( saveNeeded && dbAccess )
	{
		Debug::trace("Configuration.CfgCommInterface", "saving data ...");

		saveField(dbAccess, "ClearProdClassTotsClearsTrans", Bool::toString(clearProductClassTotalsClearsTransactions_));
		saveField(dbAccess, "ClearHourlyTotsClearsTrans", Bool::toString(clearHourlyTotalsClearsTransactions_));
		saveField(dbAccess, "ClearOperatorTotsClearsTrans", Bool::toString(clearOperatorTotalsClearsTransactions_));
		saveField(dbAccess, "ClearCOOLTotsClearsTrans", Bool::toString(clearCOOLTotalsClearsTransactions_));
		saveField(dbAccess, "ClearOperChangesClearsTrans", Bool::toString(clearOperatorChangesClearsTransactions_));


		if ( tracing )
		{
			Debug::trace("Configuration.CfgCommInterface",
				" ClearProductClassTotalsClearsTransactions = " + util_string::booltostring(clearProductClassTotalsClearsTransactions_) +
				" ClearHourlyTotalsClearsTransactions = " + util_string::booltostring(clearHourlyTotalsClearsTransactions_) +
				" ClearOperatorTotalsClearsTransactions = " + util_string::booltostring(clearOperatorTotalsClearsTransactions_) +
				" ClearCOOLTotalsClearsTransactions = " + util_string::booltostring(clearCOOLTotalsClearsTransactions_) +
				" ClearOperatorChangesClearsTransactions = " + util_string::booltostring(clearOperatorChangesClearsTransactions_));
		}

		saveNeeded = false;
	}
	else 
		if ( tracing )
			Debug::trace("Configuration.CfgCommInterface", "save method called, but no fields modified");
}


void CfgCommInterface::operator=(const CfgCommInterface & cfgComm)
{
	clearProductClassTotalsClearsTransactions_ = cfgComm.isClearProductClassTotalsClearsTransactions();
	clearHourlyTotalsClearsTransactions_       = cfgComm.isClearHourlyTotalsClearsTransactions();
	clearOperatorTotalsClearsTransactions_     = cfgComm.isClearOperatorTotalsClearsTransactions();
	clearCOOLTotalsClearsTransactions_         = cfgComm.isClearCOOLTotalsClearsTransactions();
	clearOperatorChangesClearsTransactions_    = cfgComm.isClearOperatorChangesClearsTransactions();
}


void CfgCommInterface::getFieldAsString(int fieldCode, HobString &result, int /* format */) const
{
//	Right now we don't do anything with format but keep it in case we need it.
	result.clear();

	switch ( fieldCode )
	{
		case fieldOffsetCfgCommProductClassTotalsClearTransactions:
			result.assign(Bool::toString(clearProductClassTotalsClearsTransactions_));
			break;

		case fieldOffsetCfgCommHourlyTotalsClearTransactions:
			result.assign(Bool::toString(clearHourlyTotalsClearsTransactions_));
			break;

		case fieldOffsetCfgCommOperatorTotalsClearTransactions:
			result.assign(Bool::toString(clearOperatorTotalsClearsTransactions_));
			break;

		case fieldOffsetCfgCommCOOLTotalsClearTransactions:
			result.assign(Bool::toString(clearCOOLTotalsClearsTransactions_));
			break;

		case fieldOffsetCfgCommOperatorChangesClearTransactions:
			result.assign(Bool::toString(clearOperatorChangesClearsTransactions_));
			break;

		default:
			break;
	}

	Debug::trace("Configuration.CfgCommInterface", "getFieldAsString:  fieldCode = " + util_string::inttostring(fieldCode) +
		"    result = " + result + "*");
}


const HobString& CfgCommInterface::getField(int fieldCode, int format) const
{
	getFieldAsString(fieldCode, tempValue_, format);

	return tempValue_;
}


const char *CfgCommInterface::serialize(int format) const
{
	serializedData.clear();

	if ( getFormatType() != UnknownFormat )
		format = getFormatType();


	switch ( format )
	{
		case BrowserFormat:
		{
			HobString fieldData;
			bool appendSeparator = false;
			FieldOffsetsList::iterator fieldIter = _commConfigOffsets_.begin();

			while ( fieldIter != _commConfigOffsets_.end() )
			{
				if ( appendSeparator )
					serializedData.append(BrowserFormatSeparator);
				else
					appendSeparator = true;

				getFieldAsString(*fieldIter, fieldData, format);

				serializedData.append(fieldData);

				++fieldIter;
			}

			break;
		}
		case SynchronizationTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag, fieldData;
			FieldOffsetsList::iterator fieldIter = _commConfigOffsets_.begin();

			while ( fieldIter != _commConfigOffsets_.end() )
			{
				tag = configTags.getTag(*fieldIter);
				if ( tag.size() )
				{
					fieldData.clear();
					getFieldAsString(*fieldIter, fieldData, EvolutionTaggedFormat);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}

				++fieldIter;
			}

			break;
		}
		case CommInterfaceReportFormat:
		{
			hfc::MultiLanguageText internationalize;
			ConfigurationTags configTags;

			serializedData.assign(internationalize("ConfigReport.CommInterface")).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCommProductClassTotalsClearTransactions))).append(4, '\t').
					append(yesNoValue(getField(fieldOffsetCfgCommProductClassTotalsClearTransactions, EvolutionTaggedFormat))).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCommHourlyTotalsClearTransactions))).append(1, '\t').
					append(yesNoValue(getField(fieldOffsetCfgCommHourlyTotalsClearTransactions, EvolutionTaggedFormat))).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCommOperatorTotalsClearTransactions))).append(2, '\t').
					append(yesNoValue(getField(fieldOffsetCfgCommOperatorTotalsClearTransactions, EvolutionTaggedFormat))).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCommCOOLTotalsClearTransactions))).append(2, '\t').
					append(yesNoValue(getField(fieldOffsetCfgCommCOOLTotalsClearTransactions, EvolutionTaggedFormat))).append(1, '\n');
			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCommOperatorChangesClearTransactions))).append(2, '\t').
					append(yesNoValue(getField(fieldOffsetCfgCommOperatorChangesClearTransactions, EvolutionTaggedFormat))).append(1, '\n');

			break;
		}
		default:
			serializedData = "CfgCommInterface::serialize - unknown format:" + util_string::inttostring(format);
			// todo - throw exception("invalid format");
			break;
	}

	return serializedData.c_str();
}


void CfgCommInterface::show()
{
	if ( Debug::isTracing("Configuration") )
	{
		Debug::trace("Configuration", "*** CfgCommInterface parameters ***");
		Debug::trace("Configuration", "ClearProductClassTotalsClearsTransactions: " + std::string(Bool::toString(clearProductClassTotalsClearsTransactions_)));
		Debug::trace("Configuration", "ClearHourlyTotalsClearsTransactions: " + std::string(Bool::toString(clearHourlyTotalsClearsTransactions_)));
		Debug::trace("Configuration", "ClearOperatorTotalsClearsTransactions: " + std::string(Bool::toString(clearOperatorTotalsClearsTransactions_)));
		Debug::trace("Configuration", "ClearCOOLTotalsClearsTransactions: " + std::string(Bool::toString(clearCOOLTotalsClearsTransactions_)));
		Debug::trace("Configuration", "ClearOperatorChangesClearsTransactions: " + std::string(Bool::toString(clearOperatorChangesClearsTransactions_)));
	}
}
