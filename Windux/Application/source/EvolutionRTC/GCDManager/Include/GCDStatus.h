#ifndef GCDSTATUS_H
#define GCDSTATUS_H

#include "SystemStatus.h"

class GcdSystemStatus : public SystemStatus {
	public:
		GcdSystemStatus(Hobart::CEvolMsgReceiver *pHobThread = NULL,
							SystemStatusInfo *systemStatusInfo = NULL,
							UINT16 softwareMajor = 0,
							UINT16 softwareMinor = 0);
		GcdStatus(const GcdStatus&);
		~GcdStatus();
		 
		const char *serialize() const;

		void show();	

		UINT16 getSoftwareVersionMajor() const;
		UINT16 getSoftwareVersionMinor() const;
		const HobVersion &getSoftwareVersion() const;
		void setSoftwareVersion(UINT16 major, UINT16 minor);
		void setSoftwareVersion(const HobVersion &softwareVersion);

private:
		HobVersion softwareVersion;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //GCDSTATUS_H
