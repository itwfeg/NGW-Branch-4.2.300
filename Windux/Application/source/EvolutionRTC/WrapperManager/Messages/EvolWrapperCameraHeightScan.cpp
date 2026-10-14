#include "EvolWrapperCameraHeightScan.h"

#include "HobString.h"
#include "Debug.h"
#include "Long.h"
#include "Integer.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperCameraHeightScan::_dataType_ = 0;
std::string EvolWrapperCameraHeightScan::_dataTypeName_("EvolWrapperCameraHeightScan"); 
#endif

EvolWrapperCameraHeightScan::EvolWrapperCameraHeightScan(Hobart::CEvolMsgReceiver *pHobThread)
	: WrapperGeneric(WrCameraHeightScan, pHobThread), scan(0)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

EvolWrapperCameraHeightScan::EvolWrapperCameraHeightScan(const char *buffer)
	: WrapperGeneric(buffer), scan(0)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &scan;
	for (unsigned int i = 0; i < sizeof(scan); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperCameraHeightScan::~EvolWrapperCameraHeightScan()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *EvolWrapperCameraHeightScan::serialize()const
{
	WrapperGeneric::serialize();

	Debug::trace("EvolWrapperCameraHeightScan", "serialize");

	char *pChar = (char *) &scan;
	for (unsigned int i = 0; i < sizeof(scan); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void EvolWrapperCameraHeightScan::show()
{
	if (Debug::isTracing("EvolWrapperCameraHeightScan"))
	{
		HobString headers("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");

		Debug::trace("EvolWrapperCameraHeightScan", headers + "  START");

		HobString msgDebug("0.50\" = ");
		msgDebug.append((scan & Bit_050_Mask) ? "1" : "0");
		Debug::trace("EvolWrapperCameraHeightScan", msgDebug);

		msgDebug = "0.75\" = ";
		msgDebug.append((scan & Bit_075_Mask) ? "1" : "0");
		Debug::trace("EvolWrapperCameraHeightScan", msgDebug);

		msgDebug = "1.00\" = ";
		msgDebug.append((scan & Bit_100_Mask) ? "1" : "0");
		Debug::trace("EvolWrapperCameraHeightScan", msgDebug);

		msgDebug = "1.25\" = ";
		msgDebug.append((scan & Bit_125_Mask) ? "1" : "0");
		Debug::trace("EvolWrapperCameraHeightScan", msgDebug);

		msgDebug = "1.50\" = ";
		msgDebug.append((scan & Bit_150_Mask) ? "1" : "0");
		Debug::trace("EvolWrapperCameraHeightScan", msgDebug);

		msgDebug = "1.75\" = ";
		msgDebug.append((scan & Bit_175_Mask) ? "1" : "0");
		Debug::trace("EvolWrapperCameraHeightScan", msgDebug);

		msgDebug = "2.00\" = ";
		msgDebug.append((scan & Bit_200_Mask) ? "1" : "0");
		Debug::trace("EvolWrapperCameraHeightScan", msgDebug);

		msgDebug = "2.25\" = ";
		msgDebug.append((scan & Bit_225_Mask) ? "1" : "0");
		Debug::trace("EvolWrapperCameraHeightScan", msgDebug);

		msgDebug = "2.50\" = ";
		msgDebug.append((scan & Bit_250_Mask) ? "1" : "0");
		Debug::trace("EvolWrapperCameraHeightScan", msgDebug);

		msgDebug = "3.00\" = ";
		msgDebug.append((scan & Bit_300_Mask) ? "1" : "0");
		Debug::trace("EvolWrapperCameraHeightScan", msgDebug);

		msgDebug = "3.50\" = ";
		msgDebug.append((scan & Bit_350_Mask) ? "1" : "0");
		Debug::trace("EvolWrapperCameraHeightScan", msgDebug);

		msgDebug = "4.00\" = ";
		msgDebug.append((scan & Bit_400_Mask) ? "1" : "0");
		Debug::trace("EvolWrapperCameraHeightScan", msgDebug);

		msgDebug = "4.50\" = ";
		msgDebug.append((scan & Bit_450_Mask) ? "1" : "0");
		Debug::trace("EvolWrapperCameraHeightScan", msgDebug);

		msgDebug = "5.00\" = ";
		msgDebug.append((scan & Bit_500_Mask) ? "1" : "0");
		Debug::trace("EvolWrapperCameraHeightScan", msgDebug);

		msgDebug = "6.00\" = ";
		msgDebug.append((scan & Bit_600_Mask) ? "1" : "0");
		Debug::trace("EvolWrapperCameraHeightScan", msgDebug);

		Debug::trace("EvolWrapperCameraHeightScan", headers + "  END");

	}
}

UINT16 EvolWrapperCameraHeightScan::getScan() const
{
	return scan;
}

void EvolWrapperCameraHeightScan::setScan(UINT16 value)
{
	scan = value;
}

bool EvolWrapperCameraHeightScan::equals(EvolWrapperCameraHeightScan &wrScan)
{
	bool rv = true;

	if	(scan != wrScan.getScan())
		rv = false;

	return rv;
}
