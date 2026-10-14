#ifndef GCDSETVOLUMEMSG_H
#define GCDSETVOLUMEMSG_H

#include "EvolMessage.h"
#include "HobString.h"
#include "MessageFormat.h"

class GCDDevSetVolumeMsg : public EvolMessage
{
public:
	GCDDevSetVolumeMsg(int percent);

	UINT32 getSerializedLength() const;

	const char *serialize() const;
};
#endif