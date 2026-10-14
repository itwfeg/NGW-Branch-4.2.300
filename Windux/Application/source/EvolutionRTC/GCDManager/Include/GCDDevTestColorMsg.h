#ifndef GCDTESTCOLORMSG_H
#define GCDTESTCOLORMSG_H

#include "EvolMessage.h"
#include "HobString.h"
#include "MessageFormat.h"

class GCDDevTestColorMsg : public EvolMessage
{
public:
	GCDDevTestColorMsg(int fType, const HobString &image);

	UINT32 getSerializedLength() const;

	const char *serialize() const;
};
#endif