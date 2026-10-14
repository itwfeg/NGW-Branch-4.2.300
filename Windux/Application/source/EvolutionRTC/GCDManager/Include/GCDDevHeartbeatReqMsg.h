#ifndef GCDHEARTBEATREQMSG_H
#define GCDHEARTBEATREQMSG_H

#include "EvolMessage.h"
#include "HobString.h"
#include "MessageFormat.h"

class GCDDevHeartbeatReqMsg : public EvolMessage
{
public:
	GCDDevHeartbeatReqMsg();

	UINT32 getSerializedLength() const;

	const char *serialize() const;
};
#endif