#ifndef WRAPPERHEATERINFO_H
#define WRAPPERHEATERINFO_H

#include <string>

#include "Wrapper.h"

class WrapperHeaterInfo
{
	public:
		WrapperHeaterInfo(bool state = false,
						  UINT16 setting = 0,
						  bool celsius = false,
						  UINT16 temperature = 0);
		WrapperHeaterInfo(const char *buffer);
		~WrapperHeaterInfo();

		const char *serialize() const;
		UINT32 getSerializedLength() const;

		void show();

		bool isState() const;
		void setState(bool value = true);
		void clearState();

		void setSetting(UINT16 setting);
		UINT16 getSetting() const;

		bool isCelsius() const;
		void setCelsius(bool value = true);
		void clearCelsius();

		void setTemperature(UINT16 temperature);
		UINT16 getTemperature() const;

		void setHeaterBits(WrHeaterBits heaterBits);
		WrHeaterBits getHeaterBits() const;

		bool equals(const WrapperHeaterInfo &wrHeaterInfo);

	private:
		bool	state;
		UINT16	setting;
		bool	celsius;
	    UINT16	temperature;

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};
#endif // WRAPPERHEATERINFO_H
