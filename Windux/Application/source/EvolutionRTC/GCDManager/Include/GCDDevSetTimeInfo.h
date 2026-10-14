#ifndef GCDSETTIMEINFO_H
#define GCDSETTIMEINFO_H

#include "EvolMessage.h"
#include "MessageFormat.h"

class GCDDevSetTimeInfo : public EvolMessage
{
public:
	GCDDevSetTimeInfo(const PAL_PTIME_ZONE_STRUCT pTimeZone, const PAL_PTIME_STRUCT pLocalTime);

	UINT32 getSerializedLength() const;

	const char *serialize() const;
};
#endif
