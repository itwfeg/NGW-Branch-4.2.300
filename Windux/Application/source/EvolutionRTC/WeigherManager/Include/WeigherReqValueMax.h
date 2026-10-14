/**
 * @file WeigherReqValueMax.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max requests
 * Created on: Sept 20, 2017
 *
 **/
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

#ifndef WEIGHERREQVALUEMAX_H
#define WEIGHERREQVALUEMAX_H

class WeigherReqValueMax : public EvolMessage
{
	public:

	typedef enum
	{
	    VMREQ_ASSEMBLY,
	    VMREQ_HWVERSION,
	    VMREQ_SERIALNUM,
	    VMREQ_DATE,
	    VMREQ_XGAINFACTOR,
	    VMREQ_YGAINFACTOR,
	    VMREQ_ZGAINFACTOR,
	    VMREQ_CALIBRATION,
	    VMREQ_CURRENTFACTORS,
	}ValueMaxReq;

	WeigherReqValueMax( ValueMaxReq req, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherReqValueMax( const char *buffer );
	~WeigherReqValueMax();

	ValueMaxReq getRequest() { return request; }

	const char *serialize()const;

	private:
	ValueMaxReq request;

};
#endif
