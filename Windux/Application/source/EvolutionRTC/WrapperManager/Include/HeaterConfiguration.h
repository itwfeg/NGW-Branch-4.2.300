#ifndef HEATERCONFIGURATION_H
#define HEATERCONFIGURATION_H

#include <string>

#include "Wrapper.h"

class HeaterConfiguration
{
	public:
		HeaterConfiguration(bool isUsed = false,
						  INT16 maxTemp = 0,
						  INT16 minTemp = 0,
						  UINT16 defaultTempSetting = 0);

		HeaterConfiguration(const char *buffer);
		HeaterConfiguration(const HeaterConfiguration &hc);
		~HeaterConfiguration();

		const char *serialize() const;
		UINT32 getSerializedLength() const;

		void show();

		bool isItUsed() const;
		void setIsUsed(bool value = true);
		void clearIsUsed();
		bool getIsUsed() { return isUsed; }

		INT16 getMaxTemp() const;
		void setMaxTemp(INT16 temp);

		INT16 getMinTemp() const;
		void setMinTemp(INT16 temp);

		UINT16 getDefaultTempSetting() const;
		void setDefaultTempSetting(UINT16 temp);

		void operator=(const HeaterConfiguration &heaterConfig);


	private:
		bool	isUsed;
		INT16	maxTemp;
		INT16	minTemp;
	    UINT16	defaultTempSetting;

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif // HEATERCONFIGURATION_H
