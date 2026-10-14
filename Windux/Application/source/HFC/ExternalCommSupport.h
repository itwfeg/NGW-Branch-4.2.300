/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef EXTERNALCOMMSUPPORT_H
#define EXTERNALCOMMSUPPORT_H

#include "evolution.h"
#include "HfcObject.h"

class Configuration;

typedef enum ExternalCommSupportOffsets
{
	fieldOffsetExternalCommSetting = 1,
	fieldOffsetServerScaleIpAddress,
	fieldOffsetServerScaleHostName,
	fieldOffsetServerAppIpAddress,
	fieldOffsetServerAppHostName,

	endExternalCommSupportOffsets
} ExternalCommSupportOffsets_t;

class ExternalCommSupport : public HfcObject
{
	public:

		ExternalCommSupport();
		ExternalCommSupport(const char *data, int len, int formatType);

		~ExternalCommSupport();

		const char *serialize(int format) const;


	private:

		long	junkParameter;	//	So I can see all of the private parameters.

		Configuration *scaleConfig;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif // EXTERNALCOMMSUPPORT_H
