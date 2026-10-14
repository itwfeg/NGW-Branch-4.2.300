/**
 * @file NGWUpgradeStatus.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief NGW Wrapper Events
 * Created on: April 4, 2016
 *
 **/
#ifndef NGWUPGRADESTATUS_H_
#define NGWUPGRADESTATUS_H_
#include "EvolMessage.h"
#include "NGWEvent.h"
#include "Integer.h"

class NGWUpgradeStatus : public EvolMessage
{
	public:

	NGWUpgradeStatus( Hobart::CEvolMsgReceiver *pHobThread = NULL, UINT8 id = 1 );
	NGWUpgradeStatus( const char *buffer );
	~NGWUpgradeStatus();

	typedef enum
	{
		_Version,
		_Upgrade,
		_Error,
		_Execute
	}UpgradeState;
	
	typedef enum
	{
		_None,
		_NoResponseFromNode,
		_NodeTimeout,
		_NodeWriteFailure,
		_NodeChecksumFailure,
		_NodeEraseBank1Failure,
		_NodeEraseBank2Failure,
		_NodeIdFailure,
		_NodeWriteAddressOutOfRange
	}NGWUpgradeError;

	UINT8 getNodeId() const					{ return nodeId; }
	void setNodeId( UINT8 id )				{ nodeId = id; }
	
	UpgradeState getState()	const			{ return state; }
	
	void setStateVersion()					{ state = _Version; }
	void setStateUpgrade()					{ state = _Upgrade; }
	void setStateError()					{ state = _Error; }
	void setStateExecute()					{ state = _Execute; }

	std::string getVersion() const				{ return version; }
	void setVersion( const std::string& ver ) 	{ version = ver; }

	NGWUpgradeError getError() 	const		{ return _error; }

	void setErrorNone()						{ _error = _None; }
	void setErrorNoResponse()				{ _error = _NoResponseFromNode; }
	void setErrorTimeout()					{ _error = _NodeTimeout; }
	void setErrorWriteFailure()				{ _error = _NodeWriteFailure; }
	void setErrorChecksum()					{ _error = _NodeChecksumFailure; }
	void setErrorEraseFailure1()			{ _error = _NodeEraseBank1Failure; }
	void setErrorEraseFailure2()			{ _error = _NodeEraseBank2Failure; }
	void setErrorNodeIdFailure()			{ _error = _NodeIdFailure; }
	void setErrorWriteOUtOfRange()			{ _error = _NodeWriteAddressOutOfRange; }

	const char *serialize()const;

	void show();

	private:
	UINT8 nodeId;
	UpgradeState state;
	std::string version;
	NGWUpgradeError _error;
};

#endif 
