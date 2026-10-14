#ifndef GCDLOADPLAYLISTMSG_H
#define GCDLOADPLAYLISTMSG_H

#include "EvolMessage.h"
#include "HobString.h"
#include "MessageFormat.h"

class GCDDevLoadPlaylistMsg : public EvolMessage
{
public:
	GCDDevLoadPlaylistMsg(const HobString& playlist);

	UINT32 getSerializedLength() const;

	const char *serialize() const;
};
#endif