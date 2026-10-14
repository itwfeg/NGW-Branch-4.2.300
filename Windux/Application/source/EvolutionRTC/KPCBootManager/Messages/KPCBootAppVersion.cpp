/*
 * KPCBootAppVersion.cpp
 *
 *  Created on: Mar 15, 2019
 *      Author: hobart
 */

#include "KPCBootAppVersion.h"
#include "KpcBootEvent.h"
#include "HobString.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootAppVersion");
#endif

KPCBootAppVersion::KPCBootAppVersion(const KPCBootAppVersion& newVersion) : EvolMessage(newVersion){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	printerAppVerMajor = newVersion.getPrinterAppVerMajor();
	printerAppVerMinor = newVersion.getPrinterAppVerMinor();
	printerAppVerBuild = newVersion.getPrinterAppVerBuild();

	weigherAppVerMajor = newVersion.getWeigherAppVerMajor();
	weigherAppVerBuild = newVersion.getWeigherAppVerBuild();
	weigherAppVerMinor = newVersion.getWeigherAppVerMinor();
}

KPCBootAppVersion::KPCBootAppVersion(const char* buffer) : EvolMessage(buffer){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &printerAppVerMajor;
	for (unsigned int i = 0; i < sizeof(printerAppVerMajor); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &printerAppVerMinor;
	for (unsigned int i = 0; i < sizeof(printerAppVerMinor); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &printerAppVerBuild;
	for (unsigned int i = 0; i < sizeof(printerAppVerBuild); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &weigherAppVerMajor;
	for (unsigned int i = 0; i < sizeof(weigherAppVerMajor); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &weigherAppVerMinor;
	for (unsigned int i = 0; i < sizeof(weigherAppVerMinor); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &weigherAppVerBuild;
	for (unsigned int i = 0; i < sizeof(weigherAppVerBuild); i++)
	{
		*pChar++ = buffer[j++];
	}
}

KPCBootAppVersion::KPCBootAppVersion(EvolDeviceType type, Hobart::CEvolMsgReceiver* pHobThread) : EvolMessage( type, KpcBootAppVersion, pHobThread ){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	printerAppVerMajor = 0;
	printerAppVerMinor = 0;
	printerAppVerBuild = 0;

	weigherAppVerMajor = 0;
	weigherAppVerMinor = 0;
	weigherAppVerBuild = 0;
}

KPCBootAppVersion::KPCBootAppVersion(Hobart::CEvolMsgReceiver* pHobThread) : EvolMessage( KPCBootloader, KpcBootAppVersion, pHobThread ){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	printerAppVerMajor = 0;
	printerAppVerMinor = 0;
	printerAppVerBuild = 0;

	weigherAppVerMajor = 0;
	weigherAppVerMinor = 0;
	weigherAppVerBuild = 0;
}

KPCBootAppVersion::~KPCBootAppVersion() {
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char* KPCBootAppVersion::serialize() const {
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &printerAppVerMajor;
	for (unsigned int i = 0; i < sizeof(printerAppVerMajor); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &printerAppVerMinor;
	for (unsigned int i = 0; i < sizeof(printerAppVerMinor); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &printerAppVerBuild;
	for (unsigned int i = 0; i < sizeof(printerAppVerBuild); i++) {
		serializedData.append(1, pChar[i]);
	}


	pChar = (char *) &weigherAppVerMajor;
	for (unsigned int i = 0; i < sizeof(weigherAppVerMajor); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &weigherAppVerMinor;
	for (unsigned int i = 0; i < sizeof(weigherAppVerMinor); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &weigherAppVerBuild;
	for (unsigned int i = 0; i < sizeof(weigherAppVerBuild); i++) {
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void KPCBootAppVersion::show() const {
	bool isTracing = Debug::isTracing("KPCBootAppVersion");

		if (isTracing == false) {
			return;
		}
		Debug::trace("KPCBootAppVersion", "KPCBootAppVersion: ");

		EvolMessage::show();

		Debug::trace("KPCBootAppVersion",
						HobString("Kpc Printer App Version Major: ") + Integer::toString(printerAppVerMajor));
		Debug::trace("KPCBootAppVersion",
						HobString("Kpc Printer App Version Minor: ") + Integer::toString(printerAppVerMinor));
		Debug::trace("KPCBootAppVersion",
						HobString("Kpc Printer App Version Build: ") + Integer::toString(printerAppVerBuild));

		Debug::trace("KPCBootAppVersion",
						HobString("Kpc Weigher App Version Major: ") + Integer::toString(weigherAppVerMajor));
		Debug::trace("KPCBootAppVersion",
						HobString("Kpc Weigher App Version Minor: ") + Integer::toString(weigherAppVerMinor));
		Debug::trace("KPCBootAppVersion",
						HobString("Kpc Weigher App Version Build: ") + Integer::toString(weigherAppVerBuild));
		Debug::trace("KPCBootAppVersion",
				HobString("Kpc Printer App Version: ") + getPrinterAppVersionString());
		Debug::trace("KPCBootAppVersion",
				HobString("Kpc Weigher App Version: ") + getWeigherAppVersionString());

		if (isTracing == false) {
			Debug::removeTraceCategory("KPCBootReady");
		}
}

bool KPCBootAppVersion::equals(const KPCBootAppVersion& kpcBootCmd) const {
	return !( getPrinterAppVersionString().compare(kpcBootCmd.getPrinterAppVersionString()) ||
			  getWeigherAppVersionString().compare(kpcBootCmd.getWeigherAppVersionString())
	);
}

unsigned int KPCBootAppVersion::getPrinterAppVerBuild() const {
	return printerAppVerBuild;
}

void KPCBootAppVersion::setPrinterAppVerBuild(unsigned int printerAppVerBuild) {
	this->printerAppVerBuild = printerAppVerBuild;
}

unsigned int KPCBootAppVersion::getPrinterAppVerMajor() const {
	return printerAppVerMajor;
}

void KPCBootAppVersion::setPrinterAppVerMajor(unsigned int printerAppVerMajor) {
	this->printerAppVerMajor = printerAppVerMajor;
}

unsigned int KPCBootAppVersion::getPrinterAppVerMinor() const {
	return printerAppVerMinor;
}

void KPCBootAppVersion::setPrinterAppVerMinor(unsigned int printerAppVerMinor) {
	this->printerAppVerMinor = printerAppVerMinor;
}

unsigned int KPCBootAppVersion::getWeigherAppVerBuild() const {
	return weigherAppVerBuild;
}

void KPCBootAppVersion::setWeigherAppVerBuild(unsigned int weigherAppVerBuild) {
	this->weigherAppVerBuild = weigherAppVerBuild;
}

unsigned int KPCBootAppVersion::getWeigherAppVerMajor() const {
	return weigherAppVerMajor;
}

void KPCBootAppVersion::setWeigherAppVerMajor(unsigned int weigherAppVerMajor) {
	this->weigherAppVerMajor = weigherAppVerMajor;
}

unsigned int KPCBootAppVersion::getWeigherAppVerMinor() const {
	return weigherAppVerMinor;
}

void KPCBootAppVersion::setWeigherAppVerMinor(unsigned int weigherAppVerMinor) {
	this->weigherAppVerMinor = weigherAppVerMinor;
}

std::string KPCBootAppVersion::getPrinterAppVersionString() const{
	return Integer::toString(printerAppVerMajor) + "." + Integer::toString(printerAppVerMinor) +"." + \
			Integer::toString(printerAppVerBuild);
}

std::string KPCBootAppVersion::getWeigherAppVersionString() const{
	return Integer::toString(weigherAppVerMajor) + "." + Integer::toString(weigherAppVerMinor) +"." + \
			Integer::toString(weigherAppVerBuild);
}
