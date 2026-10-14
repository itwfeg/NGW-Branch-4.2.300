#include "CompactLabelApplierConfiguration.h"
#include "CompactLabelApplierInquire.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"
#include "HobString.h"
#ifndef EVOL_NDEBUG
#include "utilstring.h"
#endif

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CompactLabelApplierConfiguration::dataType = 0;
std::string CompactLabelApplierConfiguration::dataTypeName("CompactLabelApplierConfiguration"); 
#endif

CompactLabelApplierConfiguration::CompactLabelApplierConfiguration(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(LabelApplier, LaConfig, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	configType = LaDefault;

	weigherAttached = true;
	primaryPrinterAttached = true;
	secondaryPrinterAttached = true;
	leftToRight = true;
	frontDischarge = true;
/*	primaryJustifyFont = true;
	secondaryJustifyFont = true;
	transferStationType = Bakery;
	Debug::trace("CompactLabelApplierConfiguration", "Default Constructor:  Initially setting transferStationType to Bakery");
*/	primaryPrinterLabelOffset = DEFAULT_CLA_LABEL_OFFSET;
	secondaryPrinterLabelOffset = DEFAULT_CLA_LABEL_OFFSET;
	//Alex Zavala 8/20
	labelPlacement = DEFAULT_LABEL_PLACEMENT;
	//Alex Zavala 11/7
	overdrive = DEFAULT_OVERDRIVE;
/*	weigherConveyorSensitivity = ConveyorSensitivityMedium;
	primaryPrinterConveyorSensitivity = ConveyorSensitivityHigh;
	secondaryPrinterConveyorSensitivity = ConveyorSensitivityHigh;

	primaryPrinterMaxJustificationDistance = DEFAULT_CLA_JUSTIFY_DISTANCE;
	secondaryPrinterMaxJustificationDistance = DEFAULT_CLA_JUSTIFY_DISTANCE;
	diagonalLabelPlacement = false;
	diagonalOffset	= 0;
	miniStationLabelPlacement = LaNoneSelected;
	deviceSignature[0]	= 'C';
	deviceSignature[1]	= 'L';
	deviceSignature[2]	= 'A';
	deviceSignature[3]	= '\0';
	deviceSignature[4]	= '\0';
	deviceSignature[5]	= '\0';
*/
}

CompactLabelApplierConfiguration::CompactLabelApplierConfiguration(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &configType;
	for (unsigned int i = 0; i < sizeof(configType); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &weigherAttached;
	for (unsigned int i = 0; i < sizeof(weigherAttached); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &primaryPrinterAttached;
	for (unsigned int i = 0; i < sizeof(primaryPrinterAttached); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secondaryPrinterAttached;
	for (unsigned int i = 0; i < sizeof(secondaryPrinterAttached); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &leftToRight;
	for (unsigned int i = 0; i < sizeof(leftToRight); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &frontDischarge;
	for (unsigned int i = 0; i < sizeof(frontDischarge); i++)
	{
		*pChar++ = buffer[j++];
	}
/*
	pChar = (char *) &primaryJustifyFont;
	for (unsigned int i = 0; i < sizeof(primaryJustifyFont); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secondaryJustifyFont;
	for (unsigned int i = 0; i < sizeof(secondaryJustifyFont); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &transferStationType;
	for (unsigned int i = 0; i < sizeof(transferStationType); i++)
	{
		*pChar++ = buffer[j++];
	}
	if (transferStationType == NSW)
	{
		Debug::trace("CompactLabelApplierConfiguration", "Constructor: transferStationType = NSW");
	}
	else if (transferStationType == HandWrapper)
	{
		Debug::trace("CompactLabelApplierConfiguration", "Constructor: transferStationType = HandWrapper");
	}
	else if (transferStationType == Bakery)
	{
		Debug::trace("CompactLabelApplierConfiguration", "Constructor: transferStationType = Bakery");
	}
	else if (transferStationType == ESW)
	{
		Debug::trace("CompactLabelApplierConfiguration", "Constructor: transferStationType = ESW");
	}
	else if (transferStationType == Mini)
	{
		Debug::trace("CompactLabelApplierConfiguration", "Constructor: transferStationType = Mini");
	}
	else
	{
		Debug::trace("CompactLabelApplierConfiguration", "Constructor: transferStationType = Unknown, " +
												  HobString(Integer::toString((UINT16) transferStationType)));
	}
*/	
	pChar = (char *) &primaryPrinterLabelOffset;
	for (unsigned int i = 0; i < sizeof(primaryPrinterLabelOffset); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secondaryPrinterLabelOffset;
	for (unsigned int i = 0; i < sizeof(secondaryPrinterLabelOffset); i++)
	{
		*pChar++ = buffer[j++];
	}

	//Alex Zavala 8/20
	pChar = (char *) &labelPlacement;
	for (unsigned int i = 0; i < sizeof(labelPlacement); i++)
	{
		*pChar++ = buffer[j++];
	}

	//Alex Zavala 11/7 new config parameter
	pChar = (char *) &overdrive;
	for (unsigned int i = 0; i < sizeof(overdrive); i++)
	{
		*pChar++ = buffer[j++];
	}
/*
	pChar = (char *) &weigherConveyorSensitivity;
	for (unsigned int i = 0; i < sizeof(weigherConveyorSensitivity); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &primaryPrinterConveyorSensitivity;
	for (unsigned int i = 0; i < sizeof(primaryPrinterConveyorSensitivity); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secondaryPrinterConveyorSensitivity;
	for (unsigned int i = 0; i < sizeof(secondaryPrinterConveyorSensitivity); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &primaryPrinterMaxJustificationDistance;
	for (unsigned int i = 0; i < sizeof(primaryPrinterMaxJustificationDistance); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secondaryPrinterMaxJustificationDistance;
	for (unsigned int i = 0; i < sizeof(secondaryPrinterMaxJustificationDistance); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &diagonalLabelPlacement;
	for (unsigned int i = 0; i < sizeof(diagonalLabelPlacement); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &diagonalOffset;
	for (unsigned int i = 0; i < sizeof(diagonalOffset); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &miniStationLabelPlacement;
	for (unsigned int i = 0; i < sizeof(miniStationLabelPlacement); i++)
	{
		*pChar++ = buffer[j++];
	}
	if (miniStationLabelPlacement == LaLeadingEdge)
	{
		Debug::trace("CompactLabelApplierConfiguration", "Constructor: miniStationLabelPlacement = LaLeadingEdge");
	}
	else if (miniStationLabelPlacement == LaTrailingEdge)
	{
		Debug::trace("CompactLabelApplierConfiguration", "Constructor: miniStationLabelPlacement = LaTrailingEdge");
	}
	else
	{
		Debug::trace("CompactLabelApplierConfiguration", "Constructor: miniStationLabelPlacement = LaNoneSelected");
	}

	pChar = (char *) &deviceSignature;
	for (unsigned int i = 0; i < sizeof(deviceSignature); i++)
	{
		*pChar++ = buffer[j++];
	}
*/
}

CompactLabelApplierConfiguration::~CompactLabelApplierConfiguration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *CompactLabelApplierConfiguration::serialize() const
{
#ifndef EVOL_NDEBUG
	Debug::trace("CompactLabelApplierConfiguration", "serialize()");
#endif
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &configType;
	for (unsigned int i = 0; i < sizeof(configType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &weigherAttached;
	for (unsigned int i = 0; i < sizeof(weigherAttached); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &primaryPrinterAttached;
	for (unsigned int i = 0; i < sizeof(primaryPrinterAttached); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secondaryPrinterAttached;
	for (unsigned int i = 0; i < sizeof(secondaryPrinterAttached); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &leftToRight;
	for (unsigned int i = 0; i < sizeof(leftToRight); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &frontDischarge;
	for (unsigned int i = 0; i < sizeof(frontDischarge); i++)
	{
		serializedData.append(1, pChar[i]);
	}
/*
	pChar = (char *) &primaryJustifyFont;
	for (unsigned int i = 0; i < sizeof(primaryJustifyFont); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secondaryJustifyFont;
	for (unsigned int i = 0; i < sizeof(secondaryJustifyFont); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &transferStationType;
	for (unsigned int i = 0; i < sizeof(transferStationType); i++)
	{
		serializedData.append(1, pChar[i]);
	}
*/	
	pChar = (char *) &primaryPrinterLabelOffset;
	for (unsigned int i = 0; i < sizeof(primaryPrinterLabelOffset); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secondaryPrinterLabelOffset;
	for (unsigned int i = 0; i < sizeof(secondaryPrinterLabelOffset); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	//Alex Zavala 8/20
	pChar = (char *) &labelPlacement;
	for (unsigned int i = 0; i < sizeof(labelPlacement); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	//Alex Zavala 11/7
	pChar = (char *) &overdrive;
	for (unsigned int i = 0; i < sizeof(overdrive); i++)
	{
		serializedData.append(1, pChar[i]);
	}
/*
	pChar = (char *) &weigherConveyorSensitivity;
	for (unsigned int i = 0; i < sizeof(weigherConveyorSensitivity); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &primaryPrinterConveyorSensitivity;
	for (unsigned int i = 0; i < sizeof(primaryPrinterConveyorSensitivity); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secondaryPrinterConveyorSensitivity;
	for (unsigned int i = 0; i < sizeof(secondaryPrinterConveyorSensitivity); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &primaryPrinterMaxJustificationDistance;
	for (unsigned int i = 0; i < sizeof(primaryPrinterMaxJustificationDistance); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secondaryPrinterMaxJustificationDistance;
	for (unsigned int i = 0; i < sizeof(secondaryPrinterMaxJustificationDistance); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &diagonalLabelPlacement;
	for (unsigned int i = 0; i < sizeof(diagonalLabelPlacement); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &diagonalOffset;
	for (unsigned int i = 0; i < sizeof(diagonalOffset); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &miniStationLabelPlacement;
	for (unsigned int i = 0; i < sizeof(miniStationLabelPlacement); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &deviceSignature;
	for (unsigned int i = 0; i < sizeof(deviceSignature); i++)
	{
		serializedData.append(1, pChar[i]);
	}
*/
	return serializedData.data();
}

void CompactLabelApplierConfiguration::show()
{
#ifndef EVOL_NDEBUG
	bool isTracing = Debug::isTracing("CompactLabelApplierConfiguration");

	if (isTracing == false)
	{
		return;
//		Debug::addTraceCategory( std::string("CompactLabelApplierConfiguration") );
	}

	Debug::trace("CompactLabelApplierConfiguration", "CompactLabelApplierConfiguration: ");

	EvolMessage::show();

	if (configType == LaPermanent)
	{
		Debug::trace("CompactLabelApplierConfiguration", "configType: LaPermanent");
	}
	else if (configType == LaTemporary)
	{
		Debug::trace("CompactLabelApplierConfiguration", "configType: LaTemporary");
	}
	else if (configType == LaDefault)
	{
		Debug::trace("CompactLabelApplierConfiguration", "configType: LaDefault");
	}
	else
	{
		Debug::trace("CompactLabelApplierConfiguration", HobString("configType: Unknown ") +
												  HobString(Integer::toString((UINT16) configType)));
	}

	Debug::trace("CompactLabelApplierConfiguration", HobString("weigherAttached: ") +
											  HobString(Bool::toString(weigherAttached)));
	Debug::trace("CompactLabelApplierConfiguration", HobString("primaryPrinterAttached: ") +
											  HobString(Bool::toString(primaryPrinterAttached)));
	Debug::trace("CompactLabelApplierConfiguration", HobString("secondaryPrinterAttached: ") +
											  HobString(Bool::toString(secondaryPrinterAttached)));
	Debug::trace("CompactLabelApplierConfiguration", HobString("leftToRight: ") +
											  HobString(Bool::toString(leftToRight)));
	Debug::trace("CompactLabelApplierConfiguration", HobString("frontDischarge: ") +
											  HobString(Bool::toString(frontDischarge)));
/*	Debug::trace("CompactLabelApplierConfiguration", HobString("primaryJustifyFont: ") +
											  HobString(Bool::toString(primaryJustifyFont)));
	Debug::trace("CompactLabelApplierConfiguration", HobString("secondaryJustifyFont: ") +
											  HobString(Bool::toString(secondaryJustifyFont)));

	if (transferStationType == NSW)
	{
		Debug::trace("CompactLabelApplierConfiguration", "transferStationType: NSW");
	}
	else if (transferStationType == HandWrapper)
	{
		Debug::trace("CompactLabelApplierConfiguration", "transferStationType: HandWrapper");
	}
	else if (transferStationType == Bakery)
	{
		Debug::trace("CompactLabelApplierConfiguration", "transferStationType: Bakery");
	}
	else if (transferStationType == ESW)
	{
		Debug::trace("CompactLabelApplierConfiguration", "transferStationType: ESW");
	}
	else if (transferStationType == Mini)
	{
		Debug::trace("CompactLabelApplierConfiguration", "transferStationType: Mini");
	}
	else
	{
		Debug::trace("CompactLabelApplierConfiguration", HobString("transferStationType: Unknown ") +
												  HobString(Integer::toString((UINT16) transferStationType)));
	}
*/
	Debug::trace("CompactLabelApplierConfiguration", HobString("primaryPrinterLabelOffset: ") +
											  HobString(Integer::toString((UINT16) primaryPrinterLabelOffset)));
	Debug::trace("CompactLabelApplierConfiguration", HobString("secondaryPrinterLabelOffset: ") +
											  HobString(Integer::toString((UINT16) secondaryPrinterLabelOffset)));
	//Alex Zavala 8/20
	Debug::trace("CompactLabelApplierConfiguration", HobString("labelPlacement: ") +
											  HobString(Integer::toString((UINT16) labelPlacement)));
	//Alex Zavala 11/7
	Debug::trace("CompactLabelApplierConfiguration", HobString("overdrive: ") +
											  HobString(Integer::toString((UINT16) overdrive)));
/*
	if (weigherConveyorSensitivity == ConveyorSensitivityLow)
	{
		Debug::trace("CompactLabelApplierConfiguration", "weigherConveyorSensitivity: ConveyorSensitivityLow");
	}
	else if (weigherConveyorSensitivity == ConveyorSensitivityMedium)
	{
		Debug::trace("CompactLabelApplierConfiguration", "weigherConveyorSensitivity: ConveyorSensitivityMedium");
	}
	else if (weigherConveyorSensitivity == ConveyorSensitivityHigh)
	{
		Debug::trace("CompactLabelApplierConfiguration", "weigherConveyorSensitivity: ConveyorSensitivityHigh");
	}
	else
	{
		Debug::trace("CompactLabelApplierConfiguration", HobString("weigherConveyorSensitivity: Unknown ") +
												  HobString(Integer::toString((UINT16) weigherConveyorSensitivity)));
	}

	if (primaryPrinterConveyorSensitivity == ConveyorSensitivityLow)
	{
		Debug::trace("CompactLabelApplierConfiguration", "primaryPrinterConveyorSensitivity: ConveyorSensitivityLow");
	}
	else if (primaryPrinterConveyorSensitivity == ConveyorSensitivityMedium)
	{
		Debug::trace("CompactLabelApplierConfiguration", "primaryPrinterConveyorSensitivity: ConveyorSensitivityMedium");
	}
	else if (primaryPrinterConveyorSensitivity == ConveyorSensitivityHigh)
	{
		Debug::trace("CompactLabelApplierConfiguration", "primaryPrinterConveyorSensitivity: ConveyorSensitivityHigh");
	}
	else
	{
		Debug::trace("CompactLabelApplierConfiguration", HobString("primaryPrinterConveyorSensitivity: Unknown ") +
												  HobString(Integer::toString((UINT16) primaryPrinterConveyorSensitivity)));
	}

	if (secondaryPrinterConveyorSensitivity == ConveyorSensitivityLow)
	{
		Debug::trace("CompactLabelApplierConfiguration", "secondaryPrinterConveyorSensitivity: ConveyorSensitivityLow");
	}
	else if (secondaryPrinterConveyorSensitivity == ConveyorSensitivityMedium)
	{
		Debug::trace("CompactLabelApplierConfiguration", "secondaryPrinterConveyorSensitivity: ConveyorSensitivityMedium");
	}
	else if (secondaryPrinterConveyorSensitivity == ConveyorSensitivityHigh)
	{
		Debug::trace("CompactLabelApplierConfiguration", "secondaryPrinterConveyorSensitivity: ConveyorSensitivityHigh");
	}
	else
	{
		Debug::trace("CompactLabelApplierConfiguration", HobString("secondaryPrinterConveyorSensitivity: Unknown ") +
												  HobString(Integer::toString((UINT16) secondaryPrinterConveyorSensitivity)));
	}

	Debug::trace("CompactLabelApplierConfiguration", HobString("primaryPrinterMaxJustificationDistance: ") +
											  HobString(Integer::toString((UINT16) primaryPrinterMaxJustificationDistance)));
	Debug::trace("CompactLabelApplierConfiguration", HobString("secondaryPrinterMaxJustificationDistance: ") +
											  HobString(Integer::toString((UINT16) secondaryPrinterMaxJustificationDistance)));

	Debug::trace("CompactLabelApplierConfiguration", HobString("diagonalLabelPlacement: ") +
											  HobString(Bool::toString(diagonalLabelPlacement)));
	Debug::trace("CompactLabelApplierConfiguration", HobString("diagonalOffset: ") +
											  HobString(Integer::toString((UINT16) diagonalOffset)));
	Debug::trace("CompactLabelApplierConfiguration", HobString("miniStationLabelPlacement: ") +
											  HobString(Integer::toString((UINT16) miniStationLabelPlacement)));

	if (miniStationLabelPlacement == LaLeadingEdge)
	{
		Debug::trace("CompactLabelApplierConfiguration", "miniStationLabelPlacement: LaLeadingEdge");
	}
	else if (miniStationLabelPlacement == LaTrailingEdge)
	{
		Debug::trace("CompactLabelApplierConfiguration", "miniStationLabelPlacement: LaTrailingEdge");
	}
	else
	{
		Debug::trace("CompactLabelApplierConfiguration", "miniStationLabelPlacement: LaNoneSelected");
	}

	Debug::trace("CompactLabelApplierConfiguration", "deviceSignature: ");
	Debug::trace("CompactLabelApplierConfiguration", Hex::toString((const char *) &deviceSignature[0], SystemDevSignatureSize));
*/
	if (isTracing == false)
	{
		Debug::removeTraceCategory( std::string("CompactLabelApplierConfiguration") );
	}
#endif
}

void CompactLabelApplierConfiguration::clear()
{
	Debug::trace("CompactLabelApplierConfiguration", "clear()");
	configType = LaPermanent;

	weigherAttached = false;
	primaryPrinterAttached = false;
	secondaryPrinterAttached = false;
	leftToRight = false;	// package direction
	frontDischarge = false;
/*	primaryJustifyFont = 0;
	secondaryJustifyFont = 0;
	transferStationType = ESW;
	Debug::trace("CompactLabelApplierConfiguration", "clear():  Set transferStationType to ESW");
*/	primaryPrinterLabelOffset = 0;
	secondaryPrinterLabelOffset = 0;
	//Alex Zavala 8/20
	labelPlacement = 0;
	//Alex Zavala 11/7
	overdrive = 0;
/*	weigherConveyorSensitivity = ConveyorSensitivityMedium;
	primaryPrinterConveyorSensitivity = ConveyorSensitivityHigh;
	secondaryPrinterConveyorSensitivity = ConveyorSensitivityHigh;

	primaryPrinterMaxJustificationDistance = 0;
	secondaryPrinterMaxJustificationDistance = 0;
	diagonalLabelPlacement = 0;
	diagonalOffset = 0;
	miniStationLabelPlacement = LaNoneSelected;

	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		deviceSignature[i] = '\0';
	}
*/
}

void CompactLabelApplierConfiguration::setConfigType(LaConfigType configType)
{
	this->configType = configType;
}

LaConfigType CompactLabelApplierConfiguration::getConfigType()
{
	return configType;
}

bool CompactLabelApplierConfiguration::isWeigherStationAttached() const
{
	return weigherAttached;
}
void CompactLabelApplierConfiguration::setWeigherStationAttached(bool value)
{
#ifndef EVOL_NDEBUG
	Debug::trace("CompactLabelApplierConfiguration", "setWeigherStationAttached(" + util_string::booltostring(value) + ")");
#endif
	weigherAttached = value;
}

void CompactLabelApplierConfiguration::clearWeigherStationAttached()
{
	weigherAttached = false;
}

bool CompactLabelApplierConfiguration::isPrimaryStationAttached() const
{
	return primaryPrinterAttached;
}
void CompactLabelApplierConfiguration::setPrimaryStationAttached(bool /* value */)
{
#ifndef EVOL_NDEBUG
	Debug::trace("CompactLabelApplierConfiguration", "setPrimaryStationAttached(true)");
#endif
	primaryPrinterAttached = true;
}

void CompactLabelApplierConfiguration::clearPrimaryStationAttached()
{
#ifndef EVOL_NDEBUG
	Debug::trace("CompactLabelApplierConfiguration", "clearPrimaryStationAttached(false)");
#endif
	primaryPrinterAttached = false;
}

bool CompactLabelApplierConfiguration::isSecondaryStationAttached() const
{
	return secondaryPrinterAttached;
}

void CompactLabelApplierConfiguration::setSecondaryStationAttached(bool value)
{
	secondaryPrinterAttached = value;
}

void CompactLabelApplierConfiguration::clearSecondaryStationAttached()
{
	secondaryPrinterAttached = false;
}

bool CompactLabelApplierConfiguration::isLeftToRight() const
{
	return leftToRight;
}

void CompactLabelApplierConfiguration::setLeftToRight(bool value)
{
	leftToRight = value;
}

void CompactLabelApplierConfiguration::clearLeftToRight()
{
	leftToRight = false;
}

bool CompactLabelApplierConfiguration::isFrontDischarge() const
{
	return frontDischarge;
}

void CompactLabelApplierConfiguration::setFrontDischarge(bool value)
{
	frontDischarge = value;
}

void CompactLabelApplierConfiguration::clearFrontDischarge()
{
	frontDischarge = false;
}
/*
bool CompactLabelApplierConfiguration::isPrimaryJustifyFont() const
{
	return primaryJustifyFont;
}

void CompactLabelApplierConfiguration::setPrimaryJustifyFont(bool value)
{
	primaryJustifyFont = value;
}

void CompactLabelApplierConfiguration::clearPrimaryJustifyFont()
{
	primaryJustifyFont = false;
}

bool CompactLabelApplierConfiguration::isSecondaryJustifyFont() const
{
	return secondaryJustifyFont;
}

void CompactLabelApplierConfiguration::setSecondaryJustifyFont(bool value)
{
	secondaryJustifyFont = value;
}

void CompactLabelApplierConfiguration::clearSecondaryJustifyFont()
{
	secondaryJustifyFont = false;
}

void CompactLabelApplierConfiguration::setTransferStationType(TransferStationType transferStationType)
{
	this->transferStationType = transferStationType;
	if (this->transferStationType == NSW)
	{
		Debug::trace("CompactLabelApplierConfiguration", "setTransferStationType to NSW");
	}
	else if (this->transferStationType == HandWrapper)
	{
		Debug::trace("CompactLabelApplierConfiguration", "setTransferStationType to HandWrapper");
	}
	else if (this->transferStationType == Bakery)
	{
		Debug::trace("CompactLabelApplierConfiguration", "setTransferStationType to Bakery");
	}
	else if (this->transferStationType == ESW)
	{
		Debug::trace("CompactLabelApplierConfiguration", "setTransferStationType to ESW");
	}
	else if (this->transferStationType == Mini)
	{
		Debug::trace("CompactLabelApplierConfiguration", "setTransferStationType to Mini");
	}
	else
	{
		Debug::trace("CompactLabelApplierConfiguration", HobString("setTransferStationType to Unknown, ") +
												  HobString(Integer::toString((UINT16) transferStationType)));
	}
}

TransferStationType CompactLabelApplierConfiguration::getTransferStationType() const
{
	return transferStationType;
}
*/
void CompactLabelApplierConfiguration::setPrimaryPrinterLabelOffset(UINT8 primaryPrinterLabelOffset)
{
	this->primaryPrinterLabelOffset = primaryPrinterLabelOffset;
}

UINT8 CompactLabelApplierConfiguration::getPrimaryPrinterLabelOffset() const
{
	return primaryPrinterLabelOffset;
}

void CompactLabelApplierConfiguration::setSecondaryPrinterLabelOffset(UINT8 secondaryPrinterLabelOffset)
{
	this->secondaryPrinterLabelOffset = secondaryPrinterLabelOffset;
}

UINT8 CompactLabelApplierConfiguration::getSecondaryPrinterLabelOffset() const
{
	return secondaryPrinterLabelOffset;
}

//Alex Zavala 8/20
void CompactLabelApplierConfiguration::setLabelPlacement(UINT8 labelPlacement)
{
	this->labelPlacement = labelPlacement;
}

UINT8 CompactLabelApplierConfiguration::getLabelPlacement() const
{
	return labelPlacement;
}

//Alex Zavala 11/7
void CompactLabelApplierConfiguration::setOverdrive(UINT8 overdrive)
{
	this->overdrive = overdrive;
}

UINT8 CompactLabelApplierConfiguration::getOverdrive() const
{
	return overdrive;
}
/*
void CompactLabelApplierConfiguration::setWeigherConveyorSensitivity(ConveyorSensitivity weigherConveyorSensitivity)
{
	this->weigherConveyorSensitivity = weigherConveyorSensitivity;
}

ConveyorSensitivity CompactLabelApplierConfiguration::getWeigherConveyorSensitivity() const
{
	return weigherConveyorSensitivity;
}

void CompactLabelApplierConfiguration::setPrimaryPrinterConveyorSensitivity(ConveyorSensitivity primaryPrinterConveyorSensitivity)
{
	this->primaryPrinterConveyorSensitivity = primaryPrinterConveyorSensitivity;
}

ConveyorSensitivity CompactLabelApplierConfiguration::getPrimaryPrinterConveyorSensitivity() const
{
	return primaryPrinterConveyorSensitivity;
}

void CompactLabelApplierConfiguration::setSecondaryPrinterConveyorSensitivity(ConveyorSensitivity secondaryPrinterConveyorSensitivity)
{
	this->secondaryPrinterConveyorSensitivity = secondaryPrinterConveyorSensitivity;
}

ConveyorSensitivity CompactLabelApplierConfiguration::getSecondaryPrinterConveyorSensitivity() const
{
	return secondaryPrinterConveyorSensitivity;
}

void CompactLabelApplierConfiguration::setPrimaryPrinterMaxJustificationDistance(UINT8 primaryPrinterMaxJustificationDistance)
{
	this->primaryPrinterMaxJustificationDistance = primaryPrinterMaxJustificationDistance;
}

UINT8 CompactLabelApplierConfiguration::getPrimaryPrinterMaxJustificationDistance() const
{
	return primaryPrinterMaxJustificationDistance;
}

void CompactLabelApplierConfiguration::setSecondaryPrinterMaxJustificationDistance(UINT8 secondaryPrinterMaxJustificationDistance)
{
	this->secondaryPrinterMaxJustificationDistance = secondaryPrinterMaxJustificationDistance;
}

UINT8 CompactLabelApplierConfiguration::getSecondaryPrinterMaxJustificationDistance() const
{
	return secondaryPrinterMaxJustificationDistance;
}

bool CompactLabelApplierConfiguration::isDiagonalLabelPlacement() const
{
	return diagonalLabelPlacement;
}

void CompactLabelApplierConfiguration::setDiagonalLabelPlacement(bool value)
{
	diagonalLabelPlacement = value;
}

void CompactLabelApplierConfiguration::clearDiagonalLabelPlacement()
{
	diagonalLabelPlacement = false;
}

void CompactLabelApplierConfiguration::setDiagonalOffset(UINT8 diagonalOffset)
{
	this->diagonalOffset = diagonalOffset;
}

UINT8 CompactLabelApplierConfiguration::getDiagonalOffset() const
{
	return diagonalOffset;
}

void CompactLabelApplierConfiguration::setMiniStationLabelPlacement(LaPlacement miniStationLabelPlacement)
{
	this->miniStationLabelPlacement = miniStationLabelPlacement;
}

LaPlacement CompactLabelApplierConfiguration::getMiniStationLabelPlacement() const
{
	Debug::trace("CompactLabelApplierConfiguration", "getMiniStationLabelPlacement() = " + HobString(Integer::toString((UINT16)miniStationLabelPlacement)));
	return miniStationLabelPlacement;
}

void CompactLabelApplierConfiguration::setDeviceSignature(UINT8 *deviceSignature)
{
	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		this->deviceSignature[i] = deviceSignature[i];
	}
}

void CompactLabelApplierConfiguration::getDeviceSignature(UINT8 *deviceSignature) const
{
	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		deviceSignature[i] = this->deviceSignature[i];
	}
}
*/
bool CompactLabelApplierConfiguration::update(const CompactLabelApplierConfiguration &laConfig)
{
	bool rv = false;

	if (weigherAttached != laConfig.isWeigherStationAttached())
	{
		weigherAttached = laConfig.isWeigherStationAttached();
		rv = true;
	}

	if (primaryPrinterAttached != laConfig.isPrimaryStationAttached())
	{
		primaryPrinterAttached = laConfig.isPrimaryStationAttached();
		rv = true;
	}

	if (secondaryPrinterAttached != laConfig.isSecondaryStationAttached())
	{
		secondaryPrinterAttached = laConfig.isSecondaryStationAttached();
		rv = true;
	}

	if (leftToRight != laConfig.isLeftToRight())
	{
		leftToRight = laConfig.isLeftToRight();
		rv = true;
	}

	if (frontDischarge != laConfig.isFrontDischarge())
	{
		frontDischarge = laConfig.isFrontDischarge();
		rv = true;
	}
/*
	if (transferStationType != laConfig.getTransferStationType())
	{
		transferStationType = laConfig.getTransferStationType();
		rv = true;
	}
*/
	if (primaryPrinterLabelOffset != laConfig.getPrimaryPrinterLabelOffset())
	{
		primaryPrinterLabelOffset = laConfig.getPrimaryPrinterLabelOffset();
		rv = true;
	}

	if (secondaryPrinterLabelOffset != laConfig.getSecondaryPrinterLabelOffset())
	{
		secondaryPrinterLabelOffset = laConfig.getSecondaryPrinterLabelOffset();
		rv = true;
	}

	//Alex Zavala 8/20
	if (labelPlacement != laConfig.getLabelPlacement())
	{
		labelPlacement = laConfig.getLabelPlacement();
		rv = true;
	}

	//Alex Zavala 11/7
	if (overdrive != laConfig.getOverdrive())
	{
		overdrive = laConfig.getOverdrive();
		rv = true;
	}
/*
	if (weigherConveyorSensitivity != laConfig.getWeigherConveyorSensitivity())
	{
		weigherConveyorSensitivity = laConfig.getWeigherConveyorSensitivity();
		rv = true;
	}

	if (primaryPrinterConveyorSensitivity != laConfig.getPrimaryPrinterConveyorSensitivity())
	{
		primaryPrinterConveyorSensitivity = laConfig.getPrimaryPrinterConveyorSensitivity();
		rv = true;
	}

	if (secondaryPrinterConveyorSensitivity != laConfig.getSecondaryPrinterConveyorSensitivity())
	{
		secondaryPrinterConveyorSensitivity = laConfig.getSecondaryPrinterConveyorSensitivity();
		rv = true;
	}

	if (primaryPrinterMaxJustificationDistance != laConfig.getPrimaryPrinterMaxJustificationDistance())
	{
		primaryPrinterMaxJustificationDistance = laConfig.getPrimaryPrinterMaxJustificationDistance();
		rv = true;
	}

	if (miniStationLabelPlacement != laConfig.getMiniStationLabelPlacement())
	{
		miniStationLabelPlacement = laConfig.getMiniStationLabelPlacement();
		rv = true;
	}
*/
	return rv;
}

bool CompactLabelApplierConfiguration::equals(const CompactLabelApplierConfiguration &laConfig)
{
	bool rv = false;

	if ((weigherAttached == laConfig.isWeigherStationAttached()) &&
		(primaryPrinterAttached == laConfig.isPrimaryStationAttached()) &&
		(secondaryPrinterAttached == laConfig.isSecondaryStationAttached()) &&
		(leftToRight == laConfig.isLeftToRight()) &&
		(frontDischarge == laConfig.isFrontDischarge()) &&
		/*(transferStationType == laConfig.getTransferStationType()) &&*/
		(primaryPrinterLabelOffset == laConfig.getPrimaryPrinterLabelOffset()) &&
		(secondaryPrinterLabelOffset == laConfig.getSecondaryPrinterLabelOffset()) &&
		(labelPlacement == laConfig.getLabelPlacement()) &&//Alex Zavala 8/20
		(overdrive == laConfig.getOverdrive())//Alex Zavala 11/7
		/*(weigherConveyorSensitivity == laConfig.getWeigherConveyorSensitivity()) &&
		(primaryPrinterConveyorSensitivity == laConfig.getPrimaryPrinterConveyorSensitivity()) &&
		(secondaryPrinterConveyorSensitivity == laConfig.getSecondaryPrinterConveyorSensitivity()) &&
		(primaryPrinterMaxJustificationDistance == laConfig.getPrimaryPrinterMaxJustificationDistance()) &&
		(miniStationLabelPlacement == laConfig.getMiniStationLabelPlacement())*/)
	{
		rv = true;
	}

	return rv;
}

bool CompactLabelApplierConfiguration::update(const CompactLabelApplierInquire &laInquire)
{
	bool rv = false;

	if (weigherAttached != laInquire.isWeigherStationAttached())
	{
		weigherAttached = laInquire.isWeigherStationAttached();
		rv = true;
	}

	if (primaryPrinterAttached != laInquire.isPrimaryStationAttached())
	{
		primaryPrinterAttached = laInquire.isPrimaryStationAttached();
		rv = true;
	}

	if (secondaryPrinterAttached != laInquire.isSecondaryStationAttached())
	{
		secondaryPrinterAttached = laInquire.isSecondaryStationAttached();
		rv = true;
	}

	if (leftToRight != laInquire.isLeftToRight())
	{
		leftToRight = laInquire.isLeftToRight();
		rv = true;
	}

	if (frontDischarge != laInquire.isFrontDischarge())
	{
		frontDischarge = laInquire.isFrontDischarge();
		rv = true;
	}
/*
	if (transferStationType != laInquire.getTransferStationType())
	{
		transferStationType = laInquire.getTransferStationType();
		rv = true;
	}
*/
	if (primaryPrinterLabelOffset != laInquire.getPrimaryPrinterLabelOffset())
	{
		primaryPrinterLabelOffset = laInquire.getPrimaryPrinterLabelOffset();
		rv = true;
	}

	if (secondaryPrinterLabelOffset != laInquire.getSecondaryPrinterLabelOffset())
	{
		secondaryPrinterLabelOffset = laInquire.getSecondaryPrinterLabelOffset();
		rv = true;
	}

	//Alex Zavala 8/20
	if(labelPlacement != laInquire.getLabelPlacement())
	{
		labelPlacement = laInquire.getLabelPlacement();
		rv = true;
	}

	//Alex Zavala 11/7
	if(overdrive != laInquire.getOverdrive())
	{
		overdrive = laInquire.getOverdrive();
		rv = true;
	}
/*
	if (weigherConveyorSensitivity != laInquire.getWeigherConveyorSensitivity())
	{
		weigherConveyorSensitivity = laInquire.getWeigherConveyorSensitivity();
		rv = true;
	}

	if (primaryPrinterConveyorSensitivity != laInquire.getPrimaryPrinterConveyorSensitivity())
	{
		primaryPrinterConveyorSensitivity = laInquire.getPrimaryPrinterConveyorSensitivity();
		rv = true;
	}

	if (secondaryPrinterConveyorSensitivity != laInquire.getSecondaryPrinterConveyorSensitivity())
	{
		secondaryPrinterConveyorSensitivity = laInquire.getSecondaryPrinterConveyorSensitivity();
		rv = true;
	}

	if (primaryPrinterMaxJustificationDistance != laInquire.getPrimaryPrinterMaxJustificationDistance())
	{
		primaryPrinterMaxJustificationDistance = laInquire.getPrimaryPrinterMaxJustificationDistance();
		rv = true;
	}

	if (miniStationLabelPlacement != laInquire.getMiniStationLabelPlacement())
	{
		miniStationLabelPlacement = laInquire.getMiniStationLabelPlacement();
		rv = true;
	}
*/
	return rv;
}

bool CompactLabelApplierConfiguration::equals(const CompactLabelApplierInquire &laInquire)
{
	bool rv = false;

	if ((weigherAttached == laInquire.isWeigherStationAttached()) &&
		(primaryPrinterAttached == laInquire.isPrimaryStationAttached()) &&
		(secondaryPrinterAttached == laInquire.isSecondaryStationAttached()) &&
		(leftToRight == laInquire.isLeftToRight()) &&
		(frontDischarge == laInquire.isFrontDischarge()) &&
		/*(transferStationType == laInquire.getTransferStationType()) &&*/
		(primaryPrinterLabelOffset == laInquire.getPrimaryPrinterLabelOffset()) &&
		(secondaryPrinterLabelOffset == laInquire.getSecondaryPrinterLabelOffset()) &&
		(labelPlacement == laInquire.getLabelPlacement()) &&
		(overdrive == laInquire.getOverdrive())//Alex Zavala 11/7
/*		(weigherConveyorSensitivity == laInquire.getWeigherConveyorSensitivity()) &&
		(primaryPrinterConveyorSensitivity == laInquire.getPrimaryPrinterConveyorSensitivity()) &&
		(secondaryPrinterConveyorSensitivity == laInquire.getSecondaryPrinterConveyorSensitivity()) &&
		(primaryPrinterMaxJustificationDistance == laInquire.getPrimaryPrinterMaxJustificationDistance()) &&
		(miniStationLabelPlacement == laInquire.getMiniStationLabelPlacement())*/)
	{
		rv = true;
	}

	return rv;
}
