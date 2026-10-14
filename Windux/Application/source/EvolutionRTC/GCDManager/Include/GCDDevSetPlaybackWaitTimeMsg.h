#ifndef GCDSETPLAYBACKWAITTIMEMSG_H
#define GCDSETPLAYBACKWAITTIMEMSG_H

#include "EvolMessage.h"
#include "HobString.h"
#include "MessageFormat.h"

class GCDDevSetPlaybackWaitTimeMsg : public EvolMessage
{
public:
	GCDDevSetPlaybackWaitTimeMsg(const HobString& waittime);

	UINT32 getSerializedLength() const;

	const char *serialize() const;
};
#endif