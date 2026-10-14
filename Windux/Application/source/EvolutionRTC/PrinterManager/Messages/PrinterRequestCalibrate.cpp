//#include "vxWorks.h"

#include "PrinterRequestCalibrate.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterRequestCalibrate::dataType = 0;
std::string PrinterRequestCalibrate::dataTypeName("PrinterRequestCalibrate"); 
#endif

PrinterRequestCalibrate::PrinterRequestCalibrate(Hobart::CEvolMsgReceiver *pHobThread,
								   PrinterType printerType,
								   PrinterCalibrateType printerCalibrationType) : PrinterGeneric(PrReqCalibrate,
								   																 pHobThread,
								   																 printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->printerCalibrationType = printerCalibrationType;
}

PrinterRequestCalibrate::PrinterRequestCalibrate(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &printerCalibrationType;
	for (unsigned int i = 0; i < sizeof(printerCalibrationType); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterRequestCalibrate::~PrinterRequestCalibrate()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterRequestCalibrate::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &printerCalibrationType;
	for (unsigned int i = 0; i < sizeof(printerCalibrationType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterRequestCalibrate::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("PrinterRequestCalibrate") );
#else
	Debug::addTraceCategory("PrinterRequestCalibrate");
#endif
	Debug::trace("PrinterRequestCalibrate", "PrinterRequestCalibrate: ");

	PrinterGeneric::show();

	if (printerCalibrationType == PrCalibratePrinthead)
	{
		Debug::trace("PrinterRequestCalibrate", HobString("printerCalibrationType: PrCalibratePrinthead"));
	}
	else if (printerCalibrationType == PrCalibrateMediaSensor)
	{
		Debug::trace("PrinterRequestCalibrate", HobString("printerCalibrationType: PrCalibrateMediaSensor"));
	}
	else if (printerCalibrationType == PrCalibrateLabelTaken)
	{
		Debug::trace("PrinterRequestCalibrate", HobString("printerCalibrationType: PrCalibrateLabelTaken"));
	}
	else
	{
		Debug::trace("PrinterRequestCalibrate", HobString("printerCalibrationType: Unknown ") +
											 HobString(Integer::toString((UINT16) printerCalibrationType)));
	}

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("PrinterRequestCalibrate") );
#else
	Debug::removeTraceCategory("PrinterRequestCalibrate");
#endif
}

PrinterCalibrateType PrinterRequestCalibrate::getPrinterCalibrateType()
{
	return printerCalibrationType;
}

void PrinterRequestCalibrate::setPrinterCalibrateType(PrinterCalibrateType pct)
{
	printerCalibrationType = pct;
}

void PrinterRequestCalibrate::setCalibrationTypeMediaSensor()
{
	printerCalibrationType = PrCalibrateMediaSensor;
}

void PrinterRequestCalibrate::setCalibrationTypePrinthead()
{
	printerCalibrationType = PrCalibratePrinthead;
}

void PrinterRequestCalibrate::setCalibrationTypeLabelTaken()
{
	printerCalibrationType = PrCalibrateLabelTaken;
}
