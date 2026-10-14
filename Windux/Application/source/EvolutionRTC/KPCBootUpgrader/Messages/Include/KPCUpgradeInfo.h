/*
 * KPCUpgradeInfo.h
 *
 *  Created on: Mar 14, 2019
 *      Author: Eric Landes
 */

#ifndef KPCUPGRADEINFO_H_
#define KPCUPGRADEINFO_H_
#include "EvolMessage.h"
#include "KpcBootUpgraderEvent.h"
#include "Integer.h"
#include <string>

class KPCUpgradeInfo: public EvolMessage {

public:
	KPCUpgradeInfo(const KPCUpgradeInfo&);
	KPCUpgradeInfo(const char *buffer);
	KPCUpgradeInfo(EvolDeviceType deviceType, Hobart::CEvolMsgReceiver * pSource);
	~KPCUpgradeInfo();

	const char *serialize() const;

	void show();

    std::string getCurrentVersion() const;
    std::string getAvailableVersion() const;

    void setCurrentVersion(std::string ver);
    void setAvailableVersion(std::string ver );

private:
	KPCUpgradeInfo();
    std::string currentVersion;
    std::string availableVersion;

#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif
};

#endif /* KPCUPGRADEINFO_H_ */
