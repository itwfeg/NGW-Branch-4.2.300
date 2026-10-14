#include "LabelApplierEvent.h"
#include "LabelApplierSetup.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierSetup::dataType = 0;
std::string LabelApplierSetup::dataTypeName("LabelApplierSetup"); 
#endif

LabelApplierSetup::LabelApplierSetup(Hobart::CEvolMsgReceiver *pHobThread,
									 UINT16 primaryLabelSize,
									 LaRotation primaryLabelRotation,
									 LaPosLoc primaryLabelPlacement,
									 UINT16 secondaryLabelSize,
									 LaRotation secondaryLabelRotation,
									 LaPosLoc secondaryLabelPlacement) : EvolMessage(LabelApplier, LaSetup, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->primaryLabelSize = primaryLabelSize;
	this->primaryLabelRotation = primaryLabelRotation;
	this->primaryLabelPlacement = primaryLabelPlacement;
	this->secondaryLabelSize = secondaryLabelSize;
	this->secondaryLabelRotation = secondaryLabelRotation;
	this->secondaryLabelPlacement = secondaryLabelPlacement;

	this->rotatePrimaryPrintImage		= false;
	this->rotateSecondaryPrintImage		= false;
}

LabelApplierSetup::LabelApplierSetup(Hobart::CEvolMsgReceiver *pHobThread,
									 UINT16 primaryLabelSize,
									 UINT16 primaryLabelRotation,
									 LaPosLoc primaryLabelPlacement,
									 UINT16 secondaryLabelSize,
									 UINT16 secondaryLabelRotation,
									 LaPosLoc secondaryLabelPlacement) : EvolMessage(LabelApplier, LaSetup, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif


	this->primaryLabelSize = primaryLabelSize;
	this->primaryLabelRotation = degreesToRotation(primaryLabelRotation);
	this->primaryLabelPlacement = primaryLabelPlacement;
	this->secondaryLabelSize = secondaryLabelSize;
	this->secondaryLabelRotation = degreesToRotation(secondaryLabelRotation);
	this->secondaryLabelPlacement = secondaryLabelPlacement;

	this->rotatePrimaryPrintImage	= false;
	this->rotateSecondaryPrintImage = false;	
}

