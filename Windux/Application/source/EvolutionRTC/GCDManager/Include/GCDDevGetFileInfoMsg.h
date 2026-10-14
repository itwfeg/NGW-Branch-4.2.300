#ifndef GCDGETFILEINFOMSG_H
#define GCDGETFILEINFOMSG_H

#include "EvolMessage.h"
#include "HobString.h"
#include "MessageFormat.h"

class GCDDevGetFileInfoMsg : public EvolMessage
{
public:
	GCDDevGetFileInfoMsg(LPCSTR szRemoteFilePath);

	UINT32 getSerializedLength() const;

	const char *serialize() const;
};
#endif

