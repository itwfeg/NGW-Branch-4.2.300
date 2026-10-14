/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllExternalPrinters.h"
#include "Device.h"
#include "Configuration.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBDevicesRM.h"
#include "DBDevice.h"

#include "Long.h"
#include "Integer.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllExternalPrinters::dataType = 0;
std::string AllExternalPrinters::dataTypeName("AllExternalPrinters"); 
#endif


AllExternalPrinters::AllExternalPrinters()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

AllExternalPrinters::~AllExternalPrinters()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


void AllExternalPrinters::findAll(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	DBDevicesRM *dbRM = dbAccess->getDBDevicesRM();
	if (!dbRM)
		return;

	bool traceOn = Debug::isTracing("AllExternalPrinters");
	try
	{
		DBQuery<DBDevice>* allQuery = dbRM->retrieveAll();
		if (!allQuery)
			return;

		DBDevice *dbRec = 0;
		DBQuery<DBDevice>::const_iterator iter = allQuery->begin();
		while (iter != allQuery->end())
		{
			if (traceOn)
				Debug::trace("AllExternalPrinters", string("findAll - type ") + iter->getType() + "  subtype " + iter->getSubtype() + "  kind " + inttostring(iter->getKind()));
			dbRec = new DBDevice(*iter);
			if (dbRec)
			{
				Device *device = new Device(dbRec, dbAccess);
				if (device)
				{
					EvolDeviceType type = (EvolDeviceType)Integer::valueOf(iter->getType());
					short subType = Integer::valueOf(iter->getSubtype());

					short kind = iter->getKind();
					if (kind == (int)EppPrinter || kind == (int)Epp2Printer)
						kind = (int)EppPrinter;

					if (type == Printer && (subType == (short)SecondPrinter || subType == (short)ThirdPrinter) && device->isARealPrinter()
						/* && (kind == (short)USBPrinter || kind == (short)NetworkPrinter || kind == (short)EppPrinter || kind == (short)Epp2Printer) */)
					{
						if (traceOn)
							Debug::trace("AllExternalPrinters", "findAll - printer being added to lists");
						extPrinterNumList.push_back(iter->getSubtype());
						extPrinterUserNameList.push_back(iter->getUserName());
						extPrinterTypeList.push_back(Integer::toString(kind));
						extPrinterSystemNameList.push_back((kind == (short)NetworkPrinter) ? iter->getSystemName() : "");
						extPrinterIpAddressList.push_back((kind == (short)NetworkPrinter) ? iter->getIPAddress() : "");
					}
					delete device;
				}
				// don't delete dbRec; it gets deleted when deleting device above
			}
			++iter;
		}
		delete allQuery;

		if (traceOn)
			Debug::trace("AllExternalPrinters", string("findAll - numList size ") + inttostring(extPrinterNumList.size()) + "  UserNameList size " + inttostring(extPrinterUserNameList.size()) + "  TypeList size " + inttostring(extPrinterTypeList.size())
			                                    + "  SystemNameList size " + inttostring(extPrinterSystemNameList.size()) + "  IpAddressList size " + inttostring(extPrinterIpAddressList.size()));
	}
	catch (DBException &dbe)
	{
		Debug::trace("DBAccess.exception", "AllExternalPrinters::findAll() ********** database exception **********");
		DBException::Type t = dbe.getType();
		Debug::trace("DBAccess.exception", string("     DBException: type:") + inttostring(t) + string("  info:") + dbe.info());
	}
}

const char *AllExternalPrinters::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
//			if (extPrinterNameList.size())
			{
				serializedData.append(extPrinterNumList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(extPrinterUserNameList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(extPrinterTypeList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(extPrinterSystemNameList.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(extPrinterIpAddressList.serialize(format));
			}
			break;

		default:
			break;
	}
	return serializedData.c_str();
}
