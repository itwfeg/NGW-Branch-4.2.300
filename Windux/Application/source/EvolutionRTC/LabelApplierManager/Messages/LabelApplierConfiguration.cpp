#include "LabelApplierConfiguration.h"
#include "LabelApplierInquire.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierConfiguration::dataType = 0;
std::string LabelApplierConfiguration::dataTypeName("LabelApplierConfiguration"); 
#endif

LabelApplierConfiguration::LabelApplierConfiguration(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(LabelApplier, LaConfig, pHobThread)
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
	primaryJustifyFont = true;
	secondaryJustifyFont = true;
	transferStationType = Bakery;
	Debug::trace("LabelApplierConfiguration", "Default Constructor:  Initially setting transferStationType to Bakery");
	primaryPrinterLabelOffset = DEFAULT_CLA_LABEL_OFFSET;
	secondaryPrinterLabelOffset = DEFAULT_CLA_LABEL_OFFSET;
	weigherConveyorSensitivity = ConveyorSensitivityMedium;
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
}

LabelApplierConfiguration::LabelApplierConfiguration(const char *buffer) : EvolMessage(buffer)
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
		Debug::trace("LabelApplierConfiguration", "Constructor: transferStationType = NSW");
	}
	else if (transferStationType == HandWrapper)
	{
		Debug::trace("LabelApplierConfiguration", "Constructor: transferStationType = HandWrapper");
	}
	else if (transferStationType == Bakery)
	{
		Debug::trace("LabelApplierConfiguration", "Constructor: transferStationType = Bakery");
	}
	else if (transferStationType == ESW)
	{
		Debug::trace("LabelApplierConfiguration", "Constructor: transferStationType = ESW");
	}
	else if (transferStationType == Mini)
	{
		Debug::trace("LabelApplierConfiguration", "Constructor: transferStationType = Mini");
	}
	else
	{
		Debug::trace("LabelApplierConfiguration", "Constructor: transferStationType = Unknown, " +
												  HobString(Integer::toString((UINT16) transferStationType)));
	}
	
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
		Debug::trace("LabelApplierConfiguration", "Constructor: miniStationLabelPlacement = LaLeadingEdge");
	}
	else if (miniStationLabelPlacement == LaTrailingEdge)
	{
		Debug::trace("LabelApplierConfiguration", "Constructor: miniStationLabelPlacement = LaTrailingEdge");
	}
	else
	{
		Debug::trace("LabelApplierConfiguration", "Constructor: miniStationLabelPlacement = LaNoneSelected");
	}

	pChar = (char *) &deviceSignature;
	for (unsigned int i = 0; i < sizeof(deviceSignature); i++)
	{
		*pChar++ = buffer[j++];
	}
}

LabelApplierConfiguration::~LabelApplierConfiguration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *LabelApplierConfiguration::serialize() const
{
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

	return serializedData.data();
}

void LabelApplierConfiguration::show()
{
	bool isTracing = Debug::isTracing("LabelApplierConfiguration");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("LabelApplierConfiguration") );
#else
		Debug::addTraceCategory("LabelApplierConfiguration");
