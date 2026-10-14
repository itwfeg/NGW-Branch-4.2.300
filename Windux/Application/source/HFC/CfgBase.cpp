/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgBase.h"
#include "DBAccess.h"
#include "DBException.h"
#include "DBConfigurationsRM.h"
#include "DBConfiguration.h"

#include "Debug.h"

#include "Integer.h"
#include "Bool.h"


const std::string CfgBase::yesValue("Yes");
const std::string CfgBase::noValue("No");

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgBase::dataType = 0;
std::string CfgBase::dataTypeName("CfgBase"); 
#endif

CfgBase::CfgBase()
	: receivedData_(""), receivedFormat_(UnknownFormat)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	saveNeeded = false;
}

CfgBase::~CfgBase()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


void CfgBase::load(DBAccess * /* dbAccess */)
{
}


// protected
// This method returns true if the value was loaded; false if any errors encountered.
bool CfgBase::loadField(DBAccess *dbAccess, const std::string &fieldName, std::string &value)
{
	bool ret = false;
	value = "";

	if (!dbAccess)
		return ret;

	bool isTracing = Debug::isTracing("Configuration.load");

	Util::CLockCriticalSection lock( cfgBaseCS );

	DBConfigurationsRM *dbConfigurationsRM = dbAccess->getDBConfigurationsRM();
	if (!dbConfigurationsRM)
	{
		if (isTracing)
			Debug::trace("Configuration.load", HobString("loadField() - ") + fieldName + HobString("  could not get DBConfigurationsRM"));
		return ret;
	}	

	try
	{		
		DBConfiguration *cfgRec = dbConfigurationsRM->findName(fieldName);
		if (cfgRec)
		{
			if (cfgRec->isRecordValid())
			{
				if (cfgRec->isValueNull())
				{
					if (isTracing)
						Debug::trace("Configuration.load", HobString("loadField() - ") + fieldName + HobString(" not set because both value fields are null"));
				}
				else
				{
					value = cfgRec->getValue();
					if (isTracing)
						Debug::trace("Configuration.load", HobString("loadField() - ") + fieldName + HobString(" set from value: ") + value);
				}

				ret = true;
			}
			else if (isTracing)
				Debug::trace("Configuration.load", HobString("loadField() - ") + fieldName + HobString(" not set due to db record not valid!!!"));

			delete cfgRec;
			cfgRec = (DBConfiguration *)NULL;
			return ret;
		}
		else if (isTracing)
			Debug::trace("Configuration.load", HobString("loadField() - ") + fieldName + HobString(" not set due to db record not found!!!"));
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("CfgBase::loadField() - ") + fieldName + HobString(" - ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return ret;
}


// protected
void CfgBase::saveField(DBAccess *dbAccess, const std::string &fieldName, const std::string &value, bool /* batchIt */)
{
	if (!dbAccess)
		return;

	bool isTracing = Debug::isTracing("Configuration.save");

	Util::CLockCriticalSection lock( cfgBaseCS );

	DBConfigurationsRM *dbConfigurationsRM = dbAccess->getDBConfigurationsRM();
	if (!dbConfigurationsRM)
	{
		if (isTracing)
			Debug::trace("Configuration.save", HobString("saveField() - ") + fieldName + HobString("  could not get DBConfigurationsRM"));
		return;
	}	

	try
	{		
		DBConfiguration *cfgRec = dbConfigurationsRM->findName(fieldName);
		if (!cfgRec)
		{
			if (isTracing)
				Debug::trace("Configuration.save", HobString("saveField() creating record - ") + fieldName);
			cfgRec = dbConfigurationsRM->createRecord();
			if (cfgRec)
				cfgRec->setName(fieldName);
		}			
		if (cfgRec)
		{
			if (isTracing)
				Debug::trace("Configuration.save", HobString("saveField() - ") + fieldName + HobString(" set to value: ") + value);
			cfgRec->setValue(value);

			dbConfigurationsRM->saveRecord(cfgRec);
			delete cfgRec;
			cfgRec = (DBConfiguration *)NULL;
		}
		else if (isTracing)
			Debug::trace("Configuration.save", HobString("saveField() - ") + fieldName + HobString(" not saved due to db record not found and not created!!!"));
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("CfgBase::saveField() - ") + fieldName + HobString(" - ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}
}

void CfgBase::deleteField(DBAccess *dbAccess, const std::string &fieldName)
{
	if (!dbAccess)
		return;

	bool isTracing = Debug::isTracing("CfgScale");

	Util::CLockCriticalSection lock( cfgBaseCS );

	DBConfigurationsRM *dbConfigurationsRM = dbAccess->getDBConfigurationsRM();
	if (!dbConfigurationsRM)
	{
		if (isTracing)
			Debug::trace("CfgScale", HobString("deleteField() - ") + fieldName + HobString("  could not get DBConfigurationsRM"));
		return;
	}	

	try
	{		
		DBConfiguration *cfgRec = dbConfigurationsRM->findName(fieldName);
		if (cfgRec)
		{
			dbConfigurationsRM->deleteRecord(cfgRec);
			delete cfgRec;
			cfgRec = (DBConfiguration *)NULL;
		}
		else if (isTracing)
			Debug::trace("CfgScale", HobString("deleteField() record/field not found in database - ") + fieldName);
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("CfgScale::deleteField() - ") + fieldName + HobString(" - ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}
}
