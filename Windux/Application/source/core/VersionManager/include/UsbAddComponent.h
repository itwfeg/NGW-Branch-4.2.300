#ifndef USBADDCOMPONENT_H
#define USBADDCOMPONENT_H
#include "EvolMessage.h"
//#include "usbdLib.h"
#include "evolution.h"
#include "CEvolMsgReceiver.h"
//class HobThread;
class HobVersion;

class UsbAddComponent : public EvolMessage
{
	public:
		UsbAddComponent(USBD_NODE_ID nodeId,
						const HobVersion &hardwareVersion,
						Hobart::CEvolMsgReceiver *pHobThread = NULL);
		UsbAddComponent(const char *buffer);
		~UsbAddComponent();

		const char *serialize() const;
		void show();

		USBD_NODE_ID getNodeId() const {return nodeId_;}
		void setNodeId(USBD_NODE_ID nodeId) {nodeId_ = nodeId;}

		const HobVersion *getHardwareVersion() const; 
		void setHardwareVersion(HobVersion &hardwareVer_);

	private:
		USBD_NODE_ID nodeId_;
		HobVersion *hardwareVer_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//USBADDCOMPONENT_H
