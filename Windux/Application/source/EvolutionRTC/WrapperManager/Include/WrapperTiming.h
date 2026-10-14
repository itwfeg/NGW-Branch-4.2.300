#ifndef WRAPPERTIMING_H
#define WRAPPERTIMING_H

#include <string>

#include "WrapperTime.h"

class WrapperTiming
{
	public:
		WrapperTiming(UINT8 onMin = 0,
					  UINT8 onValue = 0,
					  UINT8 onMax = 0,
					  UINT8 offMin = 0,
					  UINT8 offValue = 0,
					  UINT8 offMax = 0);
		WrapperTiming(const char *buffer);
		virtual ~WrapperTiming();

		const char *serialize() const;
		UINT32 getSerializedLength() const;

		void show();

		UINT8 getOnMin() const;
		void setOnMin(UINT8 onMin);

		UINT8 getOnValue() const;
		void setOnValue(UINT8 onValue);

		UINT8 getOnMax() const;
		void setOnMax(UINT8 onMax);

		UINT8 getOffMin() const;
		void setOffMin(UINT8 offMin);

		UINT8 getOffValue() const;
		void setOffValue(UINT8 offValue);

		UINT8 getOffMax() const;
		void setOffMax(UINT8 offMax);

	private:
		WrapperTime timeOn;
		WrapperTime timeOff;

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};
#endif // WRAPPERTIMING_H
