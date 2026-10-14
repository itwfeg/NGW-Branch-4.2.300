//#include "vxWorks.h"
#include "WrapperSystemConfiguration.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperSystemConfiguration::dataType = 0;
std::string WrapperSystemConfiguration::dataTypeName("WrapperSystemConfiguration"); 
#endif

WrapperSystemConfiguration::WrapperSystemConfiguration(Hobart::CEvolMsgReceiver *pHobThread, 
													   SysConfigType sysConfigType,
													   UINT8 *deviceSignature,
													   DeviceAddress stationAddress,
													   DeviceAddress controllerAddress) : SystemConfiguration(WrapperSystem, 
													   														  pHobThread,
													   														  sysConfigType,
													   														  deviceSignature,
													   														  stationAddress,
													   														  controllerAddress)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperSystemConfiguration::WrapperSystemConfiguration(const char *buffer) : SystemConfiguration(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperSystemConfiguration::~WrapperSystemConfiguration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

