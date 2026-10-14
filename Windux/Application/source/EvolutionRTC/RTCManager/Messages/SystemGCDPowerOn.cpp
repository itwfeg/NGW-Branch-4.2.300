#include "SystemGCDPowerOn.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SystemGCDPowerOn::dataType = 0;
std::string SystemGCDPowerOn::dataTypeName("SystemGCDPowerOn"); 
#endif

SystemGCDPowerOn::SystemGCDPowerOn()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

SystemGCDPowerOn::~SystemGCDPowerOn()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *SystemGCDPowerOn::serialize() const
{
	static char buffer[12];

	buffer[0]  = 0x00;
	buffer[1]  = 0x00;
	buffer[2]  = 0x00;
	buffer[3]  = 0x08;
	buffer[4]  = 0x46;
	buffer[5]  = 0x46;
	buffer[6]  = 0x00;
	buffer[7]  = 0x08;
	buffer[8]  = 0x00;
	buffer[9]  = 0x02;
	buffer[10] = 0x15;
	buffer[11] = 0x00;
	return (const char*)buffer;
}

int SystemGCDPowerOn::getMessageLength()
{
	return 12;
}
