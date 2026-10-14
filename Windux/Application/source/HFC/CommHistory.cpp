/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CommHistory.h"
#include "Configuration.h"
#include "ProductDevEvent.h"
#include "DBAccess.h"

//#include "HobTokenizerCSV.h"
//#include "HobTokenizerUS.h"
//#include "HobTokenTagged.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CommHistory::dataType = 0;
std::string CommHistory::dataTypeName("CommHistory"); 
#endif


CommHistory::CommHistory()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}


CommHistory::~CommHistory()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("CommHistory.Delete", HobString("    CommHistory: ") + HobString(Hex::toString((long long) this)));
}


//	private
void CommHistory::init()
{
}


void CommHistory::recordComm(EvolDeviceType deviceType, UINT8 eventType)
{
	if (deviceType == ProductDev && eventType == ProductRestore)
	{
		// Record the fact that RT89 was received:  Configuration - communicationItemReceived
		if (!alreadyReceived(deviceType))
		{
			Debug::trace("CommHistory", "received ProductDev-ProductRestore message via comm");
			deviceEventList.push_back(deviceType);
		}
	}
}

bool CommHistory::alreadyReceived(EvolDeviceType deviceType) const
{
	if (deviceEventList.size() == 0)
		return false;

	std::vector<EvolDeviceType>::const_iterator deviceIterator = deviceEventList.begin();
	while (deviceIterator != deviceEventList.end())
	{
		if (*deviceIterator == deviceType)
			return true;
		deviceIterator++;
	}
	return false;
}

void CommHistory::saveCommHistory(DBAccess *dbAccess)
{
	HobString dateTime = commSessionStart.convertToDBFormat();
	if (alreadyReceived(ProductDev))
	{
		Debug::trace("CommHistory", HobString("saving date/time for CommunicationItemReceived into Config db: ") + dateTime);
		Configuration::getInstance()->setCommunicationItemReceived(dateTime);
	}

	Debug::trace("CommHistory", HobString("saving date/time for CommunicationHistory into Config db: ") + dateTime);
	Configuration::getInstance()->setCommunicationHistory(dateTime);
	Configuration::getInstance()->save(dbAccess);
}

const char *CommHistory::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

//	switch (format)
//	{
//		default:
//			serializedData = HobString("CommHistory::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
//			break;
//	}
	return serializedData.data();
}
