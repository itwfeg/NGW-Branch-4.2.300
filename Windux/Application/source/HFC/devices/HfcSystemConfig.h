/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCSYSTEMCONFIG_H
#define HFCSYSTEMCONFIG_H

#include "evolution.h"
#include "SystemConfiguration.h"
#include "HfcObject.h"

typedef enum HfcSystemConfigFieldOffsets
{
	fieldOffsetSysConfigType = 1,
	fieldOffsetDeviceSignature,
	fieldOffsetStationAddress,
	fieldOffsetControllerAddress,

	endHfcSystemConfigFieldOffsets	//	MUST be last!
}   HfcSystemConfigFieldOffsets_t;

class HfcSystemConfig : public SystemConfiguration, public HfcObject
{
	public:
		HfcSystemConfig();
		HfcSystemConfig(const char *data);
		HfcSystemConfig(const char *data, int len, int formatType);

		~HfcSystemConfig();

		virtual const char *serialize(int format = InternalFormat) const;
		UINT32 getSerializedLength() const;
		
		void update(HfcSystemConfig *weigherSystemConfig);


	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCSYSTEMCONFIG_H
