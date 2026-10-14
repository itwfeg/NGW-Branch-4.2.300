#include "LabelApplierEvent.h"
#include "LabelApplierPositionInfo.h"

#include "Debug.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierPositionInfo::dataType = 0;
std::string LabelApplierPositionInfo::dataTypeName("LabelApplierPositionInfo"); 
#endif

LabelApplierPositionInfo::LabelApplierPositionInfo(Hobart::CEvolMsgReceiver *pHobThread,
        short printerStation,
        short primaryRotation,
        short secondaryRotation,
        LaPosLoc labelPosition	
        ) : EvolMessage(LabelApplier, LaSndLabelPos, pHobThread), 
    printerStation_(printerStation),
    primaryRotation_(primaryRotation), secondaryRotation_(secondaryRotation),
    labelPosition_(labelPosition),laSetup_(pHobThread, 0, 0, LocNoneSelected, 0, 0, LocNoneSelected)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}

LabelApplierPositionInfo::LabelApplierPositionInfo( Hobart::CEvolMsgReceiver *pHobThread,
	short printerStation,
	short primaryRotation,
	short secondaryRotation,
	LaPosLoc labelPosition,
	LaPosLoc secondLabelPosition,
	UINT16 primaryLabelSize,
	UINT16 secondaryLabelSize,
	UINT16 secondaryRotated
    ) : EvolMessage(LabelApplier, LaSndTwoLabelPos, pHobThread),
	printerStation_(printerStation),
	primaryRotation_(primaryRotation),
	secondaryRotation_(secondaryRotation),
	labelPosition_(labelPosition),
	secondLabelPosition_(secondLabelPosition),
	primaryLabelSize_(primaryLabelSize),
	secondaryLabelSize_(secondaryLabelSize),
    secondaryRotated_( secondaryRotated),
	laSetup_(pHobThread, 0, 0, LocNoneSelected, 0, 0, LocNoneSelected)
{
}

    LabelApplierPositionInfo::LabelApplierPositionInfo(const char *buffer)
