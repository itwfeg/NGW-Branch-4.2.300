#include "KnifeTimeHistory.h"
#include "DBKnifeTimeHistoryRM.h"
#include "DBKnifeTimeHistory.h"
#include "DBAccess.h"
#include "Configuration.h"
//#include "HobTokenizerCSV.h"
#include "HobString.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int KnifeTimeHistory::_dataType_ = 0;
std::string KnifeTimeHistory::_dataTypeName_("KnifeTimeHistory"); 
#endif


KnifeTimeHistory::KnifeTimeHistory(DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("KnifeTimeHistory", "KnifeTimeHistory()");

	dbAccess = access;
	myDbRecord = 0;
	knifeHistoryType_ = KnifeTimeNone;
	knifeTime_ = 0;
}

KnifeTimeHistory::KnifeTimeHistory(DBKnifeTimeHistory *dbknifeTimeHistoryRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	Debug::trace("KnifeTimeHistory", "KnifeTimeHistory(DBKnifeTimeHistory *)");

	dbAccess = access;
	myDbRecord = dbknifeTimeHistoryRec;

	if ( dbknifeTimeHistoryRec->isRecordValid() )
	{
		setKnifeHistoryType((KnifeHistoryType) stringtoint(dbknifeTimeHistoryRec->getType()));
		setKnifeTime(dbknifeTimeHistoryRec->getKnifeTime());

		HobDateTime tempDate;
		HobString tempDateStr = dbknifeTimeHistoryRec->getTimeStampDateTime();
		tempDate.convertFromDBFormat(&tempDateStr);
		setKnifeDateTime(tempDate);

		setErrorCode(RECORD_OK);
		Debug::trace("KnifeTimeHistory", "KnifeTimeHistory(DBKnifeTimeHistory *):  dbknifeTimeHistoryRec OK");
	}
	else
	{
		setKnifeHistoryType(KnifeTimeNone);
		setKnifeTime(0);

		setErrorCode(RECORD_CORRUPT);
		Debug::trace("KnifeTimeHistory", "KnifeTimeHistory(DBKnifeTimeHistory *):  dbknifeTimeHistoryRec CORRUPT!");
	}
}


KnifeTimeHistory::~KnifeTimeHistory()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
	Debug::trace("KnifeTimeHistory", "~KnifeTimeHistory()");

	if ( myDbRecord )
		delete myDbRecord;
}

void KnifeTimeHistory::setKnifeTime(std::string timeStr)
{
	knifeTime_ = Integer::valueOf(timeStr);
}

HobDateTime KnifeTimeHistory::getKnifeDateTime() const
{
	return knifeDateTime_;
}
void KnifeTimeHistory::setKnifeDateTime(HobDateTime date)
{
	knifeDateTime_ = date;
}

HobErrorCode KnifeTimeHistory::getErrorCode() const
{
	return errorCode;
}
void KnifeTimeHistory::setErrorCode(HobErrorCode errorCode)
{
	this->errorCode = errorCode;
}

