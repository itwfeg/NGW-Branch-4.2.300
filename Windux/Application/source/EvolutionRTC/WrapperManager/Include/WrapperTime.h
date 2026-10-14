#ifndef WRAPPERTIME_H
#define WRAPPERTIME_H
#include "PAL_platformheader.h"
#include <string>

class WrapperTime
{
	public:
		WrapperTime(UINT8 min = 0,
					UINT8 value = 0,
					UINT8 max = 0);
		WrapperTime(const char *buffer);
		virtual ~WrapperTime();

		const char *serialize() const;
		UINT32 getSerializedLength() const;

		void show();

		UINT8 getMin() const;
		void setMin(UINT8 min);

		UINT8 getValue() const;
		void setValue(UINT8 value);

		UINT8 getMax() const;
		void setMax(UINT8 max);

	private:
		UINT8 min;
		UINT8 value;
		UINT8 max;

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};
#endif // WRAPPERTIME_H
