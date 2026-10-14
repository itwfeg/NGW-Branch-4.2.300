/*
 * KPCBootVersion.h
 *
 *  Created on: Mar 15, 2019
 *      Author: hobart
 */

#ifndef KPCBOOTAPPVERSION_H_
#define KPCBOOTAPPVERSION_H_

#include "EvolMessage.h"
#include "Integer.h"

class KPCBootAppVersion: public EvolMessage {
public:
	KPCBootAppVersion(const KPCBootAppVersion& newVersion);
	KPCBootAppVersion(const char *buffer);
	KPCBootAppVersion(Hobart::CEvolMsgReceiver *pHobThread);
	KPCBootAppVersion(EvolDeviceType type, Hobart::CEvolMsgReceiver* pHobThread);
	~KPCBootAppVersion();

	const char *serialize() const;

	void show() const;

	bool equals(const KPCBootAppVersion &kpcBootCmd) const;

	unsigned int getWeigherAppVerBuild() const;
	void setWeigherAppVerBuild(unsigned int appVerBuild);
	unsigned int getWeigherAppVerMajor() const;
	void setWeigherAppVerMajor(unsigned int appVerMajor);
	unsigned int getWeigherAppVerMinor() const;
	void setWeigherAppVerMinor(unsigned int appVerMinor);

	unsigned int getPrinterAppVerBuild() const;
	void setPrinterAppVerBuild(unsigned int appVerBuild);
	unsigned int getPrinterAppVerMajor() const;
	void setPrinterAppVerMajor(unsigned int appVerMajor);
	unsigned int getPrinterAppVerMinor() const;
	void setPrinterAppVerMinor(unsigned int appVerMinor);



	std::string getPrinterAppVersionString() const;
	std::string getWeigherAppVersionString() const;

private:
	unsigned int printerAppVerMajor;
	unsigned int printerAppVerMinor;
	unsigned int printerAppVerBuild;

	unsigned int weigherAppVerMajor;
	unsigned int weigherAppVerMinor;
	unsigned int weigherAppVerBuild;


};

#endif /* KPCBOOTAPPVERSION_H_ */
