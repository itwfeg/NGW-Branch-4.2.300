//#include "vxWorks.h"
#include "WrapperPackageInfo.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperPackageInfo::dataType = 0;
std::string WrapperPackageInfo::dataTypeName("WrapperPackageInfo"); 
#endif

WrapperPackageInfo::WrapperPackageInfo()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	present = false;
	pullFilm = false;
	ghostPackage = false;
	slowDelay = false;
	quickDelay = false;
	intakeMode = WrapperIntakeMode::NormalIntake;
	slideBlock = false;
	booster = false;
	packCode = PackCode0;
	packWidth = FilmWidthIdle;
	packLength = FilmLengthIdle;
	slideBlock = false;
	booster = false;
	packageId = 0;
	length = 0;
	width = 0;
}

WrapperPackageInfo::WrapperPackageInfo(const char *buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	int j = 0;

	char *pChar = (char *) &present;
	for (unsigned int i = 0; i < sizeof(present); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pullFilm;
	for (unsigned int i = 0; i < sizeof(pullFilm); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &ghostPackage;
	for (unsigned int i = 0; i < sizeof(ghostPackage); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &slowDelay;
	for (unsigned int i = 0; i < sizeof(slowDelay); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &quickDelay;
	for (unsigned int i = 0; i < sizeof(quickDelay); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &intakeMode;
	for (unsigned int i = 0; i < sizeof(intakeMode); i++)
	{
		*pChar++ = buffer[j++];
	}

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

	pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &length;
	for (unsigned int i = 0; i < sizeof(length); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &width;
	for (unsigned int i = 0; i < sizeof(width); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperPackageInfo::~WrapperPackageInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperPackageInfo::serialize() const
{
	serializedData = "";

	char *pChar = (char *) &present;
	for (unsigned int i = 0; i < sizeof(present); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pullFilm;
	for (unsigned int i = 0; i < sizeof(pullFilm); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &ghostPackage;
	for (unsigned int i = 0; i < sizeof(ghostPackage); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &slowDelay;
	for (unsigned int i = 0; i < sizeof(slowDelay); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &quickDelay;
	for (unsigned int i = 0; i < sizeof(quickDelay); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &intakeMode;
	for (unsigned int i = 0; i < sizeof(intakeMode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

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

	pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &length;
	for (unsigned int i = 0; i < sizeof(length); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &width;
	for (unsigned int i = 0; i < sizeof(width); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT32 WrapperPackageInfo::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void WrapperPackageInfo::show() const
{
	Debug::addTraceCategory("WrapperPackageInfo");
	Debug::trace("WrapperPackageInfo", "WrapperPackageInfo: ");

	if (present == true)
	{
		Debug::trace("WrapperPackageInfo", "present ON");
	}
	else
	{
		Debug::trace("WrapperPackageInfo", "present OFF");
	}

	if (pullFilm == true)
	{
		Debug::trace("WrapperPackageInfo", "pullFilm ON");
	}
	else
	{
		Debug::trace("WrapperPackageInfo", "pullFilm OFF");
	}

	if (ghostPackage == true)
	{
		Debug::trace("WrapperPackageInfo", "ghostPackage ON");
	}
	else
	{
		Debug::trace("WrapperPackageInfo", "ghostPackage OFF");
	}

	if (slowDelay == true)
	{
		Debug::trace("WrapperPackageInfo", "slowDelay ON");
	}
	else
	{
		Debug::trace("WrapperPackageInfo", "slowDelay OFF");
	}

	if (quickDelay == true)
	{
		Debug::trace("WrapperPackageInfo", "quickDelay ON");
	}
	else
	{
		Debug::trace("WrapperPackageInfo", "quickDelay OFF");
	}

	if (intakeMode == WrapperIntakeMode::DefaultIntake)
	{
		Debug::trace("WrapperPackageInfo", "intakeMode: DefaultIntake");
	}
	else if (intakeMode == WrapperIntakeMode::GentleIntake)
	{
		Debug::trace("WrapperPackageInfo", "intakeMode: GentleIntake");
	}
	else if (intakeMode == WrapperIntakeMode::NormalIntake)
	{
		Debug::trace("WrapperPackageInfo", "intakeMode: NormalIntake");
	}
	else if (intakeMode == WrapperIntakeMode::FastIntake)
	{
		Debug::trace("WrapperPackageInfo", "intakeMode: FastIntake");
	}
	else
	{
		Debug::trace("WrapperPackageInfo", HobString("intakeMode: Unknown ") +
										   HobString(Integer::toString((UINT16) intakeMode)));
	}

	if (slideBlock == true)
	{
		Debug::trace("WrapperPackageInfo", "slideBlock ON");
	}
	else
	{
		Debug::trace("WrapperPackageInfo", "slideBlock OFF");
	}

	if (booster == true)
	{
		Debug::trace("WrapperPackageInfo", "booster ON");
	}
	else
	{
		Debug::trace("WrapperPackageInfo", "booster OFF");
	}

	if (packCode == PackCode0)
	{
		Debug::trace("WrapperPackageInfo", "packCode: PackCode0");
	}
	else if (packCode == PackCode1)
	{
		Debug::trace("WrapperPackageInfo", "packCode: PackCode1");
	}
	else if (packCode == PackCode2)
	{
		Debug::trace("WrapperPackageInfo", "packCode: PackCode2");
	}
	else if (packCode == PackCode3)
	{
		Debug::trace("WrapperPackageInfo", "packCode: PackCode3");
	}
	else if (packCode == PackCode4)
	{
		Debug::trace("WrapperPackageInfo", "packCode: PackCode4");
	}
	else if (packCode == PackCode5)
	{
		Debug::trace("WrapperPackageInfo", "packCode: PackCode5");
	}
	else if (packCode == PackCode6)
	{
		Debug::trace("WrapperPackageInfo", "packCode: PackCode6");
	}
	else if (packCode == PackCode7)
	{
		Debug::trace("WrapperPackageInfo", "packCode: PackCode7");
	}
	else if (packCode == PackCode8)
	{
		Debug::trace("WrapperPackageInfo", "packCode: PackCode8");
	}
	else if (packCode == PackCode9)
	{
		Debug::trace("WrapperPackageInfo", "packCode: PackCode9");
	}
	else if (packCode == PackCode10)
	{
		Debug::trace("WrapperPackageInfo", "packCode: PackCode10");
	}
	else if (packCode == PackCode11)
	{
		Debug::trace("WrapperPackageInfo", "packCode: PackCode11");
	}
	else if (packCode == UndefinedPackCode)
	{
		Debug::trace("WrapperPackageInfo", "packCode: UndefinedPackCode");
	}
	else
	{
		Debug::trace("WrapperPackageInfo", HobString("packCode: Unknown ") +
											HobString(Integer::toString((UINT16) packCode)));
	}

	if (packWidth == NarrowPackage)
	{
		Debug::trace("WrapperPackageInfo", "packWidth: NarrowPackage");
	}
	else if (packWidth == WidePackage)
	{
		Debug::trace("WrapperPackageInfo", "packWidth: WidePackage");
	}
	else if (packWidth == FilmWidthIdle)
	{
		Debug::trace("WrapperPackageInfo", "packWidth: FilmWidthIdle");
	}
	else if (packWidth == AutoWidthSelect)
	{
		Debug::trace("WrapperPackageInfo", "packWidth: AutoWidthSelect");
	}
	else
	{
		Debug::trace("WrapperPackageInfo", HobString("packWidth: Unknown ") +
										   HobString(Integer::toString((UINT16) packWidth)));
	}

	if (packLength == ShortPackage)
	{
		Debug::trace("WrapperPackageInfo", "packLength: ShortPackage");
	}
	else if (packLength == MediumPackage)
	{
		Debug::trace("WrapperPackageInfo", "packLength: MediumPackage");
	}
	else if (packLength == LongPackage)
	{
		Debug::trace("WrapperPackageInfo", "packLength: LongPackage");
	}
	else if (packLength == FilmLengthIdle)
	{
		Debug::trace("WrapperPackageInfo", "packLength: FilmLengthIdle");
	}
	else if (packLength == AutoLengthSelect)
	{
		Debug::trace("WrapperPackageInfo", "packLength: AutoLengthSelect");
	}
	else
	{
		Debug::trace("WrapperPackageInfo", HobString("packLength: Unknown ") +
										   HobString(Integer::toString((UINT16) packLength)));
	}

	Debug::trace("WrapperPackageInfo", HobString("packageId: ") +
									   HobString(Integer::toString(packageId)));
	Debug::trace("WrapperPackageInfo", HobString("length: ") +
									   HobString(Integer::toString(length)));
	Debug::trace("WrapperPackageInfo", HobString("width: ") +
									   HobString(Integer::toString(width)));

	Debug::removeTraceCategory("WrapperPackageInfo");
}

bool WrapperPackageInfo::isPresent() const
{
	return present;
}

void WrapperPackageInfo::setPresent()
{
	present = true;
}

void WrapperPackageInfo::clearPresent()
{
	present = false;
}

bool WrapperPackageInfo::isPullFilm() const
{
	return pullFilm;
}

void WrapperPackageInfo::setPullFilm()
{
	pullFilm = true;
}

void WrapperPackageInfo::clearPullFilm()
{
	pullFilm = false;
}

bool WrapperPackageInfo::isGhostPackage() const
{
	return ghostPackage;
}

void WrapperPackageInfo::setGhostPackage()
{
	ghostPackage = true;
}

void WrapperPackageInfo::clearGhostPackage()
{
	ghostPackage = false;
}

bool WrapperPackageInfo::isSlowDelay() const
{
	return slowDelay;
}

void WrapperPackageInfo::setSlowDelay()
{
	slowDelay = true;
}

void WrapperPackageInfo::clearSlowDelay()
{
	slowDelay = false;
}

bool WrapperPackageInfo::isQuickDelay() const
{
	return quickDelay;
}

void WrapperPackageInfo::setQuickDelay()
{
	quickDelay = true;
}

void WrapperPackageInfo::clearQuickDelay()
{
	quickDelay = false;
}

void WrapperPackageInfo::setIntakeMode(WrapperIntakeMode::IntakeMode intakeMode)
{
	this->intakeMode = intakeMode;
}

WrapperIntakeMode::IntakeMode WrapperPackageInfo::getIntakeMode() const
{
	return intakeMode;
}

bool WrapperPackageInfo::isIntakeModeNormal() const
{
	return (intakeMode == WrapperIntakeMode::NormalIntake);
}

bool WrapperPackageInfo::isIntakeModeGentle() const
{
	return (intakeMode == WrapperIntakeMode::GentleIntake);
}

bool WrapperPackageInfo::isSlideBlock() const
{
	return slideBlock;
}

void WrapperPackageInfo::setSlideBlock()
{
	slideBlock = true;
}

void WrapperPackageInfo::clearSlideBlock()
{
	slideBlock = false;
}

bool WrapperPackageInfo::isBooster() const
{
	return booster;
}

void WrapperPackageInfo::setBooster()
{
	booster = true;
}

void WrapperPackageInfo::clearBooster()
{
	booster = false;
}

WrPackCode WrapperPackageInfo::getPackCode() const
{
	return packCode;
}

void WrapperPackageInfo::setPackCode(WrPackCode packCode)
{
	this->packCode = packCode;
}

WrPackWidth WrapperPackageInfo::getPackWidth() const
{
	return packWidth;
}

void WrapperPackageInfo::setPackWidth(WrPackWidth packWidth)
{
	this->packWidth = packWidth;
}

// Pack Width Variants
bool WrapperPackageInfo::isNarrowPackage() const
{
	return (packWidth == NarrowPackage);
}

bool WrapperPackageInfo::isWidePackage() const
{
	return (packWidth == WidePackage);
}

bool WrapperPackageInfo::isFilmWidthIdle() const
{
	return (packWidth == FilmWidthIdle);
}

bool WrapperPackageInfo::isAutoWidthSelect() const
{
	return (packWidth == AutoWidthSelect);
}

void WrapperPackageInfo::setNarrowPackage()
{
	packWidth = NarrowPackage;
}

void WrapperPackageInfo::setWidePackage()
{
	packWidth = WidePackage;
}

void WrapperPackageInfo::setFilmWidthIdle()
{
	packWidth = FilmWidthIdle;
}

void WrapperPackageInfo::setAutoWidthSelect()
{
	packWidth = AutoWidthSelect;
}

// Pack Length Variants
WrPackLength WrapperPackageInfo::getPackLength() const
{
	return packLength;
}

void WrapperPackageInfo::setPackLength(WrPackLength packLength)
{
	this->packLength = packLength;
}

bool WrapperPackageInfo::isShortPackage() const
{
	return (packLength == ShortPackage);
}

bool WrapperPackageInfo::isMediumPackage() const
{
	return (packLength == MediumPackage);
}

bool WrapperPackageInfo::isLongPackage() const
{
	return (packLength == LongPackage);
}

bool WrapperPackageInfo::isFilmLengthIdle() const
{
	return (packLength == FilmLengthIdle);
}

bool WrapperPackageInfo::isAutoLengthSelect() const
{
	return (packLength == AutoLengthSelect);
}

void WrapperPackageInfo::setShortPackage()
{
	packLength = ShortPackage;
}

void WrapperPackageInfo::setMediumPackage()
{
	packLength = MediumPackage;
}

void WrapperPackageInfo::setLongPackage()
{
	packLength = LongPackage;
}

void WrapperPackageInfo::setFilmLengthIdle()
{
	packLength = FilmLengthIdle;
}

void WrapperPackageInfo::setAutoLengthSelect()
{
	packLength = AutoLengthSelect;
}

void WrapperPackageInfo::setPackageId(UINT32 packageId)
{
	this->packageId = packageId;
}

UINT32 WrapperPackageInfo::getPackageId() const
{
	return packageId;
}

void WrapperPackageInfo::setLength(UINT16 length)
{
	this->length = length;
}

UINT16 WrapperPackageInfo::getLength() const
{
	return length;
}

void WrapperPackageInfo::setWidth(UINT16 width)
{
	this->width = width;
}

UINT16 WrapperPackageInfo::getWidth() const
{
	return width;
}

void WrapperPackageInfo::setPackageBits(WrPackageBits packageBits)
{
	if (packageBits.present == 1)
	{
		present = true;
	}
	else
	{
		present = false;
	}

	if (packageBits.pullFilm == 1)
	{
		pullFilm = true;
	}
	else
	{
		pullFilm = false;
	}

	if (packageBits.ghostPackage == 1)
	{
		ghostPackage = true;
	}
	else
	{
		ghostPackage = false;
	}

	if (packageBits.slowDelay == 1)
	{
		slowDelay = true;
	}
	else
	{
		slowDelay = false;
	}

	if (packageBits.quickDelay == 1)
	{
		quickDelay = true;
	}
	else
	{
		quickDelay = false;
	}

	intakeMode = (WrapperIntakeMode::IntakeMode) packageBits.intakeMode;

	if (packageBits.slideBlock == 1)
	{
		slideBlock = true;
	}
	else
	{
		slideBlock = false;
	}

	if (packageBits.booster == 1)
	{
		booster = true;
	}
	else
	{
		booster = false;
	}

	packCode = (WrPackCode) packageBits.packCode;
	packWidth = (WrPackWidth) packageBits.packWidth;
	packLength = (WrPackLength) packageBits.packLength;
	packageId = packageBits.packageId;
	length = packageBits.length;
	width = packageBits.width;
}

WrPackageBits WrapperPackageInfo::getPackageBits()
{
	WrPackageBits packageBits;

	memset (&packageBits, 0, sizeof (packageBits));

	if (present == true)
	{
		packageBits.present = 1;
	}

	if (pullFilm == true)
	{
		packageBits.pullFilm = 1;
	}

	if (ghostPackage == true)
	{
		packageBits.ghostPackage = 1;
	}

	if (slowDelay == true)
	{
		packageBits.slowDelay = 1;
	}

	if (quickDelay == true)
	{
		packageBits.quickDelay = 1;
	}

	packageBits.intakeMode = intakeMode;

	if (slideBlock == true)
	{
		packageBits.slideBlock = 1;
	}

	if (booster == true)
	{
		packageBits.booster = 1;
	}

	packageBits.packCode = (WrPackCode) packCode;
	packageBits.packWidth = (WrPackWidth) packWidth;
	packageBits.packLength = (WrPackLength) packLength;
	packageBits.packageId = packageId;
	packageBits.length = length;
	packageBits.width = width;

	return packageBits;
}

bool WrapperPackageInfo::equals(WrapperPackageInfo &wrPackageInfo) const
{
	bool rv = false;

	if ((present == wrPackageInfo.isPresent()) &&
	    (pullFilm == wrPackageInfo.isPullFilm()) &&
		(ghostPackage == wrPackageInfo.isGhostPackage()) &&
		(slowDelay == wrPackageInfo.isSlowDelay()) &&
		(quickDelay == wrPackageInfo.isQuickDelay()) &&
		(intakeMode == wrPackageInfo.getIntakeMode()) &&
		(slideBlock == wrPackageInfo.isSlideBlock()) &&
		(booster == wrPackageInfo.isBooster()) &&
		(packCode == wrPackageInfo.getPackCode()) &&
		(packWidth == wrPackageInfo.getPackWidth()) &&
		(packLength == wrPackageInfo.getPackLength()) &&
		(packageId == wrPackageInfo.getPackageId()) &&
		(length == wrPackageInfo.getLength()) &&
		(width == wrPackageInfo.getWidth()))
	{
		rv = true;
	}

	return rv;
}
