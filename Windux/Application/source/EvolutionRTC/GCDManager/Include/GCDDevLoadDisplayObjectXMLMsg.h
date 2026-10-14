#ifndef GCDLOADDISPLAYOBJECTXMLMSG_H
#define GCDLOADDISPLAYOBJECTXMLMSG_H

#include "EvolMessage.h"
#include "HobString.h"
#include "MessageFormat.h"

class GCDDevLoadDisplayObjectXMLMsg : public EvolMessage
{
public:
	GCDDevLoadDisplayObjectXMLMsg(LPCSTR szFilename, const DWORD dwIdleAdvertType, LPCSTR szWebsite);

	UINT32 getSerializedLength() const;

	const char *serialize() const;
};
#endif
