#include "HfcEvolWrapperErrorHistory.h"
#include "EvolWrapperErrorTranslator.h"
#include "Configuration.h"
#include "HobString.h"
#include "DBWrapperErrorHistoryRM.h"
#include "DBAccess.h"

#include "Bool.h"
#include "Integer.h"
#include "Hex.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

	int HfcEvolWrapperErrorHistory::_dataType_ = 0;
	std::string HfcEvolWrapperErrorHistory::_dataTypeName_("HfcEvolWrapperErrorHistory"); 
#endif

HfcEvolWrapperErrorHistory::HfcEvolWrapperErrorHistory(const char *data): EvolWrapperErrorHistory(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	init();
}

HfcEvolWrapperErrorHistory::HfcEvolWrapperErrorHistory(edb::DBWrapperErrorHistory *dbWrapperErrorHistoryRec, DBAccess *access)
	: EvolWrapperErrorHistory()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperErrorHistory", "HfcEvolWrapperErrorHistory(2):  this = " + Hex::toString((long long) this));
	init();

	dbAccess = access;

	if (dbWrapperErrorHistoryRec)
	{
		myDbRecord = dbWrapperErrorHistoryRec;

		setWrapperError(dbWrapperErrorHistoryRec->getErrorCode());
		setPackageLength(dbWrapperErrorHistoryRec->getErrorPkgLength());
		setPackageWidth(dbWrapperErrorHistoryRec->getErrorPkgWidth());
		setPackageHeight(dbWrapperErrorHistoryRec->getErrorPkgHeight());
		setFilmUsed((WrFilmWidthSelection)dbWrapperErrorHistoryRec->getErrorFilmUsed());
		setUsedBestFit(dbWrapperErrorHistoryRec->getErrorBestFit());
		setStretchModified(dbWrapperErrorHistoryRec->getErrorPerNet());
		string tempStr = dbWrapperErrorHistoryRec->getErrorTrayName();
		setTrayName(tempStr);
		tempStr = dbWrapperErrorHistoryRec->getErrorDateTime();
		setDateTime(tempStr);
		Debug::trace("HfcEvolWrapperErrorHistory", "HfcEvolWrapperErrorHistory(2):  myDbRecord = " + Hex::toString((long long) myDbRecord));
	}
}


HfcEvolWrapperErrorHistory::~HfcEvolWrapperErrorHistory()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
	Debug::trace("HfcEvolWrapperErrorHistory", "~HfcEvolWrapperErrorHistory():  this = " + Hex::toString((long long) this));

	if (myDbRecord)
	{
		Debug::trace("HfcEvolWrapperErrorHistory", "~HfcEvolWrapperErrorHistory():  Deleting myDbRecord = " + Hex::toString((long long) myDbRecord));
		delete myDbRecord;
	}
}

void HfcEvolWrapperErrorHistory::init()
{
#if 1
	errorDescription = Integer::toString(getWrapperError());
#else
	//translate the error number to an error description
	errorDescription = EvolWrapperErrorTranslator::getInstance()->translateEvolWrapperError(getWrapperError());
#endif

	dbAccess = 0;
	myDbRecord = 0;
}

const char *HfcEvolWrapperErrorHistory::serialize(int format) const
{
	HfcObject::serializedData = "";
	HobString result;

	Debug::trace("HfcEvolWrapperErrorHistory", "serialize()");
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < fieldOffsetEvolWrapperErrorHistoryEnd)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetWrapperError:
					{
						HfcObject::serializedData.append(Integer::toString(getWrapperError()));
						break;
					}
					case fieldOffsetWrapperErrorDesc:
					{
						HfcObject::serializedData.append(errorDescription);
						break;
					}
					case fieldOffsetPackageLength:
					{
						Configuration::getInstance()->formatDecimalValue(getPackageLength(), result, 1);
						HfcObject::serializedData.append(result);
						break;
					}
					case fieldOffsetPackageWidth:
					{
						Configuration::getInstance()->formatDecimalValue(getPackageWidth(), result, 1);
						HfcObject::serializedData.append(result);
						break;
					}
					case fieldOffsetPackageHeight:
					{
						Configuration::getInstance()->formatDecimalValue(getPackageHeight(), result, 1);
						HfcObject::serializedData.append(result);
						break;
					}
					case fieldOffsetFilmUsed:
					{
						HobString result;
						switch (getFilmUsed())
						{
							case UpperRoll:
								result = "Upper";
								break;
							case LowerRoll:
								result = "Lower";
								break;
							case Automatic:
								result = "Auto";
								break;
							case NoFilmWidthSelected:
								result = "None";
								break;
							default:
								result = "";
								break;
						}
						HfcObject::serializedData.append(result);
						break;
					}
					case fieldOffsetUsedWrapBoxBestFit:
					{
						HfcObject::serializedData.append(Bool::toString(getUsedBestFit()));
						break;
					}
					case fieldOffsetModifiedNetStretch:
					{
						HfcObject::serializedData.append(Bool::toString(getStretchModified()));
						break;
					}
					case fieldOffsetTrayName:
					{
						HfcObject::serializedData.append(getTrayName());
						break;
					}
					case fieldOffsetDateTime:
					{
						HfcObject::serializedData.append(getDateTime());
						break;
					}
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	Debug::trace("HfcEvolWrapperErrorHistory", "serialize():  " + 	HfcObject::serializedData);
	return 	HfcObject::serializedData.c_str();
}

