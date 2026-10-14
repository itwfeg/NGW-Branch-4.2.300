#pragma once
#include "EvolMessage.h"
#include "HobString.h"

class GCDSystemMsg : public EvolMessage
{
public:
	GCDSystemMsg(EvolDeviceType deviceType, UINT8 eventType, const HobString& data);
	virtual ~GCDSystemMsg(void);

	const HobString& getData() const {return m_data;}
	/* --- Instance function members --- */
	const char *serialize() const;


private:
	HobString m_data;
};
