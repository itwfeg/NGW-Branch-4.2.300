/*
 * KPCBootVersion.h
 *
 *  Created on: Mar 15, 2019
 *      Author: hobart
 */

#ifndef KPCBOOTVERSION_H_
#define KPCBOOTVERSION_H_

#include "EvolMessage.h"
#include "Integer.h"

class KPCBootVersion : public EvolMessage
{
  public:
	KPCBootVersion(const KPCBootVersion &newVersion);
	KPCBootVersion(const char *buffer);
	KPCBootVersion(Hobart::CEvolMsgReceiver *pHobThread);
	KPCBootVersion(EvolDeviceType type, Hobart::CEvolMsgReceiver *pHobThread);
	~KPCBootVersion();

	const char *serialize() const;

	void show();

	bool equals(const KPCBootVersion &kpcBootCmd) const;

	unsigned int getBootVerBuild() const;
	void setBootVerBuild(unsigned int bootVerBuild);
	unsigned int getBootVerMajor() const;
	void setBootVerMajor(unsigned int bootVerMajor);
	unsigned int getBootVerMinor() const;
	void setBootVerMinor(unsigned int bootVerMinor);

	std::string getBootVersionString() const;

  private:
	unsigned int bootVerMajor;
	unsigned int bootVerMinor;
	unsigned int bootVerBuild;
};

#endif /* KPCBOOTVERSION_H_ */
