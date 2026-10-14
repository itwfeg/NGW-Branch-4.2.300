/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SystemEvent.h"
#include "HfcPeripheralList.h"

#include "WeigherWeigherKind.h"
#include "PrinterPrinterKind.h"
#include "LabelApplierLabelApplierKind.h"
#include "WrapperWrapperKind.h"
#include "NumericNumericKind.h"
#include "MarqueeMarqueeKind.h"
#include "GCDGCDKind.h"

#include "Debug.h"
#include "UInteger.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPeripheralList::dataType = 0;
std::string HfcPeripheralList::dataTypeName("HfcPeripheralList"); 
#endif

HfcPeripheralList::HfcPeripheralList(const HfcPeripheralList& obj) : EvolMessage(System, SysPeripheralList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initialize();
	setFormatType(InternalFormat);

	(*this) = obj;
}

HfcPeripheralList::HfcPeripheralList(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(System, SysPeripheralList, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initialize();
	setFormatType(InternalFormat);

}

HfcPeripheralList::HfcPeripheralList(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	listCS.Enter();

	EvolMessage::serialize();

	int j = EvolMessage::getSerializedLength();

	// Now, for the derived class items
	deviceKinds[fieldOffsetFirstWeigher] = new WeigherWeigherKind(&buffer[j]);
	deviceKinds[fieldOffsetFirstWeigher]->serialize();
	j += deviceKinds[fieldOffsetFirstWeigher]->getSerializedLength();

	deviceKinds[fieldOffsetSecondWeigher] = new WeigherWeigherKind(&buffer[j]);
	deviceKinds[fieldOffsetSecondWeigher]->serialize();
	j += deviceKinds[fieldOffsetSecondWeigher]->getSerializedLength();

	deviceKinds[fieldOffsetFirstPrinter] = new PrinterPrinterKind(&buffer[j]);
	deviceKinds[fieldOffsetFirstPrinter]->serialize();
	j += deviceKinds[fieldOffsetFirstPrinter]->getSerializedLength();

	deviceKinds[fieldOffsetSecondPrinter] = new PrinterPrinterKind(&buffer[j]);
	deviceKinds[fieldOffsetSecondPrinter]->serialize();
	j += deviceKinds[fieldOffsetSecondPrinter]->getSerializedLength();

	deviceKinds[fieldOffsetThirdPrinter] = new PrinterPrinterKind(&buffer[j]);
	deviceKinds[fieldOffsetThirdPrinter]->serialize();
	j += deviceKinds[fieldOffsetThirdPrinter]->getSerializedLength();

	deviceKinds[fieldOffsetLabelApplier] = new LabelApplierLabelApplierKind(&buffer[j]);
	deviceKinds[fieldOffsetLabelApplier]->serialize();
	j += deviceKinds[fieldOffsetLabelApplier]->getSerializedLength();

	deviceKinds[fieldOffsetWrapper] = new WrapperWrapperKind(&buffer[j]);
	deviceKinds[fieldOffsetWrapper]->serialize();
	j += deviceKinds[fieldOffsetWrapper]->getSerializedLength();

	deviceKinds[fieldOffsetNumeric] = new NumericNumericKind(&buffer[j]);
	deviceKinds[fieldOffsetNumeric]->serialize();
	j += deviceKinds[fieldOffsetNumeric]->getSerializedLength();

	deviceKinds[fieldOffsetMarquee] = new MarqueeMarqueeKind(&buffer[j]);
	deviceKinds[fieldOffsetMarquee]->serialize();
	j += deviceKinds[fieldOffsetMarquee]->getSerializedLength();

	deviceKinds[fieldOffsetGCD] = new GCDGCDKind(&buffer[j]);
	deviceKinds[fieldOffsetGCD]->serialize();
	j += deviceKinds[fieldOffsetGCD]->getSerializedLength();

	setFormatType(InternalFormat);

	listCS.Leave();
}

HfcPeripheralList::~HfcPeripheralList()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	for (int i = fieldOffsetFirstWeigher; i < EndHfcPeripheralListFieldOffsets; i++)
	{
		delete deviceKinds[i];
	}
}

const char *HfcPeripheralList::serialize() const
{
	return serialize(InternalFormat);
}

