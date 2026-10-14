#ifndef EVOLWRAPPERHEATERCONFIG_H
#define EVOLWRAPPERHEATERCONFIG_H

#include <string>

#include "Wrapper.h"
#include "HeaterConfiguration.h"


class EvolWrapperHeaterConfig
{
	public:
		EvolWrapperHeaterConfig();
		EvolWrapperHeaterConfig(const char *buffer);
		EvolWrapperHeaterConfig(const EvolWrapperHeaterConfig &ewhc);
		~EvolWrapperHeaterConfig();

		const char *serialize() const;
		UINT32 getSerializedLength() const;

		void show();

		ULONG getChecksum() const;
		void setChecksum(ULONG checksum);

		bool getFilmHeaterIsUsed() const;

		HeaterConfiguration &getFilmHeaterConfig();
		const HeaterConfiguration &getFilmHeaterConfig()const;

		HeaterConfiguration &getSealerHeaterConfig();
		const HeaterConfiguration &getSealerHeaterConfig()const;

		void operator=(const EvolWrapperHeaterConfig &ewhc);

		void setFilmHeaterIsUsed(bool value){ filmHeater.setIsUsed(value); }
		void setFilmHeaterMaxTemp(INT16 temp){ filmHeater.setMaxTemp(temp); }
		void setFilmHeaterMinTemp(INT16 temp){ filmHeater.setMinTemp(temp); }
		void setFilmHeaterDefaultTemp(INT16 temp){ filmHeater.setDefaultTempSetting(temp); }
		void setSealerHeaterIsUsed(bool value){ sealerHeater.setIsUsed(value); }
		void setSealerHeaterMaxTemp(INT16 temp){ sealerHeater.setMaxTemp(temp); }
		void setSealerHeaterMinTemp(INT16 temp){ sealerHeater.setMinTemp(temp); }
		void setSealerHeaterDefaultTemp(INT16 temp) { sealerHeater.setDefaultTempSetting(temp); }

	private:
	    ULONG				checksum;
		bool                isItUsed;
		HeaterConfiguration	filmHeater;
		HeaterConfiguration	sealerHeater;

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif // EVOLWRAPPERHEATERCONFIG_H
