/*
 * KPCBootVersion.cpp
 *
 *  Created on: Mar 15, 2019
 *      Author: hobart
 */

#include "KPCBootVersion.h"
#include "KpcBootEvent.h"
#include "HobString.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootVersion");
#endif

KPCBootVersion::KPCBootVersion(const KPCBootVersion &newVersion) : EvolMessage(newVersion)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	bootVerMajor = newVersion.getBootVerMajor();
	bootVerBuild = newVersion.getBootVerBuild();
	bootVerMinor = newVersion.getBootVerMinor();
}

KPCBootVersion::KPCBootVersion(const char *buffer) : EvolMessage(buffer)
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
	char * pChar = (char *)&bootVerMajor;
	for (unsigned int i = 0; i < sizeof(bootVerMajor); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *)&bootVerMinor;
	for (unsigned int i = 0; i < sizeof(bootVerMinor); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *)&bootVerBuild;
	for (unsigned int i = 0; i < sizeof(bootVerBuild); i++)
	{
		*pChar++ = buffer[j++];
	}
}

KPCBootVersion::KPCBootVersion(EvolDeviceType type, Hobart::CEvolMsgReceiver *pHobThread): EvolMessage(type, KpcBootVersion, pHobThread){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bootVerMajor = 0;
	bootVerMinor = 0;
	bootVerBuild = 0;
}


KPCBootVersion::KPCBootVersion(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(KPCBootloader, KpcBootVersion, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bootVerMajor = 0;
	bootVerMinor = 0;
	bootVerBuild = 0;
}

KPCBootVersion::~KPCBootVersion()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootVersion::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char * pChar = (char *)&bootVerMajor;
	for (unsigned int i = 0; i < sizeof(bootVerMajor); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&bootVerMinor;
	for (unsigned int i = 0; i < sizeof(bootVerMinor); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&bootVerBuild;
	for (unsigned int i = 0; i < sizeof(bootVerBuild); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void KPCBootVersion::show()
{
	bool isTracing = Debug::isTracing("KPCBootVersion");

	if (isTracing == false)
	{
		return;
	}
	Debug::trace("KPCBootVersion", "KPCBootVersion: ");

	EvolMessage::show();

	Debug::trace("KPCBootVersion",
				 HobString("Kpc Boot Version Major: ") + Integer::toString(bootVerMajor));
	Debug::trace("KPCBootVersion",
				 HobString("Kpc Boot Version Minor: ") + Integer::toString(bootVerMinor));
	Debug::trace("KPCBootVersion",
				 HobString("Kpc Boot Version Build: ") + Integer::toString(bootVerBuild));

	Debug::trace("KPCBootVersion",
				 HobString("Kpc Boot Version: ") + getBootVersionString());

	if (isTracing == false)
	{
		Debug::removeTraceCategory("KPCBootVersion");
	}
}

bool KPCBootVersion::equals(const KPCBootVersion &kpcBootCmd) const
{
	return !(getBootVersionString().compare(kpcBootCmd.getBootVersionString()));
}

unsigned int KPCBootVersion::getBootVerBuild() const
{
	return bootVerBuild;
}

void KPCBootVersion::setBootVerBuild(unsigned int bootVerBuild)
{
	this->bootVerBuild = bootVerBuild;
}

unsigned int KPCBootVersion::getBootVerMajor() const
{
	return bootVerMajor;
}

void KPCBootVersion::setBootVerMajor(unsigned int bootVerMajor)
{
	this->bootVerMajor = bootVerMajor;
}

unsigned int KPCBootVersion::getBootVerMinor() const
{
	return bootVerMinor;
}

void KPCBootVersion::setBootVerMinor(unsigned int bootVerMinor)
{
	this->bootVerMinor = bootVerMinor;
}

std::string KPCBootVersion::getBootVersionString() const
{
	return Integer::toString(bootVerMajor) + "." + Integer::toString(bootVerMinor) + "." +
		   Integer::toString(bootVerBuild);
}
