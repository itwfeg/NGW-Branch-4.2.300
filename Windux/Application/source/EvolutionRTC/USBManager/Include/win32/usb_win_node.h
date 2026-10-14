#ifndef USB_WIN_NODE_H
#define USB_WIN_NODE_H

#include "evolution.h"
#include "SUsbDescriptors.h"
#include "UsbNode.h"
#include <setupapi.h>

#include "device_event.h"


class CUsbWinNode: CUsbNode
{
	public:

		CUsbWinNode(){}
		~CUsbWinNode(){}

#ifdef LINUX_FIX
        virtual USBD_NODE_ID Initialize(DEVICE_INFO dev_i);
#else
        USBD_NODE_ID 
        Initialize(
            HDEVINFO hDevInfo, 
            SP_DEVICE_INTERFACE_DATA& devIntfcData,
            SP_DEVINFO_DATA& devInfoData
            );
#endif

	private:

        friend class UsbManager;

#ifdef LINUX_FIX
		friend class UsbManagerWin32;
#endif

};


#endif  // USB_WIN_NODE_H
