#ifndef GCDFILEUPLOADMSG_H
#define GCDFILEUPLOADMSG_H

#include "EvolMessage.h"
#include "HobString.h"
#include "MessageFormat.h"

class GCDDevFileUploadMsg : public EvolMessage
{
public:
	GCDDevFileUploadMsg(LPCSTR szRemoteFilePath, const DWORD dwFileSize, const DWORD dwLastWriteTime);

	UINT32 getSerializedLength() const;

	const char *serialize() const;
};
#endif