LabelApplierSetup::LabelApplierSetup(const char *buffer)	: EvolMessage(buffer)
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
	char *pChar = (char *) &primaryLabelSize;
	for (unsigned int i = 0; i < sizeof(primaryLabelSize); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &primaryLabelRotation;
	for (unsigned int i = 0; i < sizeof(primaryLabelRotation); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &primaryLabelPlacement;
	for (unsigned int i = 0; i < sizeof(primaryLabelPlacement); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secondaryLabelSize;
	for (unsigned int i = 0; i < sizeof(secondaryLabelSize); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secondaryLabelRotation;
	for (unsigned int i = 0; i < sizeof(secondaryLabelRotation); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secondaryLabelPlacement;
	for (unsigned int i = 0; i < sizeof(secondaryLabelPlacement); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &rotatePrimaryPrintImage;
	for (unsigned int i = 0; i < sizeof(rotatePrimaryPrintImage); i++)
	{
		*pChar++ = buffer[j++];
	}	
	
	pChar = (char *) &rotateSecondaryPrintImage;
	for (unsigned int i = 0; i < sizeof(rotateSecondaryPrintImage); i++)
	{
		*pChar++ = buffer[j++];
	}	
}

LabelApplierSetup::~LabelApplierSetup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
 
const char *LabelApplierSetup::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &primaryLabelSize;
	for (unsigned int i = 0; i < sizeof(primaryLabelSize); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &primaryLabelRotation;
	for (unsigned int i = 0; i < sizeof(primaryLabelRotation); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &primaryLabelPlacement;
	for (unsigned int i = 0; i < sizeof(primaryLabelPlacement); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secondaryLabelSize;
	for (unsigned int i = 0; i < sizeof(secondaryLabelSize); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secondaryLabelRotation;
	for (unsigned int i = 0; i < sizeof(secondaryLabelRotation); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secondaryLabelPlacement;
	for (unsigned int i = 0; i < sizeof(secondaryLabelPlacement); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &rotatePrimaryPrintImage;
	for (unsigned int i = 0; i < sizeof(rotatePrimaryPrintImage); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &rotateSecondaryPrintImage;
	for (unsigned int i = 0; i < sizeof(rotateSecondaryPrintImage); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

bool LabelApplierSetup::equals(LabelApplierSetup &laSetup)
{
	return ((primaryLabelSize == laSetup.primaryLabelSize) &&
			(primaryLabelRotation == laSetup.primaryLabelRotation) &&
			(primaryLabelPlacement == laSetup.primaryLabelPlacement) &&
			(secondaryLabelSize == laSetup.secondaryLabelSize) &&
			(secondaryLabelRotation == laSetup.secondaryLabelRotation) &&
			(secondaryLabelPlacement == laSetup.secondaryLabelPlacement) &&
			(rotatePrimaryPrintImage == laSetup.rotatePrimaryPrintImage) &&
			(rotateSecondaryPrintImage == laSetup.rotateSecondaryPrintImage) );
}

void LabelApplierSetup::show()
{
	if (!Debug::isTracing("LabelApplierSetup"))
		return;

	Debug::trace("LabelApplierSetup", "LabelApplierSetup: ");

	EvolMessage::show();

	Debug::trace("LabelApplierSetup", HobString("primaryLabelSize: ") + HobString(Integer::toString(primaryLabelSize)));

	if (primaryLabelRotation == Rotate0)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelRotation: Rotate0");
	}
	else if (primaryLabelRotation == Rotate45)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelRotation: Rotate45");
	}
	else if (primaryLabelRotation == Rotate90)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelRotation: Rotate90");
	}
	else if (primaryLabelRotation == Rotate135)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelRotation: Rotate135");
	}
	else if (primaryLabelRotation == Rotate180)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelRotation: Rotate180");
	}
	else if (primaryLabelRotation == Rotate225)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelRotation: Rotate225");
	}
	else if (primaryLabelRotation == Rotate270)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelRotation: Rotate270");
	}
	else if (primaryLabelRotation == Rotate315)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelRotation: Rotate315");
	}
	else if (primaryLabelRotation == NoRotationSelected)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelRotation: NoRotationSelected");
	}
	else
	{
		Debug::trace("LabelApplierSetup", HobString("secondaryLabelRotation: Unknown ") + HobString(Integer::toString(primaryLabelRotation)));
	}

	if (primaryLabelPlacement == LocRight)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelPlacement: LocRight");
	}
	else if (primaryLabelPlacement == LocLeft)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelPlacement: LocLeft");
	}
	else if (primaryLabelPlacement == LocNoneSelected)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelPlacement: LocNoneSelected");
	}
	else if (primaryLabelPlacement == LocLowerRight)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelPlacement: LocLowerRight");
	}
	else if (primaryLabelPlacement == LocLowerLeft)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelPlacement: LocLowerLeft");
	}
	else if (primaryLabelPlacement == LocRightOfPrimary)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelPlacement: LocRightOfPrimary");
	}
	else if (primaryLabelPlacement == LocLeftOfPrimary)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelPlacement: LocLeftOfPrimary");
	}
	else if (primaryLabelPlacement == LocTopOfPrimary)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelPlacement: LocTopOfPrimary");
	}
	else if (primaryLabelPlacement == LocBottomOfPrimary)
	{
		Debug::trace("LabelApplierSetup", "primaryLabelPlacement: LocBottomOfPrimary");
	}	
	else
	{
		Debug::trace("LabelApplierSetup", HobString("primaryLabelPlacement: Unknown ") + HobString(Integer::toString((UINT16) primaryLabelPlacement)));
	}

	Debug::trace("LabelApplierSetup", HobString("secondaryLabelSize: ") + HobString(Integer::toString(secondaryLabelSize)));

	if (secondaryLabelRotation == Rotate0)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelRotation: Rotate0");
	}
	else if (secondaryLabelRotation == Rotate45)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelRotation: Rotate45");
	}
	else if (secondaryLabelRotation == Rotate90)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelRotation: Rotate90");
	}
	else if (secondaryLabelRotation == Rotate135)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelRotation: Rotate135");
	}
	else if (secondaryLabelRotation == Rotate180)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelRotation: Rotate180");
	}
	else if (secondaryLabelRotation == Rotate225)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelRotation: Rotate225");
	}
	else if (secondaryLabelRotation == Rotate270)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelRotation: Rotate270");
	}
	else if (secondaryLabelRotation == Rotate315)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelRotation: Rotate315");
	}
	else if (secondaryLabelRotation == NoRotationSelected)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelRotation: NoRotationSelected");
	}
	else
	{
		Debug::trace("LabelApplierSetup", HobString("secondaryLabelRotation: Unknown ") + HobString(Integer::toString(secondaryLabelRotation)));
	}

	if (secondaryLabelPlacement == LocRight)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelPlacement: LocRight");
	}
	else if (secondaryLabelPlacement == LocLeft)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelPlacement: LocLeft");
	}
	else if (secondaryLabelPlacement == LocNoneSelected)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelPlacement: LocNoneSelected");
	}
	else if (secondaryLabelPlacement == LocLowerRight)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelPlacement: LocLowerRight");
	}
	else if (secondaryLabelPlacement == LocLowerLeft)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelPlacement: LocLowerLeft");
	}
	else if (secondaryLabelPlacement == LocRightOfPrimary)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelPlacement: LocRightOfPrimary");
	}
	else if (secondaryLabelPlacement == LocLeftOfPrimary)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelPlacement: LocLeftOfPrimary");
	}
	else if (secondaryLabelPlacement == LocTopOfPrimary)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelPlacement: LocTopOfPrimary");
	}
	else if (secondaryLabelPlacement == LocBottomOfPrimary)
	{
		Debug::trace("LabelApplierSetup", "secondaryLabelPlacement: LocBottomOfPrimary");
	}
	else
	{
		Debug::trace("LabelApplierSetup", HobString("secondaryLabelPlacement: Unknown ") + HobString(Integer::toString((UINT16) secondaryLabelPlacement)));
	}

	if (rotatePrimaryPrintImage) {
		Debug::trace("LabelApplierSetup", "Primary label image need to be rotated 180 degrees");
	} else {
		Debug::trace("LabelApplierSetup", "Primary label image does not require rotation");
	}

	if (rotateSecondaryPrintImage) {
		Debug::trace("LabelApplierSetup", "Secondary label image need to be rotated 180 degrees");
	} else {
		Debug::trace("LabelApplierSetup", "Secondary label image does not require rotation");
	}
}