const char *HfcPeripheralList::serialize(int format) const
{
	listCS.Enter();

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case InternalFormat:
			{
				// Start here and add the fields for Serializing the Evolution style message!!
				EvolMessage::serialize();

				// Now, for the derived class items
				const char *pCC = deviceKinds[fieldOffsetFirstWeigher]->serialize();
				int mLength = deviceKinds[fieldOffsetFirstWeigher]->getSerializedLength();
				EvolMessage::serializedData.append(pCC, mLength);

				pCC = deviceKinds[fieldOffsetSecondWeigher]->serialize();
				mLength = deviceKinds[fieldOffsetSecondWeigher]->getSerializedLength();
				EvolMessage::serializedData.append(pCC, mLength);

				pCC = deviceKinds[fieldOffsetFirstPrinter]->serialize();
				mLength = deviceKinds[fieldOffsetFirstPrinter]->getSerializedLength();
				EvolMessage::serializedData.append(pCC, mLength);

				pCC = deviceKinds[fieldOffsetSecondPrinter]->serialize();
				mLength = deviceKinds[fieldOffsetSecondPrinter]->getSerializedLength();
				EvolMessage::serializedData.append(pCC, mLength);

				pCC = deviceKinds[fieldOffsetThirdPrinter]->serialize();
				mLength = deviceKinds[fieldOffsetThirdPrinter]->getSerializedLength();
				EvolMessage::serializedData.append(pCC, mLength);

				pCC = deviceKinds[fieldOffsetLabelApplier]->serialize();
				mLength = deviceKinds[fieldOffsetLabelApplier]->getSerializedLength();
				EvolMessage::serializedData.append(pCC, mLength);

				pCC = deviceKinds[fieldOffsetWrapper]->serialize();
				mLength = deviceKinds[fieldOffsetWrapper]->getSerializedLength();
				EvolMessage::serializedData.append(pCC, mLength);

				pCC = deviceKinds[fieldOffsetNumeric]->serialize();
				mLength = deviceKinds[fieldOffsetNumeric]->getSerializedLength();
				EvolMessage::serializedData.append(pCC, mLength);

				pCC = deviceKinds[fieldOffsetMarquee]->serialize();
				mLength = deviceKinds[fieldOffsetMarquee]->getSerializedLength();
				EvolMessage::serializedData.append(pCC, mLength);

				pCC = deviceKinds[fieldOffsetGCD]->serialize();
				mLength = deviceKinds[fieldOffsetGCD]->getSerializedLength();
				EvolMessage::serializedData.append(pCC, mLength);
			}
			break;
		case BrowserFormat:
			EvolMessage::serializedData = "";

			EvolMessage::serializedData = UInteger::toString((UINT16) getDeviceType());
			EvolMessage::serializedData.append(BrowserFormatSeparator);
			EvolMessage::serializedData.append(UInteger::toString((UINT16) getEventType()));

			for (int loopCnt = 0; loopCnt < EndHfcPeripheralListFieldOffsets; loopCnt++)
			{
				EvolMessage::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetFirstWeigher:
						EvolMessage::serializedData.append(UInteger::toString((UINT16) getFirstWeigher()));
						break;
					case fieldOffsetSecondWeigher:
						EvolMessage::serializedData.append(UInteger::toString((UINT16) getSecondWeigher()));
						break;
					case fieldOffsetFirstPrinter:
						EvolMessage::serializedData.append(UInteger::toString((UINT16) getFirstPrinter()));
						break;
					case fieldOffsetSecondPrinter:
						EvolMessage::serializedData.append(UInteger::toString((UINT16) getSecondPrinter()));
						break;
					case fieldOffsetThirdPrinter:
						EvolMessage::serializedData.append(UInteger::toString((UINT16) getThirdPrinter()));
						break;
					case fieldOffsetLabelApplier:
						EvolMessage::serializedData.append(UInteger::toString((UINT16) getLabelApplier()));
						break;
					case fieldOffsetWrapper:
						EvolMessage::serializedData.append(UInteger::toString((UINT16) getWrapper()));
						break;
					case fieldOffsetNumeric:
						EvolMessage::serializedData.append(UInteger::toString((UINT16) getNumeric()));
						break;
					case fieldOffsetMarquee:
						EvolMessage::serializedData.append(UInteger::toString((UINT16) getMarquee()));
						break;
					case fieldOffsetGCD:
						EvolMessage::serializedData.append(UInteger::toString((UINT16) getGCD()));
						break;
					default:
						break;
				}
			}
			break;

		default:
			Debug::trace("HfcPeripheralList", HobString("serialize(int format): Unknown ") +
											  HobString(UInteger::toString((UINT16) format)));
			break;
	}

	listCS.Leave();

	return EvolMessage::serializedData.c_str();
}