KnifeTimeHistory *KnifeTimeHistory::findKnifeTimeHistory(KnifeTimeHistory::KnifeHistoryType type, DBAccess *access)
{
	bool traceOn = Debug::isTracing("KnifeTimeHistory");
	HobString msgDebug;
	if ( traceOn )
		msgDebug = "findKnifeTimeHistory():  ";

	KnifeTimeHistory *readHistory = 0;
	DBKnifeTimeHistoryRM* knifeTimeHistoryRM = 0;

	if ( !access )
	{
		if ( traceOn )
			Debug::trace("KnifeTimeHistory", msgDebug + "Exiting!  dbAccess = 0.");
	}
	else
	{
		knifeTimeHistoryRM = access->getKnifeTimeHistoryRM();
		if ( !knifeTimeHistoryRM )
		{
			if ( traceOn )
				Debug::trace("KnifeTimeHistory", msgDebug + "Exiting!  knifeTimeHistoryRM = 0!");
		}
		else
		{
			try
			{
				DBKnifeTimeHistory *dbknifeTimeHistoryRec = knifeTimeHistoryRM->find(Integer::toString(type));
				if ( dbknifeTimeHistoryRec )
				{
					if ( traceOn )
						Debug::trace("KnifeTimeHistory", msgDebug + "Found record in the db.");

					readHistory = new KnifeTimeHistory(dbknifeTimeHistoryRec, access);
					if ( readHistory )
					{
						if ( readHistory->getErrorCode() == RECORD_CORRUPT )
						{
							if ( traceOn )
								Debug::trace("KnifeTimeHistory", msgDebug + "Record is corrupt!");
							if ( knifeTimeHistoryRM->deleteRecord(dbknifeTimeHistoryRec) )
							{
								delete readHistory;
								readHistory = 0;
							}
						}
						else
						{
							if ( traceOn )
								Debug::trace("KnifeTimeHistory", msgDebug + "Record is good.");
						}
					}
				}
			}
			catch ( DBException &dbe )
			{
				if ( Debug::isTracing("DBAccess.exception") )
				{
					Debug::trace("DBAccess.exception", "KnifeTimeHistory::findKnifeTimeHistory() ********** database exception **********");
					Debug::trace("DBAccess.exception", "     DBException: type:" +
							util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
				}
			}
		}
	}
	return readHistory;
}

const char *KnifeTimeHistory::serialize(int format) const
{
	HfcObject::serializedData = "";
	HobString result;
	switch (format)
	{
		case BrowserFormat:
		{
			Debug::trace("KnifeTimeHistory", "serialize(BrowserFormat)");

			HfcObject::serializedData.append(Integer::toString(getKnifeTime()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			if ( dbAccess )		//	This is a real KnifeTimeHistory record.
			{
				result = knifeDateTime_.formatDate(Configuration::getInstance()->getDateDisplayFormat());
				result.quoteForCSV();
			}
			HfcObject::serializedData.append(result);
			break;
		}
		default:
			// to do - throw exception("invalid format");
			Debug::trace("KnifeTimeHistory", "serialize(Invalid format)");
			break;
	}
	return 	HfcObject::serializedData.c_str();
}

void KnifeTimeHistory::saveRecord()
{
	bool traceOn = Debug::isTracing("KnifeTimeHistory");
	HobString msgDebug;
	if ( traceOn )
	{
		msgDebug = "saveRecord()";
		Debug::trace("KnifeTimeHistory", msgDebug);
		msgDebug.append(":  ");
	}

	if ( dbAccess )
	{
		edb::DBKnifeTimeHistoryRM* knifeTimeHistoryRM = dbAccess->getKnifeTimeHistoryRM();
		if ( knifeTimeHistoryRM )
		{
			DBKnifeTimeHistory *dbknifeTimeHistoryRec = knifeTimeHistoryRM->find(util_string::inttostring(getKnifeHistoryType()));
			if ( !dbknifeTimeHistoryRec )
			{
				//	This must be a new record for the Database
				if ( traceOn )
					Debug::trace("KnifeTimeHistory", msgDebug + "Don't have a db record, creating it...");

				dbknifeTimeHistoryRec = knifeTimeHistoryRM->createRecord();
			}
			
			if ( dbknifeTimeHistoryRec )
			{			
				dbknifeTimeHistoryRec->setType(util_string::inttostring(getKnifeHistoryType()));
				dbknifeTimeHistoryRec->setKnifeTime(getKnifeTime());
				dbknifeTimeHistoryRec->setTimeStampDateTime(getKnifeDateTime().convertToDBFormat());
			
				if ( traceOn )
					Debug::trace("KnifeTimeHistory", msgDebug + "Saving type: " + util_string::inttostring(getKnifeHistoryType()));

				try
				{
					knifeTimeHistoryRM->saveRecord(dbknifeTimeHistoryRec);
				}
				catch (DBException &dbe)
				{
					Debug::trace("DBAccess.exception", "KnifeTimeHistory::saveRecord() ********** database exception **********");
					Debug::trace("DBAccess.exception", "     DBException: type:" +
							util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
				}
			}
		}
		else
		{
			if ( traceOn )
				Debug::trace("KnifeTimeHistory", msgDebug + "knifeTimeHistoryRM is 0!");
		}
	}
	else
	{
		if ( traceOn )
			Debug::trace("KnifeTimeHistory", msgDebug + "dbAccess is 0!");
	}
}

void KnifeTimeHistory::show()
{
	HobString msgDebug;
	if ( Debug::isTracing("KnifeTimeHistory.show") )
	{
		switch ( knifeHistoryType_ )
		{
			case KnifeTimeNone:
				msgDebug = "KnifeTimeNone";
				break;
			case KnifeUpTimeInstall:
				msgDebug = "KnifeUpTimeInstall";
				break;
			case KnifeUpTimeMin:
				msgDebug = "KnifeUpTimeMin";
				break;
			case KnifeUpTimeMax:
				msgDebug = "KnifeUpTimeMax";
				break;
			case KnifeDownTimeInstall:
				msgDebug = "KnifeDownTimeInstall";
				break;
			case KnifeDownTimeMin:
				msgDebug = "KnifeDownTimeMin";
				break;
			case KnifeDownTimeMax:
				msgDebug = "KnifeDownTimeMax";
				break;
			default:
				msgDebug = "Unknown!";
				break;
		}
		Debug::trace("KnifeTimeHistory.show", "knifeHistoryType_ = " + msgDebug);
		Debug::trace("KnifeTimeHistory.show", "       knifeTime_ = " + uinttostring(knifeTime_) + "ms");
		Debug::trace("KnifeTimeHistory.show", "   knifeDateTime_ = " + knifeDateTime_.formatDate(Configuration::getInstance()->getDateDisplayFormat()));
	}
}
