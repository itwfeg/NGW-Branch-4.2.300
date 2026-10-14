/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include "evolution.h" //lcg rev 0.1
//#include <vxWorks.h>
//#include <vxwMsgQLib.h>
#include "CommMechanism.h"

/**
 * The RTC needs to send a message to anyone (Session)
 * who wants the information it is currently processing.  To do this, the RTC
 * sends a message to that Session's Message Queue
 * (which is maintained by the Session's Messenger).  The
 * Observer pattern specifies that the RTC should call
 * update() for all of its observers.  Since the Session is already an observer
 *  of its Messenger, it would get confusing having the
 * Session's Messenger also calling the same update()
 * method.  Therefore, I am going to have the MessageQueue
 * be the observer.  With the registration message,
 * the RTC needs to be given the MessageQueue (myQ)  that the
 * Session's Messenger listens to for internal communication.
 */
class MessageQueue : public CommMechanism, public VXWMsgQ
{
	public:

		static const int MAX_MSGS		= 100;
		static const int MAX_MSG_LEN	= 2048;
		static const int OPTS			= MSG_Q_FIFO;


		MessageQueue(int qMaxMsgs = MAX_MSGS, int qMaxMsgLen = MAX_MSG_LEN, int qOpts = OPTS);

		~MessageQueue();

		/**
		 * This method should convert the message to a char[],
		 * and then call send() on the VXWorks message queue.
		 */
		int cmSend(const char * msg, int length);

		/**
		 * This method should call receive() on the VXWorks message queue,
		 * and then convert the received data into a Message, essentially
		 * de-serializing the data.
		 */
		int cmReceive(char *msgbuf);
		int cmReceive(HobString &msgbuf);

		int cmNumMsgs();

		void cmClose();

		MSG_Q_ID getMsgQId() { return (MSG_Q_ID) myValue(); }


	private:

		void init();
	
};

#endif //MESSAGEQUEUE_H

