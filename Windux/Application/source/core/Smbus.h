/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SMBUS_H
#define SMBUS_H

#include "evolution.h"
#if defined( __linux__ )
    #include "PAL_typedef.h"
#endif
//#include <setupapi.h>   // replaced by PAL header


class Smbus
{
	public:

		typedef enum SmbusStatus
		{
			STATUS_SUCCEEDED,
			STATUS_FAILED,
			STATUS_FILE_NOT_OPEN,
			STATUS_INVALID_PARAM // MSVC2010
		} SmbusStatus_t;

	protected:

		Smbus(UCHAR devAddr);
		~Smbus();


		SmbusStatus readByte(UCHAR * readData);
		SmbusStatus readDataByte(UCHAR command, UCHAR * readData);
		SmbusStatus readDataWord(UCHAR command, UCHAR * readData);
		SmbusStatus readDataBlock(UCHAR command, UCHAR length, UCHAR * readData);
		SmbusStatus writeByte(UCHAR command);
		SmbusStatus writeDataByte(UCHAR command, UCHAR writeData);
		SmbusStatus writeDataWord(UCHAR command, UCHAR * writeData);
		SmbusStatus writeDataBlock(UCHAR command, UCHAR length, UCHAR * writeData);


	private:

		bool openHandle();
		void closeHandle();

#if defined( PAL_WIN32_RUN )
		PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData_;
		HANDLE file_;
#else
		int file_;
#endif
		bool fileOpened_;
		UCHAR deviceAddress_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //SMBUS_H