void HfcPeripheralList::show() const
{
	bool isTracing = Debug::isTracing("HfcPeripheralList");

	if (isTracing == false)
	{
		Debug::addTraceCategory("HfcPeripheralList");
	}

	Debug::trace("HfcPeripheralList", "HfcPeripheralList: ");

	EvolMessage::show();

	Debug::trace("HfcPeripheralList", HobString("FirstWeigher: ") + UInteger::toString(getFirstWeigher()));
	Debug::trace("HfcPeripheralList", HobString("SecondWeigher: ") + UInteger::toString(getSecondWeigher()));
	Debug::trace("HfcPeripheralList", HobString("FirstPrinter: ") + UInteger::toString(getFirstPrinter()));
	Debug::trace("HfcPeripheralList", HobString("SecondPrinter: ") + UInteger::toString(getSecondPrinter()));
	Debug::trace("HfcPeripheralList", HobString("ThirdPrinter: ") + UInteger::toString(getThirdPrinter()));
	Debug::trace("HfcPeripheralList", HobString("LabelApplier: ") + UInteger::toString(getLabelApplier()));
	Debug::trace("HfcPeripheralList", HobString("Wrapper: ") + UInteger::toString(getWrapper()));
	Debug::trace("HfcPeripheralList", HobString("Numeric: ") + UInteger::toString(getNumeric()));
	Debug::trace("HfcPeripheralList", HobString("Marquee: ") + UInteger::toString(getMarquee()));
	Debug::trace("HfcPeripheralList", HobString("GCD: ") + UInteger::toString(getGCD()));

	if (getFormatType() == InternalFormat)
	{
		Debug::trace("HfcPeripheralList", HobString("formatType: InternalFormat"));
	}
	else if (getFormatType() == BrowserFormat)
	{
		Debug::trace("HfcPeripheralList", HobString("formatType: BrowserFormat"));
	}
	else
	{
		Debug::trace("HfcPeripheralList", HobString("formatType: Unknown ") +
										  HobString(UInteger::toString((UINT16) getFormatType())));
	}

	if (isTracing == false)
	{
		Debug::removeTraceCategory("HfcPeripheralList");
	}
}

void HfcPeripheralList::setFirstWeigher(WeigherKind wgKind)
{
	listCS.Enter();
	dynamic_cast<WeigherWeigherKind *> (deviceKinds[fieldOffsetFirstWeigher])->setWeigherKind(wgKind);
	listCS.Leave();
}

WeigherKind HfcPeripheralList::getFirstWeigher() const
{
	return dynamic_cast<WeigherWeigherKind *> (deviceKinds[fieldOffsetFirstWeigher])->getWeigherKind();
}

void HfcPeripheralList::setSecondWeigher(WeigherKind wgKind)
{
	listCS.Enter();
	dynamic_cast<WeigherWeigherKind *> (deviceKinds[fieldOffsetSecondWeigher])->setWeigherKind(wgKind);
	listCS.Leave();
}

WeigherKind HfcPeripheralList::getSecondWeigher() const
{
	return dynamic_cast<WeigherWeigherKind *> (deviceKinds[fieldOffsetSecondWeigher])->getWeigherKind();
}

void HfcPeripheralList::setFirstPrinter(PrinterKind prKind)
{
	listCS.Enter();
	if ( prKind != FakePrinter )
		dynamic_cast<PrinterPrinterKind *> (deviceKinds[fieldOffsetFirstPrinter])->setPrinterKind(prKind);
	listCS.Leave();
}

PrinterKind HfcPeripheralList::getFirstPrinter() const
{
	return dynamic_cast<PrinterPrinterKind *> (deviceKinds[fieldOffsetFirstPrinter])->getPrinterKind();
}

void HfcPeripheralList::setSecondPrinter(PrinterKind prKind)
{
	listCS.Enter();
	if ( prKind != FakePrinter )
		dynamic_cast<PrinterPrinterKind *> (deviceKinds[fieldOffsetSecondPrinter])->setPrinterKind(prKind);
	listCS.Leave();
}

PrinterKind HfcPeripheralList::getSecondPrinter() const
{
	return dynamic_cast<PrinterPrinterKind *> (deviceKinds[fieldOffsetSecondPrinter])->getPrinterKind();
}

