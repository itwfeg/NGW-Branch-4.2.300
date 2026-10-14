#include "OpModePackage.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OpModePackage::dataType = 0;
std::string OpModePackage::dataTypeName("OpModePackage"); 
#endif

OpModePackage::OpModePackage(
	const ProductTransaction *pTransaction,
	UINT8 packId,
	LaPosLoc primaryPlacement,
	LaRotation primaryRotation,
	LaPosLoc secondaryPlacement,
	LaRotation secondaryRotation,
	Hobart::CEvolMsgReceiver *pFirstPrinter,
	Hobart::CEvolMsgReceiver *pSecondPrinter,
	Hobart::CEvolMsgReceiver *pThirdPrinter,
	bool invalidPackage) :
		pTransaction(pTransaction),
		packId(packId),
		primaryPlacement(primaryPlacement),
		primaryRotation(primaryRotation),
		secondaryPlacement(secondaryPlacement),
		secondaryRotation(secondaryRotation),
		pFirstPrinter(pFirstPrinter),
		pSecondPrinter(pSecondPrinter),
		pThirdPrinter(pThirdPrinter),
		firstPrinterCompleted(false),
		secondPrinterCompleted(false),
		thirdPrinterCompleted(false),
		invalidPackage(invalidPackage)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OpModePackage::~OpModePackage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void OpModePackage::setProductTransaction(const ProductTransaction *pTransaction)
{
	this->pTransaction = pTransaction;
}

const ProductTransaction *OpModePackage::getProductTransaction() const
{
	return pTransaction;
}


void OpModePackage::setPackId(UINT8 packId)
{
	this->packId = packId;
}

UINT8 OpModePackage::getPackId() const
{
	return packId;
}

void OpModePackage::setPrimaryPlacement(LaPosLoc primaryPlacement)
{
	this->primaryPlacement = primaryPlacement;
}

LaPosLoc OpModePackage::getPrimaryPlacement() const
{
	return primaryPlacement;
}

void OpModePackage::setPrimaryRotation(LaRotation primaryRotation)
{
	this->primaryRotation = primaryRotation;
}

LaRotation OpModePackage::getPrimaryRotation() const
{
	return primaryRotation;
}

void OpModePackage::setSecondaryPlacement(LaPosLoc secondaryPlacement)
{
	this->secondaryPlacement = secondaryPlacement;
}

LaPosLoc OpModePackage::getSecondaryPlacement() const
{
	return secondaryPlacement;
}

void OpModePackage::setSecondaryRotation(LaRotation secondaryRotation)
{
	this->secondaryRotation = secondaryRotation;
}

LaRotation OpModePackage::getSecondaryRotation() const
{
	return secondaryRotation;
}

void OpModePackage::setFirstPrinter(Hobart::CEvolMsgReceiver *pFirstPrinter)
{
	this->pFirstPrinter = pFirstPrinter;
}

Hobart::CEvolMsgReceiver *OpModePackage::getFirstPrinter() const
{
	return pFirstPrinter;
}

void OpModePackage::setSecondPrinter(Hobart::CEvolMsgReceiver *pSecondPrinter)
{
	this->pSecondPrinter = pSecondPrinter;
}

Hobart::CEvolMsgReceiver *OpModePackage::getSecondPrinter() const
{
	return pSecondPrinter;
}

void OpModePackage::setThirdPrinter(Hobart::CEvolMsgReceiver *pThirdPrinter)
{
	this->pThirdPrinter = pThirdPrinter;
}

Hobart::CEvolMsgReceiver *OpModePackage::getThirdPrinter() const
{
	return pThirdPrinter;
}

void OpModePackage::setCompleted(Hobart::CEvolMsgReceiver *pPrinter)
{
	if (pPrinter == pFirstPrinter)
	{
		firstPrinterCompleted = true;
	}
	else if (pPrinter == pSecondPrinter)
	{
		secondPrinterCompleted = true;
	}
	else if (pPrinter == pThirdPrinter)
	{
		thirdPrinterCompleted = true;
	}
}

void OpModePackage::clearCompleted(Hobart::CEvolMsgReceiver *pPrinter)
{
	if (pPrinter == pFirstPrinter)
	{
		firstPrinterCompleted = false;
	}
	else if (pPrinter == pSecondPrinter)
	{
		secondPrinterCompleted = false;
	}
	else if (pPrinter == pThirdPrinter)
	{
		thirdPrinterCompleted = false;
	}
}

bool OpModePackage::isCompleted(Hobart::CEvolMsgReceiver *pPrinter) const
{
	bool rv = false;

	if (pPrinter == pFirstPrinter)
	{
		rv = firstPrinterCompleted;
	}
	else if (pPrinter == pSecondPrinter)
	{
		rv = secondPrinterCompleted;
	}
	else if (pPrinter == pThirdPrinter)
	{
		rv = thirdPrinterCompleted;
	}

	return rv;
}