void LabelApplierSetup::setPrimaryLabelSize(UINT16 primaryLabelSize)
{
	this->primaryLabelSize = primaryLabelSize;
}

UINT16 LabelApplierSetup::getPrimaryLabelSize() const
{
	return primaryLabelSize;
}

void LabelApplierSetup::setPrimaryLabelRotation(LaRotation primaryLabelRotation)
{
	this->primaryLabelRotation = primaryLabelRotation;
}

LaRotation LabelApplierSetup::getPrimaryLabelRotation() const
{
	return primaryLabelRotation;
}

void LabelApplierSetup::setPrimaryLabelRotationDegrees(UINT16 primaryLabelRotation)
{
	this->primaryLabelRotation = degreesToRotation(primaryLabelRotation);
}

UINT16 LabelApplierSetup::getPrimaryLabelRotationDegrees() const
{
	return rotationToDegrees(primaryLabelRotation);
}

void LabelApplierSetup::setPrimaryLabelPlacement(LaPosLoc primaryLabelPlacement)
{
	this->primaryLabelPlacement = primaryLabelPlacement;
}

LaPosLoc LabelApplierSetup::getPrimaryLabelPlacement() const
{
	return primaryLabelPlacement;
}

void LabelApplierSetup::setSecondaryLabelSize(UINT16 secondaryLabelSize)
{
	this->secondaryLabelSize = secondaryLabelSize;
}

UINT16 LabelApplierSetup::getSecondaryLabelSize() const
{
	return secondaryLabelSize;
}

void LabelApplierSetup::setSecondaryLabelRotation(LaRotation secondaryLabelRotation)
{
	this->secondaryLabelRotation = secondaryLabelRotation;
}

LaRotation LabelApplierSetup::getSecondaryLabelRotation() const
{
	return secondaryLabelRotation;
}

void LabelApplierSetup::setSecondaryLabelRotationDegrees(UINT16 secondaryLabelRotation)
{
	this->secondaryLabelRotation = degreesToRotation(secondaryLabelRotation);
}

UINT16 LabelApplierSetup::getSecondaryLabelRotationDegrees() const
{
	return rotationToDegrees(secondaryLabelRotation);
}

