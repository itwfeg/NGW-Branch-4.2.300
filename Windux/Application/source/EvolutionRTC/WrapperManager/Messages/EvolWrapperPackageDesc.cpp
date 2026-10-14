#include "WrapperEvent.h"
#include "EvolWrapperPackageDesc.h"

#include "Debug.h"
#include "Integer.h"
#include "UInteger.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperPackageDesc::_dataType_ = 0;
std::string EvolWrapperPackageDesc::_dataTypeName_("EvolWrapperPackageDesc"); 
#endif

EvolWrapperPackageDesc::EvolWrapperPackageDesc(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrEvolPackageDesc, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	id = 0;
	length = 0;
	width = 0;
	height = 0;
	lip = 0;
#ifdef NEW_FILM_WIDTH_VAR
	filmWidthSelection = Automatic;
#else
	filmWidthUsedToWrap = 0;
#endif
	wrapBoxIndex = 0;
	heightBoxIndex = 0;
	usedWrapBoxBestFit = false;
	modifiedNetStretch = false;
	trayName = "";
}

EvolWrapperPackageDesc::EvolWrapperPackageDesc(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	char name[_MaxNameLength_ + 1];

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	// Now, for the derived class items

	char *pChar = (char *) &id;
	for (unsigned int i = 0; i < sizeof(id); i++)
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

	pChar = (char *) &height;
	for (unsigned int i = 0; i < sizeof(height); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &lip;
	for (unsigned int i = 0; i < sizeof(lip); i++)
	{
		*pChar++ = buffer[j++];
	}

#ifdef NEW_FILM_WIDTH_VAR
	pChar = (char *) &filmWidthSelection;
	for (unsigned int i = 0; i < sizeof(filmWidthSelection); i++)
#else
	pChar = (char *) &filmWidthUsedToWrap;
	for (unsigned int i = 0; i < sizeof(filmWidthUsedToWrap); i++)
#endif
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &wrapBoxIndex;
	for (unsigned int i = 0; i < sizeof(wrapBoxIndex); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &heightBoxIndex;
	for (unsigned int i = 0; i < sizeof(heightBoxIndex); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &usedWrapBoxBestFit;
	for (unsigned int i = 0; i < sizeof(usedWrapBoxBestFit); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &modifiedNetStretch;
	for (unsigned int i = 0; i < sizeof(modifiedNetStretch); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = name;
	for (unsigned int i = 0; i < _MaxNameLength_ + 1; i++)
	{
		*pChar++ = buffer[j++];
	}
	trayName = name;
}

EvolWrapperPackageDesc::EvolWrapperPackageDesc(EvolWrapperPackageDesc *ewpd) : WrapperGeneric(WrEvolPackageDesc)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("EvolWrapperPackageDesc", "Constructor(3)");
	setId(ewpd->getId());
	setLength(ewpd->getLength());
	setWidth(ewpd->getWidth());
	setHeight(ewpd->getHeight());
	setLip(ewpd->getLip());
#ifdef NEW_FILM_WIDTH_VAR
	setFilmWidthSelection(ewpd->getFilmWidthSelection());
#else
	setFilmWidthUsedToWrap(ewpd->getFilmWidthUsedToWrap());
#endif
	setWrapBoxIndex(ewpd->getWrapBoxIndex());
	setHeightBoxIndex(ewpd->getHeightBoxIndex());
	setUsedWrapBoxBestFit(ewpd->hasUsedWrapBoxBestFit());
	setModifiedNetStretch(ewpd->hasModifiedNetStretch());
	setTrayName(ewpd->getTrayName());
}

EvolWrapperPackageDesc::~EvolWrapperPackageDesc()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
 
const char *EvolWrapperPackageDesc::serialize()const
{
	char boxName[_MaxNameLength_ + 1];
	WrapperGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &id;
	for (unsigned int i = 0; i < sizeof(id); i++)
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

	pChar = (char *) &height;
	for (unsigned int i = 0; i < sizeof(height); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &lip;
	for (unsigned int i = 0; i < sizeof(lip); i++)
	{
		serializedData.append(1, pChar[i]);
	}

#ifdef NEW_FILM_WIDTH_VAR
	pChar = (char *) &filmWidthSelection;
	for (unsigned int i = 0; i < sizeof(filmWidthSelection); i++)
#else
	pChar = (char *) &filmWidthUsedToWrap;
	for (unsigned int i = 0; i < sizeof(filmWidthUsedToWrap); i++)
#endif
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &wrapBoxIndex;
	for (unsigned int i = 0; i < sizeof(wrapBoxIndex); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &heightBoxIndex;
	for (unsigned int i = 0; i < sizeof(heightBoxIndex); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &usedWrapBoxBestFit;
	for (unsigned int i = 0; i < sizeof(usedWrapBoxBestFit); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &modifiedNetStretch;
	for (unsigned int i = 0; i < sizeof(modifiedNetStretch); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	strcpy(boxName, trayName.c_str());
	pChar = boxName;
	for (unsigned int i = 0; i < _MaxNameLength_ + 1; i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void EvolWrapperPackageDesc::show()
{
	if (Debug::isTracing("EvolWrapperPackageDesc"))
	{
		Debug::trace("EvolWrapperPackageDesc", "EvolWrapperPackageDesc: ");
		Debug::trace("EvolWrapperPackageDesc", HobString("                 id: ") + HobString(Integer::toString(id)));
		Debug::trace("EvolWrapperPackageDesc", HobString("             length: ") + HobString(Integer::toString(length)));
		Debug::trace("EvolWrapperPackageDesc", HobString("              width: ") + HobString(Integer::toString(width)));
		Debug::trace("EvolWrapperPackageDesc", HobString("             height: ") + HobString(Integer::toString(height)));
		Debug::trace("EvolWrapperPackageDesc", HobString("                lip: ") + HobString(Integer::toString(lip)));
#ifdef NEW_FILM_WIDTH_VAR
		Debug::trace("EvolWrapperPackageDesc", HobString(" filmWidthSelection: ") + HobString(Integer::toString(filmWidthSelection)));
#else
		Debug::trace("EvolWrapperPackageDesc", HobString("filmWidthUsedToWrap: ") + HobString(Integer::toString(filmWidthUsedToWrap)));
#endif
		Debug::trace("EvolWrapperPackageDesc", HobString("       wrapBoxIndex: ") + HobString(Integer::toString(wrapBoxIndex)));
		Debug::trace("EvolWrapperPackageDesc", HobString("     heightBoxIndex: ") + HobString(Integer::toString(heightBoxIndex)));
		Debug::trace("EvolWrapperPackageDesc", HobString(" usedWrapBoxBestFit: ") + HobString(Bool::toString(usedWrapBoxBestFit)));
		Debug::trace("EvolWrapperPackageDesc", HobString(" modifiedNetStretch: ") + HobString(Bool::toString(modifiedNetStretch)));
	}
}


UINT16 EvolWrapperPackageDesc::getId() const
{
	return id;
}
void EvolWrapperPackageDesc::setId(UINT16 id)
{
	this->id = id;
}

UINT16 EvolWrapperPackageDesc::getLength() const
{
	return length;
}
void EvolWrapperPackageDesc::setLength(UINT16 value)
{
	length = value;
}

UINT16 EvolWrapperPackageDesc::getWidth() const
{
	return width;
}
void EvolWrapperPackageDesc::setWidth(UINT16 value)
{
	width = value;
}

UINT16 EvolWrapperPackageDesc::getHeight() const
{
	return height;
}
void EvolWrapperPackageDesc::setHeight(UINT16 value)
{
	height = value;
}

UINT16 EvolWrapperPackageDesc::getLip() const
{
	return lip;
}
void EvolWrapperPackageDesc::setLip(UINT16 value)
{
	lip = value;
}

#ifdef NEW_FILM_WIDTH_VAR
WrFilmWidthSelection EvolWrapperPackageDesc::getFilmWidthSelection() const
{
	return filmWidthSelection;
}
void EvolWrapperPackageDesc::setFilmWidthSelection(WrFilmWidthSelection value)
{
	filmWidthSelection = value;
}
#else
UINT16 EvolWrapperPackageDesc::getFilmWidthUsedToWrap() const
{
	return filmWidthUsedToWrap;
}
void EvolWrapperPackageDesc::setFilmWidthUsedToWrap(UINT16 value)
{
	filmWidthUsedToWrap = value;
}
#endif

UINT16 EvolWrapperPackageDesc::getWrapBoxIndex() const
{
	return wrapBoxIndex;
}
void EvolWrapperPackageDesc::setWrapBoxIndex(UINT16 value)
{
	wrapBoxIndex = value;
}

UINT16 EvolWrapperPackageDesc::getHeightBoxIndex() const
{
	return heightBoxIndex;
}
void EvolWrapperPackageDesc::setHeightBoxIndex(UINT16 value)
{
	heightBoxIndex = value;
}

std::string & EvolWrapperPackageDesc::getTrayName()
{
	return trayName;
}

const std::string & EvolWrapperPackageDesc::getTrayName()const
{
	return trayName;
}

void EvolWrapperPackageDesc::setTrayName(std::string & name)
{
	trayName = name;
}

bool EvolWrapperPackageDesc::hasUsedWrapBoxBestFit() const
{
	return usedWrapBoxBestFit;
}
void EvolWrapperPackageDesc::setUsedWrapBoxBestFit(bool value)
{
	usedWrapBoxBestFit = value;
}

bool EvolWrapperPackageDesc::hasModifiedNetStretch() const
{
	return modifiedNetStretch;
}
void EvolWrapperPackageDesc::setModifiedNetStretch(bool value)
{
	modifiedNetStretch = value;
}

