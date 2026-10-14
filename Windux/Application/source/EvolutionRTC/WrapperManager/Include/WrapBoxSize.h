/**
 * @file WrapBoxSize.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper ACK
 * Created on: Sept 5, 2017
 *
 **/

#ifndef WRAPBOXSIZE_H
#define WRAPBOXSIZE_H
#include "EvolMessage.h"
#include "WrapperEvent.h"

class WrapBoxSize : public EvolMessage
{
	public:

	WrapBoxSize(Hobart::CEvolMsgReceiver *pHobThread = NULL, UINT16 size = 0);
	WrapBoxSize(const char *buffer);
	~WrapBoxSize();

	UINT16 getSize() const { return size_; }
	const char *serialize()const;

	private:
	UINT16 size_;
};
#endif