#endif
	}

	Debug::trace("LabelApplierConfiguration", "LabelApplierConfiguration: ");

	EvolMessage::show();

	if (configType == LaPermanent)
	{
		Debug::trace("LabelApplierConfiguration", "configType: LaPermanent");
	}
	else if (configType == LaTemporary)
	{
		Debug::trace("LabelApplierConfiguration", "configType: LaTemporary");
	}
	else if (configType == LaDefault)
	{
		Debug::trace("LabelApplierConfiguration", "configType: LaDefault");
	}
	else
	{
		Debug::trace("LabelApplierConfiguration", HobString("configType: Unknown ") +
												  HobString(Integer::toString((UINT16) configType)));
	}

	Debug::trace("LabelApplierConfiguration", HobString("weigherAttached: ") +
											  HobString(Bool::toString(weigherAttached)));
	Debug::trace("LabelApplierConfiguration", HobString("primaryPrinterAttached: ") +
											  HobString(Bool::toString(primaryPrinterAttached)));
	Debug::trace("LabelApplierConfiguration", HobString("secondaryPrinterAttached: ") +
											  HobString(Bool::toString(secondaryPrinterAttached)));
	Debug::trace("LabelApplierConfiguration", HobString("leftToRight: ") +
											  HobString(Bool::toString(leftToRight)));
	Debug::trace("LabelApplierConfiguration", HobString("frontDischarge: ") +
											  HobString(Bool::toString(frontDischarge)));
	Debug::trace("LabelApplierConfiguration", HobString("primaryJustifyFont: ") +
											  HobString(Bool::toString(primaryJustifyFont)));
	Debug::trace("LabelApplierConfiguration", HobString("secondaryJustifyFont: ") +
											  HobString(Bool::toString(secondaryJustifyFont)));

	if (transferStationType == NSW)
	{
		Debug::trace("LabelApplierConfiguration", "transferStationType: NSW");
	}
	else if (transferStationType == HandWrapper)
	{
		Debug::trace("LabelApplierConfiguration", "transferStationType: HandWrapper");
	}
	else if (transferStationType == Bakery)
	{
		Debug::trace("LabelApplierConfiguration", "transferStationType: Bakery");
	}
	else if (transferStationType == ESW)
	{
		Debug::trace("LabelApplierConfiguration", "transferStationType: ESW");
	}
	else if (transferStationType == Mini)
	{
		Debug::trace("LabelApplierConfiguration", "transferStationType: Mini");
	}
	else
	{
		Debug::trace("LabelApplierConfiguration", HobString("transferStationType: Unknown ") +
												  HobString(Integer::toString((UINT16) transferStationType)));
	}

	Debug::trace("LabelApplierConfiguration", HobString("primaryPrinterLabelOffset: ") +
											  HobString(Integer::toString((UINT16) primaryPrinterLabelOffset)));
	Debug::trace("LabelApplierConfiguration", HobString("secondaryPrinterLabelOffset: ") +
											  HobString(Integer::toString((UINT16) secondaryPrinterLabelOffset)));


	if (weigherConveyorSensitivity == ConveyorSensitivityLow)
	{
		Debug::trace("LabelApplierConfiguration", "weigherConveyorSensitivity: ConveyorSensitivityLow");
	}
	else if (weigherConveyorSensitivity == ConveyorSensitivityMedium)
	{
		Debug::trace("LabelApplierConfiguration", "weigherConveyorSensitivity: ConveyorSensitivityMedium");
	}
	else if (weigherConveyorSensitivity == ConveyorSensitivityHigh)
	{
		Debug::trace("LabelApplierConfiguration", "weigherConveyorSensitivity: ConveyorSensitivityHigh");
	}
	else
	{
		Debug::trace("LabelApplierConfiguration", HobString("weigherConveyorSensitivity: Unknown ") +
												  HobString(Integer::toString((UINT16) weigherConveyorSensitivity)));
	}

	if (primaryPrinterConveyorSensitivity == ConveyorSensitivityLow)
	{
		Debug::trace("LabelApplierConfiguration", "primaryPrinterConveyorSensitivity: ConveyorSensitivityLow");
	}
	else if (primaryPrinterConveyorSensitivity == ConveyorSensitivityMedium)
	{
		Debug::trace("LabelApplierConfiguration", "primaryPrinterConveyorSensitivity: ConveyorSensitivityMedium");
	}
	else if (primaryPrinterConveyorSensitivity == ConveyorSensitivityHigh)
	{
		Debug::trace("LabelApplierConfiguration", "primaryPrinterConveyorSensitivity: ConveyorSensitivityHigh");
	}
	else
	{
		Debug::trace("LabelApplierConfiguration", HobString("primaryPrinterConveyorSensitivity: Unknown ") +
												  HobString(Integer::toString((UINT16) primaryPrinterConveyorSensitivity)));
	}

	if (secondaryPrinterConveyorSensitivity == ConveyorSensitivityLow)
	{
		Debug::trace("LabelApplierConfiguration", "secondaryPrinterConveyorSensitivity: ConveyorSensitivityLow");
	}
	else if (secondaryPrinterConveyorSensitivity == ConveyorSensitivityMedium)
	{
		Debug::trace("LabelApplierConfiguration", "secondaryPrinterConveyorSensitivity: ConveyorSensitivityMedium");
	}
	else if (secondaryPrinterConveyorSensitivity == ConveyorSensitivityHigh)
	{
		Debug::trace("LabelApplierConfiguration", "secondaryPrinterConveyorSensitivity: ConveyorSensitivityHigh");
	}
	else
	{
		Debug::trace("LabelApplierConfiguration", HobString("secondaryPrinterConveyorSensitivity: Unknown ") +
												  HobString(Integer::toString((UINT16) secondaryPrinterConveyorSensitivity)));
	}

	Debug::trace("LabelApplierConfiguration", HobString("primaryPrinterMaxJustificationDistance: ") +
											  HobString(Integer::toString((UINT16) primaryPrinterMaxJustificationDistance)));
	Debug::trace("LabelApplierConfiguration", HobString("secondaryPrinterMaxJustificationDistance: ") +
											  HobString(Integer::toString((UINT16) secondaryPrinterMaxJustificationDistance)));

	Debug::trace("LabelApplierConfiguration", HobString("diagonalLabelPlacement: ") +
											  HobString(Bool::toString(diagonalLabelPlacement)));
	Debug::trace("LabelApplierConfiguration", HobString("diagonalOffset: ") +
											  HobString(Integer::toString((UINT16) diagonalOffset)));
	Debug::trace("LabelApplierConfiguration", HobString("miniStationLabelPlacement: ") +
											  HobString(Integer::toString((UINT16) miniStationLabelPlacement)));

	if (miniStationLabelPlacement == LaLeadingEdge)
	{
		Debug::trace("LabelApplierConfiguration", "miniStationLabelPlacement: LaLeadingEdge");
	}
	else if (miniStationLabelPlacement == LaTrailingEdge)
	{
		Debug::trace("LabelApplierConfiguration", "miniStationLabelPlacement: LaTrailingEdge");
	}
	else
	{
		Debug::trace("LabelApplierConfiguration", "miniStationLabelPlacement: LaNoneSelected");
	}

	Debug::trace("LabelApplierConfiguration", "deviceSignature: ");
	Debug::trace("LabelApplierConfiguration", Hex::toString((const char *) &deviceSignature[0], SystemDevSignatureSize));

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("LabelApplierConfiguration") );
#else
		Debug::removeTraceCategory("LabelApplierConfiguration");