void LabelApplierSetup::setSecondaryLabelPlacement(LaPosLoc secondaryLabelPlacement)
{
	this->secondaryLabelPlacement = secondaryLabelPlacement;
}

LaPosLoc LabelApplierSetup::getSecondaryLabelPlacement() const
{
	return secondaryLabelPlacement;
}

UINT16 LabelApplierSetup::getPrimaryLabelRotationDegreesUltima() const
{
	return rotationToDegreesUltima(primaryLabelRotation);
}

UINT16 LabelApplierSetup::getSecondaryLabelRotationDegreesUltima() const
{
	return rotationToDegreesUltima(secondaryLabelRotation);
}

LaRotation LabelApplierSetup::degreesToRotation(UINT16 degrees) const
{
	LaRotation laRotation = NoRotationSelected;

	degrees = degrees % 360;

	if (degrees < 45)
	{
		laRotation = Rotate0;
	}
	else if ((45 <= degrees) && (degrees < 90))
	{
		laRotation = Rotate45;
	}
	else if ((90 <= degrees) && (degrees < 135))
	{
		laRotation = Rotate90;
	}
	else if ((135 <= degrees) && (degrees < 180))
	{
		laRotation = Rotate135;
	}
	else if ((180 <= degrees) && (degrees < 225))
	{
		laRotation = Rotate180;
	}
	else if ((225 <= degrees) && (degrees < 270))
	{
		laRotation = Rotate225;
	}
	else if ((270 <= degrees) && (degrees < 315))
	{
		laRotation = Rotate270;
	}
	else if ((315 <= degrees) && (degrees < 360))
	{
		laRotation = Rotate315;
	}

	return laRotation;
}

UINT16 LabelApplierSetup::rotationToDegrees(LaRotation laRotation) const
{
	UINT16 degrees = 0;

	switch (laRotation)
	{
		case NoRotationSelected:
		case Rotate0:
						degrees = 0;
						break;
		case Rotate45:
						degrees = 45;
						break;
		case Rotate90:
						degrees = 90;
						break;
		case Rotate135:
						degrees = 135;
						break;
		case Rotate180:
						degrees = 180;
						break;
		case Rotate225:
						degrees = 225;
						break;
		case Rotate270:
						degrees = 270;
						break;
		case Rotate315:
						degrees = 315;
						break;
		default:
						degrees = 0;
						break;
	}

	return degrees;
}

LaRotation LabelApplierSetup::degreesToRotationUltima(UINT16 degrees) const
{
	LaRotation laRotation = NoRotationSelected;

	degrees = degrees % 360;

	if (degrees < 90)
	{
		laRotation = Rotate0;
	}
	else if ((90 <= degrees) && (degrees < 180))
	{
		laRotation = Rotate90;
	}
	else if ((180 <= degrees) && (degrees < 270))
	{
		laRotation = Rotate180;
	}
	else if ((270 <= degrees) && (degrees < 360))
	{
		laRotation = Rotate270;
	}

	return laRotation;
}

UINT16 LabelApplierSetup::rotationToDegreesUltima(LaRotation laRotation) const
{
	UINT16 degrees = 0;

	switch (laRotation)
	{
		case Rotate90:
		case Rotate135:
						degrees = 90;
						break;
		case Rotate180:
		case Rotate225:
						degrees = 180;
						break;
		case Rotate270:
		case Rotate315:
						degrees = 270;
						break;
		case NoRotationSelected:
		case Rotate0:
		case Rotate45:
		default:
						degrees = 0;
						break;
	}

	return degrees;
}

// Existing Primary
void LabelApplierSetup::setRotatePrimaryPrintImage(UINT16 rotatePrimaryPrintImage)
{
	this->rotatePrimaryPrintImage = rotatePrimaryPrintImage;
}

UINT16 LabelApplierSetup::getRotatePrimaryPrintImage() const
{
	return rotatePrimaryPrintImage;
}

// Existing Secondary
void LabelApplierSetup::setRotateSecondaryPrintImage(UINT16 rotateSecondaryPrintImage)
{
	this->rotateSecondaryPrintImage = rotateSecondaryPrintImage;
}

UINT16 LabelApplierSetup::getRotateSecondaryPrintImage() const
{
	return rotateSecondaryPrintImage;
}
