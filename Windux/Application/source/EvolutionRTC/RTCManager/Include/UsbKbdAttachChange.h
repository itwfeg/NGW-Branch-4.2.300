#ifndef USBKBDATTACHCHANGE_H
#define USBKBDATTACHCHANGE_H
#include "EvolMessage.h"

//#include "drv/usb/usbKeyboardLib.h"


class UsbKbdAttachChange : public EvolMessage
{
	public:
		UsbKbdAttachChange(int attachStatus, Hobart::CEvolMsgReceiver *pHobThread = NULL);
		UsbKbdAttachChange(const char *buffer);
		~UsbKbdAttachChange();

		int getAttachStatus() const {return attachStatus_;}

		const char *serialize() const;
		void show();

	private:
		int attachStatus_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif	//USBKBDATTACHCHANGE_H