bool OpModePackage::isCompleted(PrinterType prType) const
{
	bool rv = true;

	switch (prType)
	{
		case FirstPrinter:
				if (pFirstPrinter != NULL)
				{
					rv = firstPrinterCompleted;
				}
				break;
		case SecondPrinter:
				if (pSecondPrinter != NULL)
				{
					rv = secondPrinterCompleted;
				}
				break;
		case ThirdPrinter:
				if (pThirdPrinter != NULL)
				{
					rv = thirdPrinterCompleted;
				}
				break;
		default:
				break;
	}
	
	return rv;
}

void OpModePackage::setInvalidPackage(bool invalidPackage)
{
	this->invalidPackage = invalidPackage;
}

bool OpModePackage::getInvalidPackage() const
{
	return invalidPackage;
}

void OpModePackage::clearInvalidPackage()
{
	invalidPackage = false;
}

bool OpModePackage::isTotalCompleted() const
{
	bool rv = true;

	if (pFirstPrinter != NULL)
	{
		rv = firstPrinterCompleted;
	}

	if ((rv == true) && (pSecondPrinter != NULL))
	{
		rv = secondPrinterCompleted;
	}

	if ((rv == true) && (pThirdPrinter != NULL))
	{
		rv = thirdPrinterCompleted;
	}

	return rv;
}

bool OpModePackage::newLabelApplierSetup(const LabelApplierSetup &laSetup) const
{
	bool isTracing = Debug::isTracing("OpModePackage");
	HobString msgDebug = "";

	if (isTracing)
	{
		msgDebug = "newLabelApplierSetup()  ";

		HobString msgDebug2 = msgDebug;
		msgDebug2.append("  primaryPlacement = ").append(Integer::toString(primaryPlacement));
		msgDebug2.append("  laSetup.primaryPlacement = ").append(Integer::toString(laSetup.getPrimaryLabelPlacement()));
		Debug::trace("OpModePackage", msgDebug2);
	}
	bool expr1 = ((laSetup.getPrimaryLabelSize() > 0) && (pFirstPrinter != 0) &&
				  ((primaryPlacement != laSetup.getPrimaryLabelPlacement()) || (primaryRotation != laSetup.getPrimaryLabelRotation())));
	bool expr2 = ((laSetup.getSecondaryLabelSize() > 0) && (pSecondPrinter != 0) &&
				  ((secondaryPlacement != laSetup.getSecondaryLabelPlacement()) || (secondaryRotation != laSetup.getSecondaryLabelRotation())));

	if (isTracing)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("  expr1 = ").append(Bool::toString(expr1));
		msgDebug2.append("  expr2 = ").append(Bool::toString(expr2));
		msgDebug2.append("  newSetup = ").append(Bool::toString(expr1 || expr2));
		Debug::trace("OpModePackage", msgDebug2);
	}
	return  (expr1 || expr2);
}

void OpModePackage::show() const
{
	bool isTracing = Debug::isTracing("OpModePackage");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("OpModePackage") );
#else
		Debug::addTraceCategory("OpModePackage");
#endif
	}

	Debug::trace("OpModePackage", "OpModePackage: ");

	Debug::trace("OpModePackage", HobString("pTransaction: 0x") + HobString(Hex::toString((long long) pTransaction)));
	Debug::trace("OpModePackage", HobString("packId: ") + HobString(Integer::toString(packId)));
	Debug::trace("OpModePackage", HobString("primaryPlacement: ") + HobString(Integer::toString(primaryPlacement)));
	Debug::trace("OpModePackage", HobString("primaryRotation: ") + HobString(Integer::toString(primaryRotation)));
	Debug::trace("OpModePackage", HobString("secondaryPlacement: ") + HobString(Integer::toString(secondaryPlacement)));
	Debug::trace("OpModePackage", HobString("secondaryRotation: ") + HobString(Integer::toString(secondaryRotation)));
	Debug::trace("OpModePackage", HobString("pFirstPrinter: 0x") + HobString(Hex::toString((long long) pFirstPrinter)));
	Debug::trace("OpModePackage", HobString("firstPrinterCompleted: ") + HobString(Bool::toString(firstPrinterCompleted)));
	Debug::trace("OpModePackage", HobString("pSecondPrinter: 0x") + HobString(Hex::toString((long long) pSecondPrinter)));
	Debug::trace("OpModePackage", HobString("secondPrinterCompleted: ") + HobString(Bool::toString(secondPrinterCompleted)));
	Debug::trace("OpModePackage", HobString("pThirdPrinter: 0x") + HobString(Hex::toString((long long) pThirdPrinter)));
	Debug::trace("OpModePackage", HobString("thirdPrinterCompleted: ") + HobString(Bool::toString(thirdPrinterCompleted)));
	Debug::trace("OpModePackage", HobString("totalCompleted: ") + HobString(Bool::toString(isTotalCompleted())));
	Debug::trace("OpModePackage", HobString("invalidPackage: ") + HobString(Bool::toString(invalidPackage)));

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("OpModePackage") );
#else
		Debug::removeTraceCategory("OpModePackage");
#endif
	}
}
