#ifndef EVOLWRAPPERCAMERAHEIGHTSCAN_H
#define EVOLWRAPPERCAMERAHEIGHTSCAN_H

#include <string>

#include "WrapperGeneric.h"
#include "Wrapper.h"


class EvolWrapperCameraHeightScan : public WrapperGeneric
{
	public:

		static const UINT16 Bit_050_Mask 		= 0x0001;
		static const UINT16 Bit_075_Mask 		= 0x0002;
		static const UINT16 Bit_100_Mask 		= 0x0004;
		static const UINT16 Bit_125_Mask 		= 0x0008;
		static const UINT16 Bit_150_Mask 		= 0x0010;
		static const UINT16 Bit_175_Mask 		= 0x0020;
		static const UINT16 Bit_200_Mask 		= 0x0040;
		static const UINT16 Bit_225_Mask 		= 0x0080;
		static const UINT16 Bit_250_Mask 		= 0x0100;
		static const UINT16 Bit_300_Mask 		= 0x0200;
		static const UINT16 Bit_350_Mask 		= 0x0400;
		static const UINT16 Bit_400_Mask 		= 0x0800;
		static const UINT16 Bit_450_Mask 		= 0x1000;
		static const UINT16 Bit_500_Mask 		= 0x2000;
		static const UINT16 Bit_600_Mask 		= 0x4000;

		EvolWrapperCameraHeightScan(Hobart::CEvolMsgReceiver *pHobThread = 0);
		EvolWrapperCameraHeightScan(const char *buffer);
		~EvolWrapperCameraHeightScan();

		const char *serialize()const;
		void show();

		UINT16 getScan() const;
		void setScan(UINT16 value);

		bool equals(EvolWrapperCameraHeightScan &wrScan);

	private:

		UINT16 				scan;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif // EVOLWRAPPERCAMERAHEIGHTSCAN_H
