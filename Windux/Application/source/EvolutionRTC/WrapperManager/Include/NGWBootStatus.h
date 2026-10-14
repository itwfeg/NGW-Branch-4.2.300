/**
 * @file NGWBootStatus.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN wrapper node boot status message
 * Created on: Dec 10, 2015
 *
 **/
#ifndef NGWBOOTSTATUS_H_
#define NGWBOOTSTATUS_H_
#include "EvolMessage.h"
#include "WrapperEvent.h"

class NGWBootStatus : public EvolMessage
{
	public:

	typedef enum
	{
		INITIALIZED,
		READY,
		FAULT,
		VERIFY,
		DONE
	}LoaderStatus;

	typedef enum
	{
		None,
		Timeout,
		WriteFailure,
		Checksum,
		UnknownRecordType,
		EraseFailureBank1,
		EraseFailureBank2,
		ComponentNotSet,
		NodeImageMismatch,
		ImageChecksumMismatch,
		NodeIdError,
		WriteAddressOutOfRange
	}LoaderFaults;

	NGWBootStatus(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWBootStatus(const char *buffer);
	~NGWBootStatus();

	const char *serialize()const;

	void setNodeId( UINT8 id ) 				{ nodeId = id; }
	UINT8 getNodeId() const					{ return nodeId; }

	LoaderStatus getStatus() const			{ return status; }
	void setStatus(LoaderStatus nodeStatus)	{ status = nodeStatus; }

	UINT32 getFaultAddress() const			{ return faultAddress; }
	void setFaultAddress(UINT32 address)	{ faultAddress = address; }
	LoaderFaults getFault() const			{ return fault; }
	void setFault(LoaderFaults error) 		{ fault = error; }

	private:
	UINT8 nodeId;
	LoaderStatus status;
	UINT32 faultAddress;
	LoaderFaults fault;
};



#endif /* NGWBOOTSTATUS_H_ */
