/**
 * @file NGWStartUpgrade.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief NGW Wrapper Events
 * Created on: April 4, 2016
 *
 **/
#ifndef NGWSTARTUPGRADE_H_
#include "EvolMessage.h"
#include "NGWEvent.h"

class NGWStartUpgrade : public EvolMessage
{
	public:

	NGWStartUpgrade( Hobart::CEvolMsgReceiver *pHobThread = NULL, UINT8 numOfNodes = 6 );
	NGWStartUpgrade( const char *buffer );
	~NGWStartUpgrade();

	UINT8 getNumOfNodes() const			{ return numberOfNodes; }
	void setNumOfNodes( UINT8 id )		{ numberOfNodes = id; }

	const char *serialize()const;

	private:
	UINT8 numberOfNodes;
};



#endif
