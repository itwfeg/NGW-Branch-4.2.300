#ifndef SYSTEMEVENT_H
#define SYSTEMEVENT_H

	#include "Event.h"

	// System Device on Ultima Peripherals - WeigherSystem, LabelApplierSystem, PrinterSystem, WrapperSystem
	typedef enum SysEvent
	{
		SysWakeup,
		SysConfig,
		SysInquire,
		SysReset,
		SysStatus,
		SysEnable,
		SysDisable,
		SysMode,
		SysRead,
		SysWrite,
		SysEEPBlockRead,
		SysEEPBlockWrite,
		SysReqWakeup,		// Request Wakeup message from Peripheral
		SysReqStatus,
		SysReqInquire,
		SysCalibSwitch = 17,
		SysFlashStatus,
		SysReqFlashStatus,
		SysUsbWgStatus,		// System Status for the USB Weigher
		SysUsbWrStatus,		// System Status for the USB Wrapper

		SysCfgGCD			= 0x13,
		SysGCDPwrOff		= 0x14,
		SysGCDPwrOn			= 0x15,
		
		SysPeripheralList = 100,
		SysReqPeripheralList
	} SysEvent_t;


#endif // SYSTEMEVENT_H