: EvolMessage(buffer), printerStation_(MaxPrinterTypes),
    primaryRotation_(0), secondaryRotation_(0),
    labelPosition_(LocNoneSelected), laSetup_(0, 0, 0, LocNoneSelected, 0, 0, LocNoneSelected)
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
    char* pChar = (char *) &printerStation_;
    for (unsigned int i = 0; i < sizeof(printerStation_); i++)
    {
        *pChar++ = buffer[j++];
    }

    pChar = (char *) &labelPosition_;
    for (unsigned int i = 0; i < sizeof(labelPosition_); i++)
    {
        *pChar++ = buffer[j++];
    }

    pChar = (char *) &primaryRotation_;
    for (unsigned int i = 0; i < sizeof(primaryRotation_); i++)
    {
        *pChar++ = buffer[j++];
    }

    pChar = (char *) &secondaryRotation_;
    for (unsigned int i = 0; i < sizeof(secondaryRotation_); i++)
    {
        *pChar++ = buffer[j++];
    }
	
	pChar = (char *) &secondLabelPosition_;
	for (unsigned int i = 0; i < sizeof(secondLabelPosition_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&primaryLabelSize_;
	for (unsigned int i = 0; i < sizeof(primaryLabelSize_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&secondaryLabelSize_;
	for (unsigned int i = 0; i < sizeof(secondaryLabelSize_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

LabelApplierPositionInfo::~LabelApplierPositionInfo()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}

const LabelApplierSetup & LabelApplierPositionInfo::getLabelApplierSetup(UINT16 primaryLabelSize,
        UINT16 secondaryLabelSize)
{
    laSetup_.setPrimaryLabelSize(primaryLabelSize);
    laSetup_.setSecondaryLabelSize(secondaryLabelSize);

    laSetup_.setPrimaryLabelRotationDegrees(primaryRotation_ );
    laSetup_.setSecondaryLabelRotationDegrees(secondaryRotation_);

    laSetup_.setPrimaryLabelPlacement(labelPosition_);
    if (labelPosition_ == LocRight)
        laSetup_.setSecondaryLabelPlacement(LocLeft);
    else
        laSetup_.setSecondaryLabelPlacement(LocRight);

    return laSetup_;
}

const char *LabelApplierPositionInfo::serialize() const
{
    EvolMessage::serialize();

    // Now, for the derived class items
    char* pChar = (char *) &printerStation_;
    for (unsigned int i = 0; i < sizeof(printerStation_); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *) &labelPosition_;
    for (unsigned int i = 0; i < sizeof(labelPosition_); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *) &primaryRotation_;
    for (unsigned int i = 0; i < sizeof(primaryRotation_); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *) &secondaryRotation_;
    for (unsigned int i = 0; i < sizeof(secondaryRotation_); i++)
    {
        serializedData.append(1, pChar[i]);
    }

	pChar = (char *) &secondLabelPosition_;
	for (unsigned int i = 0; i < sizeof(secondLabelPosition_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&primaryLabelSize_;
	for (unsigned int i = 0; i < sizeof(primaryLabelSize_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&secondaryLabelSize_;
	for (unsigned int i = 0; i < sizeof(secondaryLabelSize_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    return serializedData.data();
}

void LabelApplierPositionInfo::show()
{
    bool isTracing = Debug::isTracing("LabelApplierPositionInfo");

    if (isTracing == false)
    {
#if defined(LINUX_FIX)
        Debug::addTraceCategory( std::string("LabelApplierPositionInfo") );
#else
        Debug::addTraceCategory("LabelApplierPositionInfo");
#endif
    }

    Debug::trace("LabelApplierPositionInfo", "*****  LabelApplierPositionInfo: ");

    EvolMessage::show();


    //Debug::trace("LabelApplierPositionInfo",  "           Printer: " + Integer::toString(printerStation_));
    HobString msgDebug = "    Label position: ";
    switch(labelPosition_)
    {
        case LocLeft:
            Debug::trace("LabelApplierPositionInfo", msgDebug + "Left");
            break;
        case LocRight:
            Debug::trace("LabelApplierPositionInfo", msgDebug + "Right");
            break;
        default:
            Debug::trace("LabelApplierPositionInfo", msgDebug + "Unknown value: " + Integer::toString(labelPosition_));
            break;
    }
    //Debug::trace("LabelApplierPositionInfo", "  Primary rotation:  " + Integer::toString(primaryRotation_));
    //Debug::trace("LabelApplierPositionInfo", "Secondary rotation:  " + Integer::toString(secondaryRotation_));
    Debug::trace("LabelApplierPositionInfo", "*********************************");

    if (isTracing == false)
    {
#if defined(LINUX_FIX)
        Debug::removeTraceCategory( std::string("LabelApplierPositionInfo") );
#else
        Debug::removeTraceCategory("LabelApplierPositionInfo");
#endif
    }
}


short LabelApplierPositionInfo::getPrinterStation()
{
    return printerStation_;
}

void LabelApplierPositionInfo::setPrinterStation(short printer)
{
    printerStation_ = printer;
}

LaPosLoc LabelApplierPositionInfo::getLabelPosition()
{
    return labelPosition_;
}
void LabelApplierPositionInfo::setLabelPosition(LaPosLoc location)
{
    labelPosition_ = location;
}

LaPosLoc LabelApplierPositionInfo::getSecondLabelPosition()
{
    return secondLabelPosition_;
}
void LabelApplierPositionInfo::setSecondLabelPosition(LaPosLoc location)
{
    secondLabelPosition_ = location;
}

short LabelApplierPositionInfo::getPrimaryRotation()
{
    return primaryRotation_;
}

void LabelApplierPositionInfo::setPrimaryRotation(short rotation)
{
    primaryRotation_ = rotation;
}

short LabelApplierPositionInfo::getSecondaryRotation()
{
    return secondaryRotation_;
}
void LabelApplierPositionInfo::setSecondaryRotation(short rotation)
{
    secondaryRotation_ = rotation;
}

UINT16 LabelApplierPositionInfo::getPrimaryLabelSize()
{
	return primaryLabelSize_;
}

void LabelApplierPositionInfo::setPrimaryLabelSize(UINT16 size)
{
	primaryLabelSize_ = size;
}

UINT16 LabelApplierPositionInfo::getSecondaryLabelSize()
{
	return secondaryLabelSize_;
}

void LabelApplierPositionInfo::setSecondaryLabelSize(UINT16 size)
{
	secondaryLabelSize_ = size;
}


UINT16 LabelApplierPositionInfo::getSecondaryLabelRotated()
{
	return secondaryRotated_;
}

void LabelApplierPositionInfo::setSecondaryLabelLabelRotated(UINT16 rotate)
{
	secondaryRotated_ = rotate;
}