void HfcEvolWrapperErrorHistory::saveRecord()
{
	bool traceOn = Debug::isTracing("HfcEvolWrapperErrorHistory");
	HobString msgDebug;
	if (traceOn)
		msgDebug = "saveEvolWrapperErrorHistory():  ";

	DBWrapperErrorHistory *dbWrapperErrorHistoryRec = 0;
	DBWrapperErrorHistoryRM* wrapperErrorHistoryRM = dbAccess->getWrapperErrorHistoryRM();

	if( !wrapperErrorHistoryRM ) 
		return;

	if (!myDbRecord)
	{
		Debug::trace("HfcEvolWrapperErrorHistory", HobString("saveRecord() - don't have a db record, creating it..."));
		myDbRecord = wrapperErrorHistoryRM->createRecord();
	}

	if ( myDbRecord )
	{			
		myDbRecord->setErrorCode(getWrapperError());
		myDbRecord->setErrorDateTime(getDateTime());
		myDbRecord->setErrorPkgLength(getPackageLength());
		myDbRecord->setErrorPkgWidth(getPackageWidth());
		myDbRecord->setErrorPkgHeight(getPackageHeight());
		myDbRecord->setErrorFilmUsed(getFilmUsed());
		myDbRecord->setErrorBestFit(getUsedBestFit());
		myDbRecord->setErrorPerNet(getStretchModified());
		myDbRecord->setErrorTrayName(getTrayName());

		Debug::trace("HfcEvolWrapperErrorHistory", msgDebug + "Saving WrapperError: " + inttostring(getWrapperError()) + " at " + getDateTime());

		try
		{
			wrapperErrorHistoryRM->saveRecord(dbWrapperErrorHistoryRec);
		}
		catch (DBException &dbe)
		{
			Debug::trace("DBAccess.exception", "HfcEvolWrapperErrorHistory::saveRecord() ********** database exception **********");
			Debug::trace("DBAccess.exception", "     DBException: type:" +
					util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
		}
	}
}

void HfcEvolWrapperErrorHistory::show()
{
	Debug::trace("HfcEvolWrapperErrorHistory", "HfcEvolWrapperErrorHistory: ");
	Debug::trace("HfcEvolWrapperErrorHistory", HobString("wrapperError_: ") + HobString(Integer::toString(getWrapperError())));
	Debug::trace("HfcEvolWrapperErrorHistory", HobString("packageLength_: ") + HobString(Integer::toString(getPackageLength())));
	Debug::trace("HfcEvolWrapperErrorHistory", HobString("packageWidth_: ") + HobString(Integer::toString(getPackageWidth())));
	Debug::trace("HfcEvolWrapperErrorHistory", HobString("packageHeight_: ") + HobString(Integer::toString(getPackageHeight())));
	Debug::trace("HfcEvolWrapperErrorHistory", HobString("filmWidthUsedToWrap: ") + HobString(Integer::toString(getFilmUsed())));
	Debug::trace("HfcEvolWrapperErrorHistory", HobString("usedWrapBoxBestFit_: ") + HobString(Bool::toString(getUsedBestFit())));
	Debug::trace("HfcEvolWrapperErrorHistory", HobString("modifiedNetStretch_: ") + HobString(Bool::toString(getStretchModified())));
	Debug::trace("HfcEvolWrapperErrorHistory", HobString("trayName_: ") + getTrayName());


}