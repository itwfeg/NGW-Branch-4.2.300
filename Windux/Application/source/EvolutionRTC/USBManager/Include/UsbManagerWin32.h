#ifndef USBMANAGERWIN32_H
#define USBMANAGERWIN32_H

#include "device_event.h"
#include "HIDDeviceEvent.h"

class UsbManagerWin32 : public UsbManager
{
public:
	UsbManagerWin32();
	~UsbManagerWin32();

	virtual bool readDescriptors( HANDLE handle, SUsbDescriptors * pDescs, bool bEzUsb );
	bool isSupportedClassGuid(GUID& guid);
	void HandleDeviceNotification(PEVENT_INFO ev_info, int32_t vid = 0, int32_t pid = 0);
    USBD_NODE_ID NodeIdFromDeviceName(TCHAR* devName);
    virtual SUsbDescriptors * getUsbDescs( USBD_NODE_ID nodeId );
        bool isBulkStorageDevicePresent() const;

	// SOF-2931
	bool isSambaPresent() const;
	void resetSamba();


#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif
protected:
	void UpdateDeviceMap();
	bool searchForKeyFiles();
};


#endif //USBMANAGERWIN32_H