#endif
	}
}

void LabelApplierConfiguration::clear()
{
	Debug::trace("LabelApplierConfiguration", "clear()");
	configType = LaPermanent;

	weigherAttached = false;
	primaryPrinterAttached = false;
	secondaryPrinterAttached = false;
	leftToRight = false;	// package direction
	frontDischarge = false;
	primaryJustifyFont = 0;
	secondaryJustifyFont = 0;
	transferStationType = ESW;
	Debug::trace("LabelApplierConfiguration", "clear():  Set transferStationType to ESW");
	primaryPrinterLabelOffset = 0;
	secondaryPrinterLabelOffset = 0;
	weigherConveyorSensitivity = ConveyorSensitivityMedium;
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
}

void LabelApplierConfiguration::setConfigType(LaConfigType configType)
{
	this->configType = configType;
}

LaConfigType LabelApplierConfiguration::getConfigType()
{
	return configType;
}

bool LabelApplierConfiguration::isWeigherStationAttached() const
{
	return weigherAttached;
}
void LabelApplierConfiguration::setWeigherStationAttached(bool value)
{
	weigherAttached = value;
}

void LabelApplierConfiguration::clearWeigherStationAttached()
{
	weigherAttached = false;
}

bool LabelApplierConfiguration::isPrimaryStationAttached() const
{
	return primaryPrinterAttached;
}
void LabelApplierConfiguration::setPrimaryStationAttached(bool /* value */)
{
	primaryPrinterAttached = true;
}

void LabelApplierConfiguration::clearPrimaryStationAttached()
{
	primaryPrinterAttached = false;
}

bool LabelApplierConfiguration::isSecondaryStationAttached() const
{
	return secondaryPrinterAttached;
}

void LabelApplierConfiguration::setSecondaryStationAttached(bool value)
{
	secondaryPrinterAttached = value;
}

void LabelApplierConfiguration::clearSecondaryStationAttached()
{
	secondaryPrinterAttached = false;
}

bool LabelApplierConfiguration::isLeftToRight() const
{
	return leftToRight;
}

void LabelApplierConfiguration::setLeftToRight(bool value)
{
	leftToRight = value;
}

void LabelApplierConfiguration::clearLeftToRight()
{
	leftToRight = false;
}

bool LabelApplierConfiguration::isFrontDischarge() const
{
	return frontDischarge;
}

void LabelApplierConfiguration::setFrontDischarge(bool value)
{
	frontDischarge = value;
}

void LabelApplierConfiguration::clearFrontDischarge()
{
	frontDischarge = false;
}

bool LabelApplierConfiguration::isPrimaryJustifyFont() const
{
	return primaryJustifyFont;
}

void LabelApplierConfiguration::setPrimaryJustifyFont(bool value)
{
	primaryJustifyFont = value;
}

void LabelApplierConfiguration::clearPrimaryJustifyFont()
{
	primaryJustifyFont = false;
}

