/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "DevicesList.h"
#include "HobString.h"
#include "CfgInternal.h"
#include "Configuration.h"
#include "Integer.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int DevicesList::dataType = 0;
std::string DevicesList::dataTypeName("DevicesList"); 
#endif

DevicesList::DevicesList()
	: HobList()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	findDeviceNames();
}

DevicesList::DevicesList(const char *data, int len, int formatType)
	: HobList(data, len, formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

DevicesList::~DevicesList()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


void DevicesList::findDeviceNames()
{
#if 0 //lcg rev 0.1!!!
	char device[6] = "/null";
	char *pTail;
	DEV_HDR * pHdr;
	Configuration* pCFG = Configuration::getInstance();

	int nfsDrvNum = nfsDrvNumGet();
	Debug::trace("DevicesList", HobString(" - nfsDrvNum:") + HobString(Integer::toString(nfsDrvNum)));

	extern int dosFsDrvNum;
	Debug::trace("DevicesList", HobString(" - dosfsDrvNum:") + HobString(Integer::toString(dosFsDrvNum)));

	pHdr = iosDevFind(device, &pTail);
	while (pHdr)
	{
		HobString devName(pHdr->name);
		Debug::trace("DevicesList", HobString(" - device -  name:") + devName +
											HobString("  drvnum:") + Integer::toString(pHdr->drvNum));

		if (!pCFG->getBulkDrivesOnlyMode())
		{
			if (pHdr->drvNum == dosFsDrvNum || pHdr->drvNum == nfsDrvNum || devName == "/tgtsvr")
			{
				if (devName.find("thumb") != string::npos)
					push_front(devName);
				else
					push_back(devName);
			}
		}
		else
		{
			if ((pHdr->drvNum == dosFsDrvNum) && (pCFG->getUSBBulkDriveName() == devName))
			{

				push_front(devName);

			}

		}

		pHdr = iosNextDevGet(pHdr);
	}
#endif //lcg rev 0.1
}
