#ifndef PERIPHERALREQUESTCODEUPDATE_H
#define PERIPHERALREQUESTCODEUPDATE_H
#include "EvolMessage.h"
//#include "usbdLib.h"
#include "evolution.h"
//class HobThread;
class HobVersion;

class PeripheralRequestCodeUpdate : public EvolMessage
{
	public:
		PeripheralRequestCodeUpdate(USBD_NODE_ID nodeId, const std::string &hardwareVersion, const std::string &softwareVersion = NULL, const int failureCnt = 0, Hobart::CEvolMsgReceiver *pHobThread = NULL);
		PeripheralRequestCodeUpdate(USBD_NODE_ID nodeId, const HobVersion &hardwareVersion, const HobVersion &softwareVersion, const int failureCnt = 0, Hobart::CEvolMsgReceiver *pHobThread = NULL);
		PeripheralRequestCodeUpdate(const char *buffer);
		~PeripheralRequestCodeUpdate();

		const HobVersion *getHardwareVersion() const; 
		void setHardwareVersion(HobVersion &hardwareVer_);

		const HobVersion *getSoftwareVersion() const;
		void setSoftwareVersion(HobVersion &softwareVer_);

		USBD_NODE_ID getNodeId(){return nodeId_;};
		void setNodeId(USBD_NODE_ID nodeId) {nodeId_ = nodeId;}

		int getFailureCount(){return failureCount_;}
		void setFailureCount(int cnt){failureCount_ = cnt;}
		
		const char *serialize() const;

		void show();

	private:
		USBD_NODE_ID nodeId_;
		HobVersion *hardwareVer_;
		HobVersion *softwareVer_;
		int failureCount_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//PERIPHERALREQUESTCODEUPDATE_H
