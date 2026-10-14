/**
 * @file NGWBootVersion.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper message containing source image
 * Created on: Dec 10, 2015
 *
 **/
#ifndef NGWBOOTVERSION_H_
#define NGWBOOTVERSION_H_
#include "EvolMessage.h"
#include "WrapperEvent.h"

class NGWBootVersion : public EvolMessage
{
	public:

	NGWBootVersion(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWBootVersion(const char *buffer);
	~NGWBootVersion();

	void setNodeId( UINT8 id )							{ nodeId = id; }
	UINT8 getNodeId() const								{ return nodeId; }

	void setAppMinorVersion( UINT16 version )			{ appMinorVersion = version; }
	void setAppMajorVersion( UINT16 version )			{ appMajorVersion = version; }

	void setDataMinorVersion( UINT16 version )			{ dataMinorVersion = version; }
	void setDataMajorVersion( UINT16 version )			{ dataMajorVersion = version; }

	void setBootMinorVersion( UINT16 version )			{ bootMinorVersion = version; }
	void setBootMajorVersion( UINT16 version )			{ bootMajorVersion = version; }

	std::string getAppVersionString() const				{ return appString; }

	const char *serialize()const;

	private:
	UINT8 nodeId;
	UINT16 appMinorVersion;
	UINT16 appMajorVersion;
	UINT16 dataMinorVersion;
	UINT16 dataMajorVersion;
	UINT16 bootMinorVersion;
	UINT16 bootMajorVersion;

	std::string appString;
};



#endif