bool LabelApplierConfiguration::isSecondaryJustifyFont() const
{
	return secondaryJustifyFont;
}

void LabelApplierConfiguration::setSecondaryJustifyFont(bool value)
{
	secondaryJustifyFont = value;
}

void LabelApplierConfiguration::clearSecondaryJustifyFont()
{
	secondaryJustifyFont = false;
}

void LabelApplierConfiguration::setTransferStationType(TransferStationType transferStationType)
{
	this->transferStationType = transferStationType;
	if (this->transferStationType == NSW)
	{
		Debug::trace("LabelApplierConfiguration", "setTransferStationType to NSW");
	}
	else if (this->transferStationType == HandWrapper)
	{
		Debug::trace("LabelApplierConfiguration", "setTransferStationType to HandWrapper");
	}
	else if (this->transferStationType == Bakery)
	{
		Debug::trace("LabelApplierConfiguration", "setTransferStationType to Bakery");
	}
	else if (this->transferStationType == ESW)
	{
		Debug::trace("LabelApplierConfiguration", "setTransferStationType to ESW");
	}
	else if (this->transferStationType == Mini)
	{
		Debug::trace("LabelApplierConfiguration", "setTransferStationType to Mini");
	}
	else
	{
		Debug::trace("LabelApplierConfiguration", HobString("setTransferStationType to Unknown, ") +
												  HobString(Integer::toString((UINT16) transferStationType)));
	}
}

TransferStationType LabelApplierConfiguration::getTransferStationType() const
{
	return transferStationType;
}

void LabelApplierConfiguration::setPrimaryPrinterLabelOffset(UINT8 primaryPrinterLabelOffset)
{
	this->primaryPrinterLabelOffset = primaryPrinterLabelOffset;
}

UINT8 LabelApplierConfiguration::getPrimaryPrinterLabelOffset() const
{
	return primaryPrinterLabelOffset;
}

void LabelApplierConfiguration::setSecondaryPrinterLabelOffset(UINT8 secondaryPrinterLabelOffset)
{
	this->secondaryPrinterLabelOffset = secondaryPrinterLabelOffset;
}

UINT8 LabelApplierConfiguration::getSecondaryPrinterLabelOffset() const
{
	return secondaryPrinterLabelOffset;
}

void LabelApplierConfiguration::setWeigherConveyorSensitivity(ConveyorSensitivity weigherConveyorSensitivity)
{
	this->weigherConveyorSensitivity = weigherConveyorSensitivity;
}

ConveyorSensitivity LabelApplierConfiguration::getWeigherConveyorSensitivity() const
{
	return weigherConveyorSensitivity;
}

void LabelApplierConfiguration::setPrimaryPrinterConveyorSensitivity(ConveyorSensitivity primaryPrinterConveyorSensitivity)
{
	this->primaryPrinterConveyorSensitivity = primaryPrinterConveyorSensitivity;
}

ConveyorSensitivity LabelApplierConfiguration::getPrimaryPrinterConveyorSensitivity() const
{
	return primaryPrinterConveyorSensitivity;
}

void LabelApplierConfiguration::setSecondaryPrinterConveyorSensitivity(ConveyorSensitivity secondaryPrinterConveyorSensitivity)
{
	this->secondaryPrinterConveyorSensitivity = secondaryPrinterConveyorSensitivity;
}

ConveyorSensitivity LabelApplierConfiguration::getSecondaryPrinterConveyorSensitivity() const
{
	return secondaryPrinterConveyorSensitivity;
}

void LabelApplierConfiguration::setPrimaryPrinterMaxJustificationDistance(UINT8 primaryPrinterMaxJustificationDistance)
{
	this->primaryPrinterMaxJustificationDistance = primaryPrinterMaxJustificationDistance;
}

UINT8 LabelApplierConfiguration::getPrimaryPrinterMaxJustificationDistance() const
{
	return primaryPrinterMaxJustificationDistance;
}

void LabelApplierConfiguration::setSecondaryPrinterMaxJustificationDistance(UINT8 secondaryPrinterMaxJustificationDistance)
{
	this->secondaryPrinterMaxJustificationDistance = secondaryPrinterMaxJustificationDistance;
}

UINT8 LabelApplierConfiguration::getSecondaryPrinterMaxJustificationDistance() const
{
	return secondaryPrinterMaxJustificationDistance;
}

bool LabelApplierConfiguration::isDiagonalLabelPlacement() const
{
	return diagonalLabelPlacement;
}

void LabelApplierConfiguration::setDiagonalLabelPlacement(bool value)
{
	diagonalLabelPlacement = value;
}

