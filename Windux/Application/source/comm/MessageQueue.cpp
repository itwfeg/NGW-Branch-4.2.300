/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "MessageQueue.h"
#include "Integer.h"
#include "Hex.h"


MessageQueue::MessageQueue(int qMaxMsgs, int qMaxMsgLen, int qOpts) 
	: VXWMsgQ(qMaxMsgs, qMaxMsgLen, qOpts)
{
	init();
}

void MessageQueue::init()
{
//	CommMechanism::hasHeader = false;
//	CommMechanism::hasRecordSeparator = false;

	// In CommMechanism
	setMyReaderWriter((CMReaderWriter *)NULL);
	setMode(CommMechanism::MODE_INPUT | CommMechanism::MODE_OUTPUT);
}


MessageQueue::~MessageQueue()
{
	if (myReaderWriter)
		delete myReaderWriter;
}

int MessageQueue::cmSend(const char *msg, int length)
{
	// if (numberOfBytesSent < length) write log message (Warning/Error)
	return -1;
}


int MessageQueue::cmReceive(char *msgbuf)
{
	int msgLen = -1;

	msgLen = receive(msgbuf, MAX_MSG_LEN, NO_WAIT);
//	Debug::trace("MessageQueue", HobString("cmReceive - ") +
//									((msgLen == ERROR) ? HobString("error=") : HobString("bytesReceived=")) +
//									HobString(Integer::toString(msgLen)) +
//									HobString("  message=") + HobString(Hex::toString(msgbuf, msgLen)));
	if (msgLen == ERROR_CODE)
	{
		// Can get errno and display it/write it to a log message.
		perror("read from VXWMsgQ");
	}
	return msgLen;
}

int MessageQueue::cmReceive(HobString &msgbuf)
{
	int msgLen = -1;
	msgbuf = "";
	char buffer[MAX_MSG_LEN + 1];

	msgLen = receive(buffer, MAX_MSG_LEN, NO_WAIT);
//	Debug::trace("MessageQueue", HobString("cmReceive - ") +
//									((msgLen == ERROR) ? HobString("error=") : HobString("bytesReceived=")) +
//									HobString(Integer::toString(msgLen)) +
//									HobString("  message=") + HobString(Hex::toString(msgbuf, msgLen)));
	if (msgLen == ERROR_CODE)
	{
		// Can get errno and display it/write it to a log message.
		perror("read from VXWMsgQ");
	}
	else
		msgbuf.assign(buffer, msgLen);

	return msgLen;
}


int MessageQueue::cmNumMsgs()
{
	return numMsgs();
}

void MessageQueue::cmClose()
{
}

//void MessageQueue::update(/*Message*/ char *msg)
//{
//}
