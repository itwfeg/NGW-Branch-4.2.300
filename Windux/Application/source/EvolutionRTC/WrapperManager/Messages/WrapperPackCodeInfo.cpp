//#include "vxWorks.h"
#include "WrapperPackCodeInfo.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperPackCodeInfo::dataType = 0;
std::string WrapperPackCodeInfo::dataTypeName("WrapperPackCodeInfo"); 
#endif

WrapperPackCodeInfo::WrapperPackCodeInfo()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	description[0] = '\0';
	packCode = PackCode0;
	packWidth = FilmWidthIdle;
	packLength = FilmLengthIdle;
	slideBlock = false;
	booster = false;
	packageTotal = 0;
	jamTotal = 0;
}

WrapperPackCodeInfo::WrapperPackCodeInfo(const char *buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	int j = 0;

	char *pChar = (char *) &description;
	for (unsigned int i = 0; i < sizeof(description); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packCode;
	for (unsigned int i = 0; i < sizeof(packCode); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packWidth;
	for (unsigned int i = 0; i < sizeof(packWidth); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packLength;
	for (unsigned int i = 0; i < sizeof(packLength); i++)
	{
		*pChar++ = buffer[j++];
	}

	packSize = WrapperPackSize(&buffer[j]);

	packSize.serialize();
	j += packSize.getSerializedLength();

	sideClamp = WrapperTiming(&buffer[j]);

	sideClamp.serialize();
	j += sideClamp.getSerializedLength();

	gripper = WrapperTiming(&buffer[j]);

	gripper.serialize();
	j += gripper.getSerializedLength();

	knife = WrapperTiming(&buffer[j]);

	knife.serialize();
	j += knife.getSerializedLength();

	pChar = (char *) &slideBlock;
	for (unsigned int i = 0; i < sizeof(slideBlock); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &booster;
	for (unsigned int i = 0; i < sizeof(booster); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageTotal;
	for (unsigned int i = 0; i < sizeof(packageTotal); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &jamTotal;
	for (unsigned int i = 0; i < sizeof(jamTotal); i++)
	{
		*pChar++ = buffer[j++];
	}

}

WrapperPackCodeInfo::~WrapperPackCodeInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperPackCodeInfo::serialize() const
{
	serializedData = "";

	char *pChar = (char *) &description;
	for (unsigned int i = 0; i < sizeof(description); i++)
	{
		serializedData.append(1, pChar[i]);
	}	

	pChar = (char *) &packCode;
	for (unsigned int i = 0; i < sizeof(packCode); i++)
	{
		serializedData.append(1, pChar[i]);
	}	

	pChar = (char *) &packWidth;
	for (unsigned int i = 0; i < sizeof(packWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}	

	pChar = (char *) &packLength;
	for (unsigned int i = 0; i < sizeof(packLength); i++)
	{
		serializedData.append(1, pChar[i]);
	}	

	const char *pCC = packSize.serialize();
	int mLength = packSize.getSerializedLength();

	serializedData.append(pCC, mLength);

	pCC = sideClamp.serialize();
	mLength = sideClamp.getSerializedLength();

	serializedData.append(pCC, mLength);

	pCC = gripper.serialize();
	mLength = gripper.getSerializedLength();

	serializedData.append(pCC, mLength);

	pCC = knife.serialize();
	mLength = knife.getSerializedLength();

	serializedData.append(pCC, mLength);

	pChar = (char *) &slideBlock;
	for (unsigned int i = 0; i < sizeof(slideBlock); i++)
	{
		serializedData.append(1, pChar[i]);
	}	

	pChar = (char *) &booster;
	for (unsigned int i = 0; i < sizeof(booster); i++)
	{
		serializedData.append(1, pChar[i]);
	}	

	pChar = (char *) &packageTotal;
	for (unsigned int i = 0; i < sizeof(packageTotal); i++)
	{
		serializedData.append(1, pChar[i]);
	}	

	pChar = (char *) &jamTotal;
	for (unsigned int i = 0; i < sizeof(jamTotal); i++)
	{
		serializedData.append(1, pChar[i]);
	}	

	return serializedData.data();
}

UINT32 WrapperPackCodeInfo::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void WrapperPackCodeInfo::show()
{
	Debug::addTraceCategory("WrapperPackCodeInfo");
	Debug::trace("WrapperPackCodeInfo", "WrapperPackCodeInfo: ");

	if (packCode == PackCode0)
	{
		Debug::trace("WrapperPackCodeInfo", "packCode: PackCode0");
	}
	else if (packCode == PackCode1)
	{
		Debug::trace("WrapperPackCodeInfo", "packCode: PackCode1");
	}
	else if (packCode == PackCode2)
	{
		Debug::trace("WrapperPackCodeInfo", "packCode: PackCode2");
	}
	else if (packCode == PackCode3)
	{
		Debug::trace("WrapperPackCodeInfo", "packCode: PackCode3");
	}
	else if (packCode == PackCode4)
	{
		Debug::trace("WrapperPackCodeInfo", "packCode: PackCode4");
	}
	else if (packCode == PackCode5)
	{
		Debug::trace("WrapperPackCodeInfo", "packCode: PackCode5");
	}
	else if (packCode == PackCode6)
	{
		Debug::trace("WrapperPackCodeInfo", "packCode: PackCode6");
	}
	else if (packCode == PackCode7)
	{
		Debug::trace("WrapperPackCodeInfo", "packCode: PackCode7");
	}
	else if (packCode == PackCode8)
	{
		Debug::trace("WrapperPackCodeInfo", "packCode: PackCode8");
	}
	else if (packCode == PackCode9)
	{
		Debug::trace("WrapperPackCodeInfo", "packCode: PackCode9");
	}
	else if (packCode == PackCode10)
	{
		Debug::trace("WrapperPackCodeInfo", "packCode: PackCode10");
	}
	else if (packCode == PackCode11)
	{
		Debug::trace("WrapperPackCodeInfo", "packCode: PackCode11");
	}
	else if (packCode == UndefinedPackCode)
	{
		Debug::trace("WrapperPackCodeInfo", "packCode: UndefinedPackCode");
	}
	else
	{
		Debug::trace("WrapperPackCodeInfo", HobString("packCode: Unknown ") +
											HobString(Integer::toString((UINT16) packCode)));
	}

	if (packWidth == NarrowPackage)
	{
		Debug::trace("WrapperPackCodeInfo", "packWidth: NarrowPackage");
	}
	else if (packWidth == WidePackage)
	{
		Debug::trace("WrapperPackCodeInfo", "packWidth: WidePackage");
	}
	else if (packWidth == FilmWidthIdle)
	{
		Debug::trace("WrapperPackCodeInfo", "packWidth: FilmWidthIdle");
	}
	else if (packWidth == AutoWidthSelect)
	{
		Debug::trace("WrapperPackCodeInfo", "packWidth: AutoWidthSelect");
	}
	else
	{
		Debug::trace("WrapperPackCodeInfo", HobString("packWidth: Unknown ") +
											HobString(Integer::toString((UINT16) packWidth)));
	}

	Debug::trace("WrapperPackCodeInfo", HobString("description: ") + HobString((const char *) description));

	if (packLength == ShortPackage)
	{
		Debug::trace("WrapperPackCodeInfo", "packLength: ShortPackage");
	}
	else if (packLength == MediumPackage)
	{
		Debug::trace("WrapperPackCodeInfo", "packLength: MediumPackage");
	}
	else if (packLength == LongPackage)
	{
		Debug::trace("WrapperPackCodeInfo", "packLength: LongPackage");
	}
	else if (packLength == FilmLengthIdle)
	{
		Debug::trace("WrapperPackCodeInfo", "packLength: FilmLengthIdle");
	}
	else if (packLength == AutoLengthSelect)
	{
		Debug::trace("WrapperPackCodeInfo", "packLength: AutoLengthSelect");
	}
	else
	{
		Debug::trace("WrapperPackCodeInfo", HobString("packLength: Unknown ") +
											HobString(Integer::toString((UINT16) packLength)));
	}

	packSize.show();
	sideClamp.show();
	gripper.show();
	knife.show();

	if (slideBlock == true)
	{
		Debug::trace("WrapperPackCodeInfo", "slideBlock ON");
	}
	else
	{
		Debug::trace("WrapperPackCodeInfo", "slideBlock OFF");
	}

	if (booster == true)
	{
		Debug::trace("WrapperPackCodeInfo", "booster ON");
	}
	else
	{
		Debug::trace("WrapperPackCodeInfo", "booster OFF");
	}

	Debug::trace("WrapperPackCodeInfo", HobString("packageTotal: ") +
										HobString(Integer::toString(packageTotal)));
	Debug::trace("WrapperPackCodeInfo", HobString("jamTotal: ") +
										HobString(Integer::toString(jamTotal)));

	Debug::removeTraceCategory("WrapperPackCodeInfo");
}

void WrapperPackCodeInfo::getDescription(UINT8 *description) const
{
	int i = 0;

	while ((this->description[i] != '\0') && (i < PackCodeDescriptionLength - 1))
	{
		description[i] = this->description[i];
		i++;
	}

	// Set the NULL!!
	description[i] = this->description[i];
}

void WrapperPackCodeInfo::setDescription(UINT8 *description)
{
	int i = 0;

	while ((description[i] != '\0') && (i < PackCodeDescriptionLength - 1))
	{
		this->description[i] = description[i];
		i++;
	}

	// Set the NULL!!
	this->description[i] = description[i];
}

WrPackCode WrapperPackCodeInfo::getPackCode() const
{
	return packCode;
}

void WrapperPackCodeInfo::setPackCode(WrPackCode packCode)
{
	this->packCode = packCode;
}

WrPackWidth WrapperPackCodeInfo::getPackWidth() const
{
	return packWidth;
}

void WrapperPackCodeInfo::setPackWidth(WrPackWidth packWidth)
{
	this->packWidth = packWidth;
}

// Pack Width Variants
bool WrapperPackCodeInfo::isNarrowPackage() const
{
	return (packWidth == NarrowPackage);
}

bool WrapperPackCodeInfo::isWidePackage() const
{
	return (packWidth == WidePackage);
}

bool WrapperPackCodeInfo::isFilmWidthIdle() const
{
	return (packWidth == FilmWidthIdle);
}

bool WrapperPackCodeInfo::isAutoWidthSelect() const
{
	return (packWidth == AutoWidthSelect);
}

void WrapperPackCodeInfo::setNarrowPackage()
{
	packWidth = NarrowPackage;
}

void WrapperPackCodeInfo::setWidePackage()
{
	packWidth = WidePackage;
}

void WrapperPackCodeInfo::setFilmWidthIdle()
{
	packWidth = FilmWidthIdle;
}

void WrapperPackCodeInfo::setAutoWidthSelect()
{
	packWidth = AutoWidthSelect;
}

// Pack Length Variants
WrPackLength WrapperPackCodeInfo::getPackLength() const
{
	return packLength;
}

void WrapperPackCodeInfo::setPackLength(WrPackLength packLength)
{
	this->packLength = packLength;
}

bool WrapperPackCodeInfo::isShortPackage() const
{
	return (packLength == ShortPackage);
}

bool WrapperPackCodeInfo::isMediumPackage() const
{
	return (packLength == MediumPackage);
}

bool WrapperPackCodeInfo::isLongPackage() const
{
	return (packLength == LongPackage);
}

bool WrapperPackCodeInfo::isFilmLengthIdle() const
{
	return (packLength == FilmLengthIdle);
}

bool WrapperPackCodeInfo::isAutoLengthSelect() const
{
	return (packLength == AutoLengthSelect);
}

void WrapperPackCodeInfo::setShortPackage()
{
	packLength = ShortPackage;
}

void WrapperPackCodeInfo::setMediumPackage()
{
	packLength = MediumPackage;
}

void WrapperPackCodeInfo::setLongPackage()
{
	packLength = LongPackage;
}

void WrapperPackCodeInfo::setFilmLengthIdle()
{
	packLength = FilmLengthIdle;
}

void WrapperPackCodeInfo::setAutoLengthSelect()
{
	packLength = AutoLengthSelect;
}

// Pack Size Variants
WrapperPackSize &WrapperPackCodeInfo::getPackSize()
{
	return packSize;
}

void WrapperPackCodeInfo::setPackSize(WrapperPackSize &packSize)
{
	this->packSize = packSize;
}

void WrapperPackCodeInfo::setPackSizeMinLength(UINT16 packSizeMinLength)
{
	packSize.setMinLength(packSizeMinLength);
}

UINT16 WrapperPackCodeInfo::getPackSizeMinLength() const
{
	return packSize.getMinLength();
}

void WrapperPackCodeInfo::setPackSizeMaxLength(UINT16 packSizeMaxLength)
{
	packSize.setMaxLength(packSizeMaxLength);
}

UINT16 WrapperPackCodeInfo::getPackSizeMaxLength() const
{
	return packSize.getMaxLength();
}

void WrapperPackCodeInfo::setPackSizeMinWidth(UINT16 packSizeMinWidth)
{
	packSize.setMinWidth(packSizeMinWidth);
}

UINT16 WrapperPackCodeInfo::getPackSizeMinWidth() const
{
	return packSize.getMinWidth();
}

void WrapperPackCodeInfo::setPackSizeMaxWidth(UINT16 packSizeMaxWidth)
{
	packSize.setMaxWidth(packSizeMaxWidth);
}

UINT16 WrapperPackCodeInfo::getPackSizeMaxWidth() const
{
	return packSize.getMaxWidth();
}

// sideClamp Variants
WrapperTiming &WrapperPackCodeInfo::getSideClamp()
{
	return sideClamp;
}

void WrapperPackCodeInfo::setSideClamp(WrapperTiming &sideClamp)
{
	this->sideClamp = sideClamp;
}

void WrapperPackCodeInfo::setSideClampOnMin(UINT8 sideClampOnMin)
{
	sideClamp.setOnMin(sideClampOnMin);
}

UINT8 WrapperPackCodeInfo::getSideClampOnMin() const
{
	return sideClamp.getOnMin();
}

void WrapperPackCodeInfo::setSideClampOnValue(UINT8 sideClampOnValue)
{
	sideClamp.setOnValue(sideClampOnValue);
}

UINT8 WrapperPackCodeInfo::getSideClampOnValue() const
{
	return sideClamp.getOnValue();
}

void WrapperPackCodeInfo::setSideClampOnMax(UINT8 sideClampOnMax)
{
	sideClamp.setOnMax(sideClampOnMax);
}

UINT8 WrapperPackCodeInfo::getSideClampOnMax() const
{
	return sideClamp.getOnMax();
}

void WrapperPackCodeInfo::setSideClampOffMin(UINT8 sideClampOffMin)
{
	sideClamp.setOffMin(sideClampOffMin);
}

UINT8 WrapperPackCodeInfo::getSideClampOffMin() const
{
	return sideClamp.getOffMin();
}

void WrapperPackCodeInfo::setSideClampOffValue(UINT8 sideClampOffValue)
{
	sideClamp.setOffValue(sideClampOffValue);
}

UINT8 WrapperPackCodeInfo::getSideClampOffValue() const
{
	return sideClamp.getOffValue();
}

void WrapperPackCodeInfo::setSideClampOffMax(UINT8 sideClampOffMax)
{
	sideClamp.setOffMax(sideClampOffMax);
}

UINT8 WrapperPackCodeInfo::getSideClampOffMax() const
{
	return sideClamp.getOffMax();
}

// gripper Variants
WrapperTiming &WrapperPackCodeInfo::getGripper()
{
	return gripper;
}

void WrapperPackCodeInfo::setGripper(WrapperTiming &gripper)
{
	this->gripper = gripper;
}

void WrapperPackCodeInfo::setGripperOnMin(UINT8 gripperOnMin)
{
	gripper.setOnMin(gripperOnMin);
}

UINT8 WrapperPackCodeInfo::getGripperOnMin() const
{
	return gripper.getOnMin();
}

void WrapperPackCodeInfo::setGripperOnValue(UINT8 gripperOnValue)
{
	gripper.setOnValue(gripperOnValue);
}

UINT8 WrapperPackCodeInfo::getGripperOnValue() const
{
	return gripper.getOnValue();
}

void WrapperPackCodeInfo::setGripperOnMax(UINT8 gripperOnMax)
{
	gripper.setOnMax(gripperOnMax);
}

UINT8 WrapperPackCodeInfo::getGripperOnMax() const
{
	return gripper.getOnMax();
}

void WrapperPackCodeInfo::setGripperOffMin(UINT8 gripperOffMin)
{
	gripper.setOffMin(gripperOffMin);
}

UINT8 WrapperPackCodeInfo::getGripperOffMin() const
{
	return gripper.getOffMin();
}

void WrapperPackCodeInfo::setGripperOffValue(UINT8 gripperOffValue)
{
	gripper.setOffValue(gripperOffValue);
}

UINT8 WrapperPackCodeInfo::getGripperOffValue() const
{
	return gripper.getOffValue();
}

void WrapperPackCodeInfo::setGripperOffMax(UINT8 gripperOffMax)
{
	gripper.setOffMax(gripperOffMax);
}

UINT8 WrapperPackCodeInfo::getGripperOffMax() const
{
	return gripper.getOffMax();
}

// knife Variants
WrapperTiming &WrapperPackCodeInfo::getKnife()
{
	return knife;
}

void WrapperPackCodeInfo::setKnife(WrapperTiming &knife)
{
	this->knife = knife;
}

void WrapperPackCodeInfo::setKnifeOnMin(UINT8 knifeOnMin)
{
	knife.setOnMin(knifeOnMin);
}

UINT8 WrapperPackCodeInfo::getKnifeOnMin() const
{
	return knife.getOnMin();
}

void WrapperPackCodeInfo::setKnifeOnValue(UINT8 knifeOnValue)
{
	knife.setOnValue(knifeOnValue);
}

UINT8 WrapperPackCodeInfo::getKnifeOnValue() const
{
	return knife.getOnValue();
}

void WrapperPackCodeInfo::setKnifeOnMax(UINT8 knifeOnMax)
{
	knife.setOnMax(knifeOnMax);
}

UINT8 WrapperPackCodeInfo::getKnifeOnMax() const
{
	return knife.getOnMax();
}

void WrapperPackCodeInfo::setKnifeOffMin(UINT8 knifeOffMin)
{
	knife.setOffMin(knifeOffMin);
}

UINT8 WrapperPackCodeInfo::getKnifeOffMin() const
{
	return knife.getOffMin();
}

void WrapperPackCodeInfo::setKnifeOffValue(UINT8 knifeOffValue)
{
	knife.setOffValue(knifeOffValue);
}

UINT8 WrapperPackCodeInfo::getKnifeOffValue() const
{
	return knife.getOffValue();
}

void WrapperPackCodeInfo::setKnifeOffMax(UINT8 knifeOffMax)
{
	knife.setOffMax(knifeOffMax);
}

UINT8 WrapperPackCodeInfo::getKnifeOffMax() const
{
	return knife.getOffMax();
}

// Booleans
bool WrapperPackCodeInfo::isSlideBlock() const
{
	return slideBlock;
}

void WrapperPackCodeInfo::setSlideBlock()
{
	slideBlock = true;
}

void WrapperPackCodeInfo::clearSlideBlock()
{
	slideBlock = false;
}

bool WrapperPackCodeInfo::isBooster() const
{
	return booster;
}

void WrapperPackCodeInfo::setBooster()
{
	booster = true;
}

void WrapperPackCodeInfo::clearBooster()
{
	booster = false;
}

// Totals
void WrapperPackCodeInfo::setPackageTotal(UINT32 packageTotal)
{
	this->packageTotal = packageTotal;
}

UINT32 WrapperPackCodeInfo::getPackageTotal() const
{
	return packageTotal;
}

void WrapperPackCodeInfo::setJamTotal(UINT32 jamTotal)
{
	this->jamTotal = jamTotal;
}

UINT32 WrapperPackCodeInfo::getJamTotal() const
{
	return jamTotal;
}