void LabelApplierConfiguration::clearDiagonalLabelPlacement()
{
	diagonalLabelPlacement = false;
}

void LabelApplierConfiguration::setDiagonalOffset(UINT8 diagonalOffset)
{
	this->diagonalOffset = diagonalOffset;
}

UINT8 LabelApplierConfiguration::getDiagonalOffset() const
{
	return diagonalOffset;
}

void LabelApplierConfiguration::setMiniStationLabelPlacement(LaPlacement miniStationLabelPlacement)
{
	this->miniStationLabelPlacement = miniStationLabelPlacement;
}

LaPlacement LabelApplierConfiguration::getMiniStationLabelPlacement() const
{
	Debug::trace("LabelApplierConfiguration", "getMiniStationLabelPlacement() = " + HobString(Integer::toString((UINT16)miniStationLabelPlacement)));
	return miniStationLabelPlacement;
}

void LabelApplierConfiguration::setDeviceSignature(UINT8 *deviceSignature)
{
	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		this->deviceSignature[i] = deviceSignature[i];
	}
}

void LabelApplierConfiguration::getDeviceSignature(UINT8 *deviceSignature) const
{
	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		deviceSignature[i] = this->deviceSignature[i];
	}
}

bool LabelApplierConfiguration::update(const LabelApplierConfiguration &laConfig)
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

	if (transferStationType != laConfig.getTransferStationType())
	{
		transferStationType = laConfig.getTransferStationType();
		rv = true;
	}

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

	return rv;
}

bool LabelApplierConfiguration::equals(const LabelApplierConfiguration &laConfig)
{
	bool rv = false;

	if ((weigherAttached == laConfig.isWeigherStationAttached()) &&
		(primaryPrinterAttached == laConfig.isPrimaryStationAttached()) &&
		(secondaryPrinterAttached == laConfig.isSecondaryStationAttached()) &&
		(leftToRight == laConfig.isLeftToRight()) &&
		(frontDischarge == laConfig.isFrontDischarge()) &&
		(transferStationType == laConfig.getTransferStationType()) &&
		(primaryPrinterLabelOffset == laConfig.getPrimaryPrinterLabelOffset()) &&
		(secondaryPrinterLabelOffset == laConfig.getSecondaryPrinterLabelOffset()) &&
		(weigherConveyorSensitivity == laConfig.getWeigherConveyorSensitivity()) &&
		(primaryPrinterConveyorSensitivity == laConfig.getPrimaryPrinterConveyorSensitivity()) &&
		(secondaryPrinterConveyorSensitivity == laConfig.getSecondaryPrinterConveyorSensitivity()) &&
		(primaryPrinterMaxJustificationDistance == laConfig.getPrimaryPrinterMaxJustificationDistance()) &&
		(miniStationLabelPlacement == laConfig.getMiniStationLabelPlacement()))
	{
		rv = true;
	}

	return rv;
}

bool LabelApplierConfiguration::update(const LabelApplierInquire &laInquire)
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

	if (transferStationType != laInquire.getTransferStationType())
	{
		transferStationType = laInquire.getTransferStationType();
		rv = true;
	}

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

	return rv;
}

bool LabelApplierConfiguration::equals(const LabelApplierInquire &laInquire)
{
	bool rv = false;

	if ((weigherAttached == laInquire.isWeigherStationAttached()) &&
		(primaryPrinterAttached == laInquire.isPrimaryStationAttached()) &&
		(secondaryPrinterAttached == laInquire.isSecondaryStationAttached()) &&
		(leftToRight == laInquire.isLeftToRight()) &&
		(frontDischarge == laInquire.isFrontDischarge()) &&
		(transferStationType == laInquire.getTransferStationType()) &&
		(primaryPrinterLabelOffset == laInquire.getPrimaryPrinterLabelOffset()) &&
		(secondaryPrinterLabelOffset == laInquire.getSecondaryPrinterLabelOffset()) &&
		(weigherConveyorSensitivity == laInquire.getWeigherConveyorSensitivity()) &&
		(primaryPrinterConveyorSensitivity == laInquire.getPrimaryPrinterConveyorSensitivity()) &&
		(secondaryPrinterConveyorSensitivity == laInquire.getSecondaryPrinterConveyorSensitivity()) &&
		(primaryPrinterMaxJustificationDistance == laInquire.getPrimaryPrinterMaxJustificationDistance()) &&
		(miniStationLabelPlacement == laInquire.getMiniStationLabelPlacement()))
	{
		rv = true;
	}

	return rv;
}
