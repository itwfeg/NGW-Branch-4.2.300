/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef WEIGHERINFO_H
#define WEIGHERINFO_H

#include "evolution.h"
#include "HfcObject.h"
#include "Configuration.h"
#include "HfcWeigherSystemConfig.h"

typedef enum WeigherInfoFieldOffsets
{
	fieldOffsetWeigherInfoAddress = 1,
	endWeigherInfoFieldOffsets	//	MUST be last!
}   WeigherInfoFieldOffsets_t;


class WeigherInfo : public HfcObject
{
	public:

		WeigherInfo();
		WeigherInfo(const char *data);
		WeigherInfo(const char *data, int len, int formatType);

		~WeigherInfo();

		HfcWeigherSystemConfig* getWeigherSystemConfig();
		void setWeigherSystemConfig(HfcWeigherSystemConfig *weigherSystemConfig);

		DeviceAddress getStationAddress() const;
		void setStationAddress(DeviceAddress stationAddress);

		virtual const char *serialize(int format = InternalFormat) const;

		void update(WeigherInfo *weigherInfo);


	private:

		HfcWeigherSystemConfig *weigherSystemConfig;

		DeviceAddress stationAddress;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//WEIGHERINFO_H
