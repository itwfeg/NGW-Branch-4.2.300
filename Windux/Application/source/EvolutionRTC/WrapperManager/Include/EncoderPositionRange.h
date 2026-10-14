#ifndef ENCODERPOSITIONRANGE_H
#define ENCODERPOSITIONRANGE_H

#include "PAL_platformheader.h"
class EncoderPositionRange
{
	public:
		EncoderPositionRange(UINT16 lb = 0, UINT16 ub = 360);
		~EncoderPositionRange();

		void show();
		bool isEncoderInRange(UINT16 ec);

	private:
		UINT16 lowerBound;
		UINT16 upperBound;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif // ENCODERPOSITIONRANGE_H