void HfcPeripheralList::setThirdPrinter(PrinterKind prKind)
{
	listCS.Enter();
	if ( prKind != FakePrinter )
		dynamic_cast<PrinterPrinterKind *> (deviceKinds[fieldOffsetThirdPrinter])->setPrinterKind(prKind);
	listCS.Leave();
}

PrinterKind HfcPeripheralList::getThirdPrinter() const
{
	return dynamic_cast<PrinterPrinterKind *> (deviceKinds[fieldOffsetThirdPrinter])->getPrinterKind();
}

void HfcPeripheralList::setLabelApplier(LabelApplierKind laKind)
{
	listCS.Enter();
	dynamic_cast<LabelApplierLabelApplierKind *> (deviceKinds[fieldOffsetLabelApplier])->setLabelApplierKind(laKind);
	listCS.Leave();
}

LabelApplierKind HfcPeripheralList::getLabelApplier() const
{
	return dynamic_cast<LabelApplierLabelApplierKind *> (deviceKinds[fieldOffsetLabelApplier])->getLabelApplierKind();
}

void HfcPeripheralList::setWrapper(WrapperKind wrKind)
{
	listCS.Enter();
	dynamic_cast<WrapperWrapperKind *> (deviceKinds[fieldOffsetWrapper])->setWrapperKind(wrKind);
	listCS.Leave();
}

WrapperKind HfcPeripheralList::getWrapper() const
{
	return dynamic_cast<WrapperWrapperKind *> (deviceKinds[fieldOffsetWrapper])->getWrapperKind();
}

void HfcPeripheralList::setNumeric(NumericKind nmKind)
{
	listCS.Enter();
	dynamic_cast<NumericNumericKind *> (deviceKinds[fieldOffsetNumeric])->setNumericKind(nmKind);
	listCS.Leave();
}

NumericKind HfcPeripheralList::getNumeric() const
{
	return dynamic_cast<NumericNumericKind *> (deviceKinds[fieldOffsetNumeric])->getNumericKind();
}

void HfcPeripheralList::setMarquee(MarqueeKind mqKind)
{
	listCS.Enter();
	dynamic_cast<MarqueeMarqueeKind *> (deviceKinds[fieldOffsetMarquee])->setMarqueeKind(mqKind);
	listCS.Leave();
}

MarqueeKind HfcPeripheralList::getMarquee() const
{
	return dynamic_cast<MarqueeMarqueeKind *> (deviceKinds[fieldOffsetMarquee])->getMarqueeKind();
}


void HfcPeripheralList::setGCD(GCDKind gcdKind)
{
	listCS.Enter();
	dynamic_cast<GCDGCDKind *> (deviceKinds[fieldOffsetGCD])->setGCDKind(gcdKind);
	listCS.Leave();
}

GCDKind HfcPeripheralList::getGCD() const
{
	return dynamic_cast<GCDGCDKind *> (deviceKinds[fieldOffsetGCD])->getGCDKind();
}

void HfcPeripheralList::operator=(const HfcPeripheralList &periphList)
{
	Debug::trace("HfcPeripheralList", "operator=(const HfcPeripheralList &periphList)");
	setFirstWeigher(periphList.getFirstWeigher());
	setSecondWeigher(periphList.getSecondWeigher());
	setFirstPrinter(periphList.getFirstPrinter());
	setSecondPrinter(periphList.getSecondPrinter());
	setThirdPrinter(periphList.getThirdPrinter());
	setLabelApplier(periphList.getLabelApplier());
	setWrapper(periphList.getWrapper());
	setNumeric(periphList.getNumeric());
	setMarquee(periphList.getMarquee());
	setGCD(periphList.getGCD());
	Debug::trace("HfcPeripheralList", "operator=(const HfcPeripheralList &periphList) - Done");
}

// Is any weigher attached?
bool HfcPeripheralList::isWeigherAttached() const
{
	if (isWeigherAttached(FirstWeigher))
		return true;
	else if (isWeigherAttached(SecondWeigher))
		return true;
	else
		return false;
}

// Is a specific weigher (first or second) attached?
bool HfcPeripheralList::isWeigherAttached(WeigherType weigherNumber) const
{
	if (weigherNumber == FirstWeigher)
		return (getFirstWeigher() == WgNoWeigher) ? false : true;
	else if (weigherNumber == SecondWeigher)
		return (getSecondWeigher() == WgNoWeigher) ? false : true;
	else
		return false;
}

