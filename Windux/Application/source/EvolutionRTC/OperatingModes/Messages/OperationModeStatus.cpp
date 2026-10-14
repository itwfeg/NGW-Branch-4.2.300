//#include "vxWorks.h"
#include "OperationModeStatus.h"

#include "Debug.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModeStatus::dataType = 0;
std::string OperationModeStatus::dataTypeName("OperationModeStatus"); 
#endif

OperationModeStatus::OperationModeStatus(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModeStatus, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	errorSource = NoErrorSource;

	wrapperPressStart = false;
	wrapperIdleError = false;
	wrapperGhostPackage = false;
	wrapperOverWeight = false;
	printerOverflowFirstPrinter = false;
	printerOverflowSecondPrinter = false;
	printerOverflowThirdPrinter = false;
	labelApplierPackageOverrun = false;
	encoderWindow_1 = false;
	encoderWindow_2 = false;
	encoderWindow_3 = false;
	evolWrapperHoming = false;
	evolWrapperCalibratePlacePkg = false;
	evolWrapperBuildingAirPressure = false;

	newWrapperError = false;
	wrapperError = WrapperErrorInfo::NoErrorSource;
	prevWrapperError = WrapperErrorInfo::NoErrorSource;

	newEvolWrapperError = false;
	evolWrapperError = EvolWrapperErrorInfo::NoWrapperError;
	evolWrapperErrorSource = EvolWrapperErrorInfo::NoWrapperErrorSource;
	prevEvolWrapperError = EvolWrapperErrorInfo::NoWrapperError;
	prevEvolWrapperErrorSource = EvolWrapperErrorInfo::NoWrapperErrorSource;

	bakeryCLAPackageMissing = false;
	bakeryCLALabelMissing = false;

	pkgDischargeErrorFirstPrinter = false;
	pkgDischargeErrorSecondPrinter = false;
	pkgDischargeErrorThirdPrinter = false;

	productRequiredToWrap = false;
}

