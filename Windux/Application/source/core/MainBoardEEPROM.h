/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef MAINBOARDEEPROM_H
#define MAINBOARDEEPROM_H

#include "Smbus.h"
#include "evolution.h"

class HobVersion;

class MainBoardEEPROM: public Smbus
{
	public:

		#define DEVICE_ADDRESS 0x2e
		#define OFFSET_BOARD_TYPE 0x00
		#define OFFSET_HARDWARE_MAJOR_REV 0x02
		#define OFFSET_HARDWARE_MINOR_REV 0x04
		#define OFFSET_MODEL_TYPE 0x06
		#define OFFSET_SERIAL_NUMBER 0x08

		~MainBoardEEPROM();

		static MainBoardEEPROM *getInstance();

		SmbusStatus readBoardType(USHORT * boardType);
		SmbusStatus readHardwareMajorRev(USHORT * hardwareMajorRev);
		SmbusStatus readHardwareMinorRev(USHORT * hardwareMinorRev);
		SmbusStatus readModelType(USHORT * modelType);
		SmbusStatus readSerialNumber(ULONG * serialNumber);
		SmbusStatus readHardwareRevision(HobVersion & hardwareRev);

		SmbusStatus writeBoardType(USHORT boardType);
		SmbusStatus writeHardwareMajorRev(USHORT hardwareMajorRev);
		SmbusStatus writeHardwareMinorRev(USHORT hardwareMinorRev);
		SmbusStatus writeModelType(USHORT modelType);
		SmbusStatus writeSerialNumber(ULONG serialNumber);
		SmbusStatus writeHardwareRevision(HobVersion & hardwareRev);

	private:

		MainBoardEEPROM();

		static MainBoardEEPROM *instance;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //MAINBOARDEEPROM_H