// Is any printer attached?
bool HfcPeripheralList::isPrinterAttached() const
{
	if (isPrinterAttached(FirstPrinter))
		return true;
	else if (isPrinterAttached(SecondPrinter))
		return true;
	else if (isPrinterAttached(ThirdPrinter))
		return true;
	else
		return false;
}

// Is a specific printer (first, second, or third) attached?
bool HfcPeripheralList::isPrinterAttached(PrinterType printerNumber) const
{
	if (printerNumber == FirstPrinter)
		return (getFirstPrinter() == NoPrinter) ? false : true;
	else if (printerNumber == SecondPrinter)
		return (getSecondPrinter() == NoPrinter) ? false : true;
	else if (printerNumber == ThirdPrinter)
		return (getThirdPrinter() == NoPrinter) ? false : true;
	else
		return false;
}

bool HfcPeripheralList::isWrapperAttached() const
{
	if (getWrapper() == WrNoWrapper)
		return false;
	else
		return true;
}
bool HfcPeripheralList::isEvolutionWrapperAttached() const
{
	if (getWrapper() == WrEvolWrapper)
		return true;
	else
		return false;
}
#ifdef SUPPORT_ULTIMA_WRAPPERS
bool HfcPeripheralList::isUltimaWrapperAttached() const
{
	if (getWrapper() == WrUltimaWrapper)
		return true;
	else
		return false;
}
#endif
bool HfcPeripheralList::isLabelApplierAttached() const
{
	if (getLabelApplier() == NoLabelApplier)
		return false;
	else
		return true;
}

bool HfcPeripheralList::isCLAAttached() const
{
	if (getLabelApplier() == CompactLabelApplier)
		return true;
	else
		return false;
}

bool HfcPeripheralList::isCLAEPSAC2Attached() const
{
	if (getLabelApplier() == CompactLabelApplier_EPSAC2)
		return true;
	else
		return false;
}
bool HfcPeripheralList::isILAAttached() const
{
	if (getLabelApplier() == ILA)
		return true;
	else
		return false;
}
bool HfcPeripheralList::isEvolLAAttached() const
{
	if (getLabelApplier() == EvolutionLabelApplier)
		return true;
	else
		return false;
}

bool HfcPeripheralList::isCustomerDisplayAttached() const		//	GCDR-158
{
	//Debug::trace("HfcPeripheralList", "isCustomerDisplayAttached():      GCDKind = " + (UInteger::toString(getGCD())));
	//Debug::trace("HfcPeripheralList", "isCustomerDisplayAttached():  NumericKind = " + (UInteger::toString(getNumeric())));
	//Debug::trace("HfcPeripheralList", "isCustomerDisplayAttached():  MarqueeKind = " + (UInteger::toString(getMarquee())));
	
	if ((getGCD() != GCDNoGCD) || (getNumeric() != NmNoNumeric) || (getMarquee() != MqNoMarquee))
		return true;
	else
		return false;
}

void HfcPeripheralList::initialize()
{
	deviceKinds[fieldOffsetFirstWeigher] = new WeigherWeigherKind(NULL, WgNoWeigher);
	deviceKinds[fieldOffsetSecondWeigher] = new WeigherWeigherKind(NULL, WgNoWeigher);
	deviceKinds[fieldOffsetFirstPrinter] = new PrinterPrinterKind(NULL, FirstPrinter, NoPrinter);
	deviceKinds[fieldOffsetSecondPrinter] = new PrinterPrinterKind(NULL, SecondPrinter, NoPrinter);
	deviceKinds[fieldOffsetThirdPrinter] = new PrinterPrinterKind(NULL, ThirdPrinter, NoPrinter);
	deviceKinds[fieldOffsetLabelApplier] = new LabelApplierLabelApplierKind(NULL, NoLabelApplier);
	deviceKinds[fieldOffsetWrapper] = new WrapperWrapperKind(NULL, WrNoWrapper);
	deviceKinds[fieldOffsetNumeric] = new NumericNumericKind(NULL, NmNoNumeric);
	deviceKinds[fieldOffsetMarquee] = new MarqueeMarqueeKind(NULL, MqNoMarquee);
	deviceKinds[fieldOffsetGCD] = new GCDGCDKind(NULL, GCDNoGCD);
}