OperationModeStatus::OperationModeStatus(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	OperationModeGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &errorSource;
	for (unsigned int i = 0; i < sizeof(errorSource); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &wrapperPressStart;
	for (unsigned int i = 0; i < sizeof(wrapperPressStart); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &wrapperIdleError;
	for (unsigned int i = 0; i < sizeof(wrapperIdleError); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &wrapperGhostPackage;
	for (unsigned int i = 0; i < sizeof(wrapperGhostPackage); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &wrapperOverWeight;
	for (unsigned int i = 0; i < sizeof(wrapperOverWeight); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &printerOverflowFirstPrinter;
	for (unsigned int i = 0; i < sizeof(printerOverflowFirstPrinter); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &printerOverflowSecondPrinter;
	for (unsigned int i = 0; i < sizeof(printerOverflowSecondPrinter); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &printerOverflowThirdPrinter;
	for (unsigned int i = 0; i < sizeof(printerOverflowThirdPrinter); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &labelApplierPackageOverrun;
	for (unsigned int i = 0; i < sizeof(labelApplierPackageOverrun); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &encoderWindow_1;
	for (unsigned int i = 0; i < sizeof(encoderWindow_1); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &encoderWindow_2;
	for (unsigned int i = 0; i < sizeof(encoderWindow_2); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &encoderWindow_3;
	for (unsigned int i = 0; i < sizeof(encoderWindow_3); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &evolWrapperHoming;
	for (unsigned int i = 0; i < sizeof(evolWrapperHoming); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &evolWrapperCalibratePlacePkg;
	for (unsigned int i = 0; i < sizeof(evolWrapperCalibratePlacePkg); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &evolWrapperBuildingAirPressure;
	for (unsigned int i = 0; i < sizeof(evolWrapperBuildingAirPressure); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &newWrapperError;
	for (unsigned int i = 0; i < sizeof(newWrapperError); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &wrapperError;
	for (unsigned int i = 0; i < sizeof(wrapperError); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &prevWrapperError;
	for (unsigned int i = 0; i < sizeof(prevWrapperError); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &newEvolWrapperError;
	for (unsigned int i = 0; i < sizeof(newEvolWrapperError); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &evolWrapperError;
	for (unsigned int i = 0; i < sizeof(evolWrapperError); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &evolWrapperErrorSource;
	for (unsigned int i = 0; i < sizeof(evolWrapperErrorSource); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &prevEvolWrapperError;
	for (unsigned int i = 0; i < sizeof(prevEvolWrapperError); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &prevEvolWrapperErrorSource;
	for (unsigned int i = 0; i < sizeof(prevEvolWrapperErrorSource); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &bakeryCLAPackageMissing;
	for (unsigned int i = 0; i < sizeof(bakeryCLAPackageMissing); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &bakeryCLALabelMissing;
	for (unsigned int i = 0; i < sizeof(bakeryCLALabelMissing); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pkgDischargeErrorFirstPrinter;
	for (unsigned int i = 0; i < sizeof(pkgDischargeErrorFirstPrinter); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pkgDischargeErrorSecondPrinter;
	for (unsigned int i = 0; i < sizeof(pkgDischargeErrorSecondPrinter); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pkgDischargeErrorThirdPrinter;
	for (unsigned int i = 0; i < sizeof(pkgDischargeErrorThirdPrinter); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &productRequiredToWrap;
	for (unsigned int i = 0; i < sizeof(productRequiredToWrap); i++)
	{
		*pChar++ = buffer[j++];
	}
}

OperationModeStatus::~OperationModeStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *OperationModeStatus::serialize() const
{
	OperationModeGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &errorSource;
	for (unsigned int i = 0; i < sizeof(errorSource); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &wrapperPressStart;
	for (unsigned int i = 0; i < sizeof(wrapperPressStart); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &wrapperIdleError;
	for (unsigned int i = 0; i < sizeof(wrapperIdleError); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &wrapperGhostPackage;
	for (unsigned int i = 0; i < sizeof(wrapperGhostPackage); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &wrapperOverWeight;
	for (unsigned int i = 0; i < sizeof(wrapperOverWeight); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &printerOverflowFirstPrinter;
	for (unsigned int i = 0; i < sizeof(printerOverflowFirstPrinter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &printerOverflowSecondPrinter;
	for (unsigned int i = 0; i < sizeof(printerOverflowSecondPrinter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &printerOverflowThirdPrinter;
	for (unsigned int i = 0; i < sizeof(printerOverflowThirdPrinter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &labelApplierPackageOverrun;
	for (unsigned int i = 0; i < sizeof(labelApplierPackageOverrun); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &encoderWindow_1;
	for (unsigned int i = 0; i < sizeof(encoderWindow_1); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &encoderWindow_2;
	for (unsigned int i = 0; i < sizeof(encoderWindow_2); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &encoderWindow_3;
	for (unsigned int i = 0; i < sizeof(encoderWindow_3); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &evolWrapperHoming;
	for (unsigned int i = 0; i < sizeof(evolWrapperHoming); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &evolWrapperCalibratePlacePkg;
	for (unsigned int i = 0; i < sizeof(evolWrapperCalibratePlacePkg); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &evolWrapperBuildingAirPressure;
	for (unsigned int i = 0; i < sizeof(evolWrapperBuildingAirPressure); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &newWrapperError;
	for (unsigned int i = 0; i < sizeof(newWrapperError); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &wrapperError;
	for (unsigned int i = 0; i < sizeof(wrapperError); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &prevWrapperError;
	for (unsigned int i = 0; i < sizeof(prevWrapperError); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &newEvolWrapperError;
	for (unsigned int i = 0; i < sizeof(newEvolWrapperError); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &evolWrapperError;
	for (unsigned int i = 0; i < sizeof(evolWrapperError); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &evolWrapperErrorSource;
	for (unsigned int i = 0; i < sizeof(evolWrapperErrorSource); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &prevEvolWrapperError;
	for (unsigned int i = 0; i < sizeof(prevEvolWrapperError); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &prevEvolWrapperErrorSource;
	for (unsigned int i = 0; i < sizeof(prevEvolWrapperErrorSource); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &bakeryCLAPackageMissing;
	for (unsigned int i = 0; i < sizeof(bakeryCLAPackageMissing); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &bakeryCLALabelMissing;
	for (unsigned int i = 0; i < sizeof(bakeryCLALabelMissing); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pkgDischargeErrorFirstPrinter;
	for (unsigned int i = 0; i < sizeof(pkgDischargeErrorFirstPrinter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pkgDischargeErrorSecondPrinter;
	for (unsigned int i = 0; i < sizeof(pkgDischargeErrorSecondPrinter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pkgDischargeErrorThirdPrinter;
	for (unsigned int i = 0; i < sizeof(pkgDischargeErrorThirdPrinter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &productRequiredToWrap;
	for (unsigned int i = 0; i < sizeof(productRequiredToWrap); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void OperationModeStatus::show()
{
	bool isTracing = Debug::isTracing("OperationModeStatus");

	if (isTracing == false)
		Debug::addTraceCategory( std::string("OperationModeStatus") );

	Debug::trace("OperationModeStatus", "OperationModeStatus: ");

	OperationModeGeneric::show();

	if (errorSource == NoErrorSource)
	{
		Debug::trace("OperationModeStatus", "errorSource: NoErrorSource");
	}

	Debug::trace("OperationModeStatus", "             wrapperPressStart: " + HobString(Bool::toString(wrapperPressStart)));
	Debug::trace("OperationModeStatus", "              wrapperIdleError: " + HobString(Bool::toString(wrapperIdleError)));
	Debug::trace("OperationModeStatus", "           wrapperGhostPackage: " + HobString(Bool::toString(wrapperGhostPackage)));
	Debug::trace("OperationModeStatus", "             wrapperOverWeight: " + HobString(Bool::toString(wrapperOverWeight)));
	Debug::trace("OperationModeStatus", "   printerOverflowFirstPrinter: " + HobString(Bool::toString(printerOverflowFirstPrinter)));
	Debug::trace("OperationModeStatus", "  printerOverflowSecondPrinter: " + HobString(Bool::toString(printerOverflowSecondPrinter)));
	Debug::trace("OperationModeStatus", "   printerOverflowThirdPrinter: " + HobString(Bool::toString(printerOverflowThirdPrinter)));
	Debug::trace("OperationModeStatus", "    labelApplierPackageOverrun: " + HobString(Bool::toString(labelApplierPackageOverrun)));
	Debug::trace("OperationModeStatus", "               encoderWindow_1: " + HobString(Bool::toString(encoderWindow_1)));
	Debug::trace("OperationModeStatus", "               encoderWindow_2: " + HobString(Bool::toString(encoderWindow_2)));
	Debug::trace("OperationModeStatus", "               encoderWindow_3: " + HobString(Bool::toString(encoderWindow_3)));
	Debug::trace("OperationModeStatus", "             evolWrapperHoming: " + HobString(Bool::toString(evolWrapperHoming)));
	Debug::trace("OperationModeStatus", "  evolWrapperCalibratePlacePkg: " + HobString(Bool::toString(evolWrapperCalibratePlacePkg)));
	Debug::trace("OperationModeStatus", "evolWrapperBuildingAirPressure: " + HobString(Bool::toString(evolWrapperBuildingAirPressure)));
	Debug::trace("OperationModeStatus", "       bakeryCLAPackageMissing: " + HobString(Bool::toString(bakeryCLAPackageMissing)));
	Debug::trace("OperationModeStatus", "         bakeryCLALabelMissing: " + HobString(Bool::toString(bakeryCLALabelMissing)));
	Debug::trace("OperationModeStatus", "               newWrapperError: " + HobString(Bool::toString(newWrapperError)));
	Debug::trace("OperationModeStatus", " pkgDischargeErrorFirstPrinter: " + HobString(Bool::toString(pkgDischargeErrorFirstPrinter)));
	Debug::trace("OperationModeStatus", "pkgDischargeErrorSecondPrinter: " + HobString(Bool::toString(pkgDischargeErrorSecondPrinter)));
	Debug::trace("OperationModeStatus", " pkgDischargeErrorThirdPrinter: " + HobString(Bool::toString(pkgDischargeErrorThirdPrinter)));
	Debug::trace("OperationModeStatus", "         productRequiredToWrap: " + HobString(Bool::toString(productRequiredToWrap)));

	Debug::trace("OperationModeStatus", "****** Access wrapper error *****");

	Debug::trace("OperationModeStatus", "       newEvolWrapperError: " + HobString(Bool::toString(newEvolWrapperError)));

	Debug::trace("OperationModeStatus", "    evolWrapperErrorSource: " + EvolWrapperErrorInfo::getEvolWrapperErrorSourceText(evolWrapperErrorSource));
	Debug::trace("OperationModeStatus", "          evolWrapperError: " + EvolWrapperErrorInfo::getEvolWrapperErrorText(evolWrapperError));

	Debug::trace("OperationModeStatus", "prevEvolWrapperErrorSource: " + EvolWrapperErrorInfo::getEvolWrapperErrorSourceText(prevEvolWrapperErrorSource));
	Debug::trace("OperationModeStatus", "      prevEvolWrapperError: " + EvolWrapperErrorInfo::getEvolWrapperErrorText(prevEvolWrapperError));

	Debug::trace("OperationModeStatus", "****** UWS error *****");

	Debug::trace("OperationModeStatus", "    wrapperErrorSource: " + getWrapperErrorText(wrapperError));
	Debug::trace("OperationModeStatus", "prevWrapperErrorSource: " + getWrapperErrorText(prevWrapperError));

	if (isTracing == false)
		Debug::removeTraceCategory( std::string("OperationModeStatus") );
}

bool OperationModeStatus::isWrapperPressStart()
{
	return wrapperPressStart;
}

void OperationModeStatus::setWrapperPressStart(bool value)
{
	wrapperPressStart = value;
	setError();
}

void OperationModeStatus::clearWrapperPressStart()
{
	wrapperPressStart = false;
	setError();
}

bool OperationModeStatus::isWrapperIdleError()
{
	return wrapperIdleError;
}

void OperationModeStatus::setWrapperIdleError(bool value)
{
	wrapperIdleError = value;
	setError();
}

void OperationModeStatus::clearWrapperIdleError()
{
	wrapperIdleError = false;
	setError();
}

bool OperationModeStatus::isWrapperGhostPackage()
{
	return wrapperGhostPackage;
}

void OperationModeStatus::setWrapperGhostPackage(bool value)
{
	wrapperGhostPackage = value;
	setError();
}

void OperationModeStatus::clearWrapperGhostPackage()
{
	wrapperGhostPackage = false;
	setError();
}

bool OperationModeStatus::isWrapperOverWeight()
{
	return wrapperOverWeight;
}

void OperationModeStatus::setWrapperOverWeight(bool value)
{
	wrapperOverWeight = value;
	setError();
}

void OperationModeStatus::clearWrapperOverWeight()
{
	wrapperOverWeight = false;
	setError();
}

bool OperationModeStatus::isPrinterOverflow(PrinterType printerType)
{
	bool rv = false;

	if (printerType == FirstPrinter)
	{
		rv = printerOverflowFirstPrinter;
	}
	else if (printerType == SecondPrinter)
	{
		rv = printerOverflowSecondPrinter;
	}
	else if (printerType == ThirdPrinter)
	{
		rv = printerOverflowThirdPrinter;
	}

	return rv;
}

void OperationModeStatus::setPrinterOverflow(PrinterType printerType, bool value)
{
	if (printerType == FirstPrinter)
	{
		printerOverflowFirstPrinter = value;
	}
	else if (printerType == SecondPrinter)
	{
		printerOverflowSecondPrinter = value;
	}
	else if (printerType == ThirdPrinter)
	{
		printerOverflowThirdPrinter = value;
	}

	setError();
}

void OperationModeStatus::clearPrinterOverflow(PrinterType printerType)
{
	if (printerType == FirstPrinter)
	{
		printerOverflowFirstPrinter = false;
	}
	else if (printerType == SecondPrinter)
	{
		printerOverflowSecondPrinter = false;
	}
	else if (printerType == ThirdPrinter)
	{
		printerOverflowThirdPrinter = false;
	}

	setError();
}

bool OperationModeStatus::isLabelApplierPackageOverrun()
{
	return labelApplierPackageOverrun;
}

void OperationModeStatus::setLabelApplierPackageOverrun(bool value)
{
	labelApplierPackageOverrun = value;
	setError();
}

void OperationModeStatus::clearLabelApplierPackageOverrun()
{
	labelApplierPackageOverrun = false;
	setError();
}

bool OperationModeStatus::isEncoderWindow_1()
{
	return encoderWindow_1;
}

void OperationModeStatus::setEncoderWindow_1(bool value)
{
	encoderWindow_1 = value;
	setError();
}

void OperationModeStatus::clearEncoderWindow_1()
{
	encoderWindow_1 = false;
	setError();
}

bool OperationModeStatus::isEncoderWindow_2()
{
	return encoderWindow_2;
}

void OperationModeStatus::setEncoderWindow_2(bool value)
{
	encoderWindow_2 = value;
	setError();
}

void OperationModeStatus::clearEncoderWindow_2()
{
	encoderWindow_2 = false;
	setError();
}

bool OperationModeStatus::isEncoderWindow_3()
{
	return encoderWindow_3;
}

void OperationModeStatus::setEncoderWindow_3(bool value)
{
	encoderWindow_3 = value;
	setError();
}

void OperationModeStatus::clearEncoderWindow_3()
{
	encoderWindow_3 = false;
	setError();
}


bool OperationModeStatus::isEvolWrapperHoming()
{
	return evolWrapperHoming;
}

void OperationModeStatus::setEvolWrapperHoming(bool value)
{
	evolWrapperHoming = value;
	setError();
}

void OperationModeStatus::clearEvolWrapperHoming()
{
	evolWrapperHoming = false;
	setError();
}

bool OperationModeStatus::isEvolWrapperCalibratePlacePkg()
{
	Debug::trace("OperationModeStatus", "isEvolWrapperCalibratePlacePkg() = " + HobString(Bool::toString(evolWrapperCalibratePlacePkg)));
	return evolWrapperCalibratePlacePkg;
}
void OperationModeStatus::setEvolWrapperCalibratePlacePkg(bool value)
{
	evolWrapperCalibratePlacePkg = value;
	setError();
	Debug::trace("OperationModeStatus", "setEvolWrapperCalibratePlacePkg() = " + HobString(Bool::toString(evolWrapperCalibratePlacePkg)));
}
void OperationModeStatus::clearEvolWrapperCalibratePlacePkg()
{
	evolWrapperCalibratePlacePkg = false;
	setError();
	Debug::trace("OperationModeStatus", "clearEvolWrapperCalibratePlacePkg() = " + HobString(Bool::toString(evolWrapperCalibratePlacePkg)));
}

bool OperationModeStatus::isEvolWrapperBuildingAirPressure()
{
	Debug::trace("OperationModeStatus", "isEvolWrapperBuildingAirPressure() = " + HobString(Bool::toString(evolWrapperBuildingAirPressure)));
	return evolWrapperBuildingAirPressure;
}
void OperationModeStatus::setEvolWrapperBuildingAirPressure(bool value)
{
	evolWrapperBuildingAirPressure = value;
	setError();
	Debug::trace("OperationModeStatus", "setEvolWrapperBuildingAirPressure() = " + HobString(Bool::toString(evolWrapperBuildingAirPressure)));
}
void OperationModeStatus::clearEvolWrapperBuildingAirPressure()
{
	evolWrapperBuildingAirPressure = false;
	setError();
	Debug::trace("OperationModeStatus", "clearEvolWrapperBuildingAirPressure() = " + HobString(Bool::toString(evolWrapperBuildingAirPressure)));
}

bool OperationModeStatus::isOpModeError()
{
	return (errorSource != NoErrorSource);
}

const OperationModeStatus::OpModeErrorSource &OperationModeStatus::getErrorSource()
{
	return (const OperationModeStatus::OpModeErrorSource &) errorSource;
}

void OperationModeStatus::setError(OpModeErrorSource errorSource)
{
	if (this->errorSource < errorSource)
	{
		this->errorSource = errorSource;
	}
}

void OperationModeStatus::setError()
{
	if (wrapperPressStart == true)
	{
		setError(WrapperPressStart);
	}

	if (wrapperIdleError == true)
	{
		setError(WrapperIdleError);
	}

	if (wrapperGhostPackage == true)
	{
		setError(WrapperGhostPackage);
		Debug::trace("OperationModeStatus", "setError(WrapperGhostPackage)");
	}

	if (wrapperOverWeight == true)
	{
		setError(WrapperOverWeight);
	}

	if (printerOverflowFirstPrinter == true)
	{
		setError(PrinterOverflowFirstPrinter);
	}

	if (printerOverflowSecondPrinter == true)
	{
		setError(PrinterOverflowSecondPrinter);
	}

	if (printerOverflowThirdPrinter == true)
	{
		setError(PrinterOverflowThirdPrinter);
	}

	if (labelApplierPackageOverrun == true)
	{
		setError(LaPackageOverrun);
	}

	if (encoderWindow_1 == true)
	{
		setError(WrapperTestEncoderWindow_1_Failed);
	}

	if (encoderWindow_2 == true)
	{
		setError(WrapperTestEncoderWindow_2_Failed);
	}

	if (encoderWindow_3 == true)
	{
		setError(WrapperTestEncoderWindow_3_Failed);
	}

	if (evolWrapperHoming == true)
	{
		setError(EvolWrapperHoming);
		Debug::trace("OperationModeStatus", "setError(EvolWrapperHoming)");
	}

	if (evolWrapperCalibratePlacePkg == true)
	{
		setError(EvolWrapperCalPlacePackage);
		Debug::trace("OperationModeStatus", "setError(EvolWrapperCalPlacePackage)");
	}

	if (evolWrapperBuildingAirPressure == true)
	{
		setError(BuildingAirPressure);
		Debug::trace("OperationModeStatus", "setError(evolWrapperBuildingAirPressure)");
	}

	if (pkgDischargeErrorFirstPrinter == true)
	{
		setError(PkgDischargeErrorFirstPrinter);
		Debug::trace("OperationModeStatus", "setError(PkgDischargeErrorFirstPrinter)");
	}

	if (pkgDischargeErrorSecondPrinter == true)
	{
		setError(PkgDischargeErrorSecondPrinter);
		Debug::trace("OperationModeStatus", "setError(pkgDischargeErrorSecondPrinter)");
	}

	if (pkgDischargeErrorThirdPrinter == true)
	{
		setError(PkgDischargeErrorThirdPrinter);
		Debug::trace("OperationModeStatus", "setError(pkgDischargeErrorThirdPrinter)");
	}

	if (productRequiredToWrap == true)
	{
		setError(ProductRequiredToWrap);
		Debug::trace("OperationModeStatus", "setError(ProductRequiredToWrap)");
	}
}

bool OperationModeStatus::isNewWrapperError()
{
	return newWrapperError;
}

void OperationModeStatus::setNewWrapperError(const WrapperErrorInfo::WrapperErrorSource &wrapperError)
{
	if (wrapperError == WrapperErrorInfo::NoErrorSource)
	{
		newWrapperError = false;
		this->wrapperError = WrapperErrorInfo::NoErrorSource;
		prevWrapperError = WrapperErrorInfo::NoErrorSource;
	}
	else
	{
		newWrapperError = true;
		prevWrapperError = this->wrapperError;
		this->wrapperError = wrapperError;
	}
}

void OperationModeStatus::clearNewWrapperError()
{
	newWrapperError = false;
	wrapperError = WrapperErrorInfo::NoErrorSource;
	prevWrapperError = WrapperErrorInfo::NoErrorSource;
}

const WrapperErrorInfo::WrapperErrorSource &OperationModeStatus::getWrapperError()
{
	return wrapperError;
}

void OperationModeStatus::setWrapperError(const WrapperErrorInfo::WrapperErrorSource &wrapperError)
{
	this->wrapperError = wrapperError;
}

const WrapperErrorInfo::WrapperErrorSource &OperationModeStatus::getPrevWrapperError()
{
	return prevWrapperError;
}

void OperationModeStatus::setPrevWrapperError(const WrapperErrorInfo::WrapperErrorSource &prevWrapperError)
{
	this->prevWrapperError = prevWrapperError;
}

bool OperationModeStatus::isNewEvolWrapperError()
{
	return newEvolWrapperError;
}

void OperationModeStatus::setNewEvolWrapperError(const EvolWrapperErrorInfo::EvolWrapperError &wrapperError,
												 const EvolWrapperErrorInfo::EvolWrapperErrorSource &wrapperErrorSource)
{
/*	if (wrapperError == EvolWrapperErrorInfo::NoWrapperError)
	{
		newEvolWrapperError = false;
		evolWrapperError = EvolWrapperErrorInfo::NoWrapperError;
		evolWrapperErrorSource = EvolWrapperErrorInfo::NoWrapperErrorSource;
		prevEvolWrapperError = EvolWrapperErrorInfo::NoWrapperError;
		prevEvolWrapperErrorSource = EvolWrapperErrorInfo::NoWrapperErrorSource;
	}
	else */
	{
		newEvolWrapperError = true;
		prevEvolWrapperError = evolWrapperError;
		evolWrapperError = wrapperError;
		prevEvolWrapperErrorSource = evolWrapperErrorSource;
		evolWrapperErrorSource = wrapperErrorSource;
	}
}

void OperationModeStatus::clearNewEvolWrapperError()
{
	newEvolWrapperError = false;
	evolWrapperError = EvolWrapperErrorInfo::NoWrapperError;
	evolWrapperErrorSource = EvolWrapperErrorInfo::NoWrapperErrorSource;
	prevEvolWrapperError = EvolWrapperErrorInfo::NoWrapperError;
	prevEvolWrapperErrorSource = EvolWrapperErrorInfo::NoWrapperErrorSource;
}

const EvolWrapperErrorInfo::EvolWrapperError &OperationModeStatus::getEvolWrapperError()
{
	return evolWrapperError;
}

void OperationModeStatus::setEvolWrapperError(const EvolWrapperErrorInfo::EvolWrapperError &wrapperError)
{
	this->evolWrapperError = wrapperError;
}

const EvolWrapperErrorInfo::EvolWrapperErrorSource &OperationModeStatus::getEvolWrapperErrorSource()
{
	return evolWrapperErrorSource;
}

void OperationModeStatus::setEvolWrapperErrorSource(const EvolWrapperErrorInfo::EvolWrapperErrorSource &wrapperErrorSource)
{
	this->evolWrapperErrorSource = wrapperErrorSource;
}

const EvolWrapperErrorInfo::EvolWrapperError &OperationModeStatus::getPrevEvolWrapperError()
{
	return prevEvolWrapperError;
}

void OperationModeStatus::setPrevEvolWrapperError(const EvolWrapperErrorInfo::EvolWrapperError &prevWrapperError)
{
	this->prevEvolWrapperError = prevWrapperError;
}

const EvolWrapperErrorInfo::EvolWrapperErrorSource &OperationModeStatus::getPrevEvolWrapperErrorSource()
{
	return prevEvolWrapperErrorSource;
}

void OperationModeStatus::setPrevEvolWrapperErrorSource(const EvolWrapperErrorInfo::EvolWrapperErrorSource &wrapperErrorSource)
{
	this->prevEvolWrapperErrorSource = wrapperErrorSource;
}

HobString OperationModeStatus::getWrapperErrorText(WrapperErrorInfo::WrapperErrorSource wrapperErrorSource)
{
	HobString errorSourceText = "";
	switch (wrapperErrorSource)
	{
		case WrapperErrorInfo::NoErrorSource:
			errorSourceText = "NoErrorSource";
										break;
		case WrapperErrorInfo::StopSwitchInterlock:
			errorSourceText = "StopSwitchInterlock";
										break;
		case WrapperErrorInfo::EepWriteFailure:
			errorSourceText = "EepWriteFailure";
										break;
		case WrapperErrorInfo::IntakeOutOfSync:
			errorSourceText = "IntakeOutOfSync";
										break;
		case WrapperErrorInfo::IntakePackagePresent:
			errorSourceText = "IntakePackagePresent";
										break;
		case WrapperErrorInfo::MainMotorJammed:
			errorSourceText = "MainMotorJammed";
										break;
		case WrapperErrorInfo::IntakeMotorJammed:
			errorSourceText = "IntakeMotorJammed";
										break;
		case WrapperErrorInfo::IntakeLengthFailure:
			errorSourceText = "IntakeLengthFailure";
										break;
		case WrapperErrorInfo::IntakeWidthFailure:
		 	errorSourceText = "IntakeWidthFailure";
										break;
		case WrapperErrorInfo::IntakeHomeFailure:
			errorSourceText = "IntakeHomeFailure";
										break;
		case WrapperErrorInfo::PackageJammed:
			errorSourceText = "PackageJammed";
										break;
		case WrapperErrorInfo::WideFilmFailure:
			errorSourceText = "WideFilmFailure";
										break;
		case WrapperErrorInfo::NarrowFilmFailure:
			errorSourceText = "NarrowFilmFailure";
										break;
		case WrapperErrorInfo::RightSideCoverInterlock:
			errorSourceText = "RightSideCoverInterlock";
										break;
		case WrapperErrorInfo::LeftSideCoverInterlock:
			errorSourceText = "LeftSideCoverInterlock";
										break;
		case WrapperErrorInfo::FrontCoverInterlock: 
			errorSourceText = "FrontCoverInterlock";
										break;
		case WrapperErrorInfo::RightCanopyInterlock:
			errorSourceText = "RightCanopyInterlock";
										break;
		case WrapperErrorInfo::LeftCanopyInterlock:
			errorSourceText = "LeftCanopyInterlock";
										break;
		case WrapperErrorInfo::TooTallInterlock:
			errorSourceText = "TooTallInterlock";
										break;
		case WrapperErrorInfo::FilmDoorInterlock: 
			errorSourceText = "FilmDoorInterlock";
										break;
		case WrapperErrorInfo::HandCrankDoorInterlock:
			errorSourceText = "HandCrankDoorInterlock";
										break;
		case WrapperErrorInfo::IntakeInterlock:
			errorSourceText = "IntakeInterlock";
										break;
		case WrapperErrorInfo::ElevatorInterlock:
			errorSourceText = "ElevatorInterlock";
										break;
		case WrapperErrorInfo::SealerBeltInterlock:
			errorSourceText = "SealerBeltInterlock";
										break;
		case WrapperErrorInfo::GeneralFault2:
			errorSourceText = "GeneralFault2";
										break;
		case WrapperErrorInfo::GeneralFault1:
			errorSourceText = "GeneralFault1";
										break;
		case WrapperErrorInfo::PackageTooLarge:
			errorSourceText = "PackageTooLarge";
										break;
		case WrapperErrorInfo::HeaterFailure:
			errorSourceText = "HeaterFailure";
										break;
		default:
		{
			errorSourceText = "Unknown: ";
			errorSourceText.append(Integer::toString((UINT16) wrapperErrorSource));
										break;
	}
}
	return errorSourceText;
}


bool OperationModeStatus::isBakeryCLAPackageMissing()
{
	Debug::trace("OperationModeStatus", "isBakeryCLAPackageMissing() = " + HobString(Bool::toString(bakeryCLAPackageMissing)));
	return bakeryCLAPackageMissing;
}

void OperationModeStatus::setBakeryCLAPackageMissing(bool value)
{
	bakeryCLAPackageMissing = value;
//	setError();
	Debug::trace("OperationModeStatus", "setBakeryCLAPackageMissing() = " + HobString(Bool::toString(bakeryCLAPackageMissing)));
}

void OperationModeStatus::clearBakeryCLAPackageMissing()
{
	bakeryCLAPackageMissing = false;
//	setError();
	Debug::trace("OperationModeStatus", "clearBakeryCLAPackageMissing() = " + HobString(Bool::toString(bakeryCLAPackageMissing)));
}


bool OperationModeStatus::isBakeryCLALabelMissing()
{
	Debug::trace("OperationModeStatus", "isBakeryCLALabelMissing() = " + HobString(Bool::toString(bakeryCLALabelMissing)));
	return bakeryCLALabelMissing;
}
void OperationModeStatus::setBakeryCLALabelMissing(bool value)
{
	bakeryCLALabelMissing = value;
//	setError();
	Debug::trace("OperationModeStatus", "setBakeryCLALabelMissing() = " + HobString(Bool::toString(bakeryCLALabelMissing)));
}
void OperationModeStatus::clearBakeryCLALabelMissing()
{
	bakeryCLALabelMissing = false;
//	setError();
	Debug::trace("OperationModeStatus", "clearBakeryCLALabelMissing() = " + HobString(Bool::toString(bakeryCLALabelMissing)));
}

bool OperationModeStatus::isPkgDischargeError(PrinterType printerType)
{
	bool rv = false;

	if (printerType == FirstPrinter)
	{
		rv = pkgDischargeErrorFirstPrinter;
		Debug::trace("OperationModeStatus", "isPkgDischargeError(FirstPrinter) = " + HobString(Bool::toString(pkgDischargeErrorFirstPrinter)));
	}
	else if (printerType == SecondPrinter)
	{
		rv = pkgDischargeErrorSecondPrinter;
		Debug::trace("OperationModeStatus", "isPkgDischargeError(SecondPrinter) = " + HobString(Bool::toString(pkgDischargeErrorSecondPrinter)));
	}
	else if (printerType == ThirdPrinter)
	{
		rv = pkgDischargeErrorThirdPrinter;
		Debug::trace("OperationModeStatus", "isPkgDischargeError(ThirdPrinter) = " + HobString(Bool::toString(pkgDischargeErrorThirdPrinter)));
	}
	return rv;
}

void OperationModeStatus::setPkgDischargeError(PrinterType printerType, bool value)
{
	if (printerType == FirstPrinter)
	{
		pkgDischargeErrorFirstPrinter = value;
		Debug::trace("OperationModeStatus", "setPkgDischargeError():  pkgDischargeErrorFirstPrinter = " + HobString(Bool::toString(pkgDischargeErrorFirstPrinter)));
	}
	else if (printerType == SecondPrinter)
	{
		pkgDischargeErrorSecondPrinter = value;
		Debug::trace("OperationModeStatus", "setPkgDischargeError():  pkgDischargeErrorSecondPrinter = " + HobString(Bool::toString(pkgDischargeErrorSecondPrinter)));
	}
	else if (printerType == ThirdPrinter)
	{
		pkgDischargeErrorThirdPrinter = value;
		Debug::trace("OperationModeStatus", "setPkgDischargeError():  pkgDischargeErrorThirdPrinter = " + HobString(Bool::toString(pkgDischargeErrorThirdPrinter)));
	}
	setError();
}

void OperationModeStatus::clearPkgDischargeError(PrinterType printerType)
{
	if (printerType == FirstPrinter)
	{
		pkgDischargeErrorFirstPrinter = false;
		Debug::trace("OperationModeStatus", "clearPkgDischargeError():  pkgDischargeErrorFirstPrinter = " + HobString(Bool::toString(pkgDischargeErrorFirstPrinter)));
	}
	else if (printerType == SecondPrinter)
	{
		pkgDischargeErrorSecondPrinter = false;
		Debug::trace("OperationModeStatus", "clearPkgDischargeError():  pkgDischargeErrorSecondPrinter = " + HobString(Bool::toString(pkgDischargeErrorSecondPrinter)));
	}
	else if (printerType == ThirdPrinter)
	{
		pkgDischargeErrorThirdPrinter = false;
		Debug::trace("OperationModeStatus", "clearPkgDischargeError():  pkgDischargeErrorThirdPrinter = " + HobString(Bool::toString(pkgDischargeErrorThirdPrinter)));
	}
	setError();
}

bool OperationModeStatus::isAnyPkgDischargeError()
{
	bool rv = false;

	rv = isPkgDischargeError(FirstPrinter);
	if (!rv)
	{
		rv = isPkgDischargeError(SecondPrinter);
		if (!rv)
			rv = isPkgDischargeError(ThirdPrinter);
	}
	Debug::trace("OperationModeStatus", "isAnyPkgDischargeError() = " + HobString(Bool::toString(rv)));
	return rv;
}

bool OperationModeStatus::isProductRequiredToWrap()
{
	return productRequiredToWrap;
}

void OperationModeStatus::setProductRequiredToWrap(bool value)
{
	productRequiredToWrap = value;
	setError();
}

void OperationModeStatus::clearProductRequiredToWrap()
{
	productRequiredToWrap = false;
	setError();
}

