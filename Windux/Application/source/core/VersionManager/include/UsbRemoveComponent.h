#ifndef USBREMOVECOMPONENT_H
#define USBREMOVECOMPONENT_H
#include "EvolMessage.h"
//#include "usbdLib.h"
#include "CEvolMsgReceiver.h"
//class HobThread;
class HobVersion;


class UsbRemoveComponent : public EvolMessage
{
	public:
		UsbRemoveComponent(USBD_NODE_ID nodeId, Hobart::CEvolMsgReceiver *pHobThread = NULL);
		UsbRemoveComponent(const char *buffer);
		~UsbRemoveComponent();

		USBD_NODE_ID getNodeId(){return nodeId_;}

		const char *serialize() const;
		void show();

	private:
		USBD_NODE_ID nodeId_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif	//USBREMOVECOMPONENT_H
