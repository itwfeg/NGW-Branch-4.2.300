/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef MESSENGER_H
#define MESSENGER_H


#include <vector>


#include "evolution.h"
#include "CommMechanism.h"
#include "CEvolMsgReceiver.h"

class SessionMessage;


/**
 * This class is responsible for sending and receiving messages to/from other processes
 * and receiving messages from other processes.
 *
 * It is a wrapper for any communication topology used to communicate with another process.
 * It could communicate across a socket, to the browser, or across a message queue to the
 * real-time controller or another session.
 *
 * When receiving messages, it will use the observer pattern for notifying a module/process
 * that a message has been received.  Currently, the Session is that observing process.
 *
 * If one Session (Secondary Console Session) wants to communicate with another Session
 * (Primary Console Session), the secondaryConsoleSession needs to have a reference to the
 * primaryConsoleSession's Messenger.  It gets this from the SessionManager.
 *
 * Since we need both a C++ and Java versions of this class, we will design it according to the
 * Java implementation of threads (ie. having the start() and run() methods).*/
class Messenger //: public IEventManager
{
	public:

		static const int DEST_UI		= -1;
		static const int DEST_RTC		= -2;
		static const int DEST_SESSION	= DEST_UI;
		static const int DEST_BACKUP	= 0;


		Messenger( Hobart::CEvolMsgReceiver *mySession = NULL, CommMechanism *myUI = (CommMechanism *)NULL);

		virtual ~Messenger();

		void setMySession(Hobart::CEvolMsgReceiver *session);

		/**
		 * This is the communication mechanism other processes will use to
		 * send messages to the Session for which this Messenger was set up.
		 * This will be implemented as a Message Queue.
		 */
		CommMechanism *getMyQ();
		void setMyQ(CommMechanism *myQ);

		/**
		 * This is the communication mechanism for sending messages
		 * to and receiving messages from the UI.
		 * This will be a Socket connection, to allow a standard interface
		 * to both internal and external browsers.
		 */
		CommMechanism *getMyUI();
		void setMyUI(CommMechanism *myUI);


		// Returns the ID the application should pass to the send() method to identify
		// to which CommMechanism the message should go.
		int addCommMechanism(CommMechanism *cm);

		void removeCommMechanism(unsigned int cmId);
		void removeCommMechanism(CommMechanism *cm);


		/**
		 * If the message requires a response
		 * {
		 * .   add it to the tracking list
		 * }
		 *
		 * from the message's destination, get the CommMechanism object to which the message is to be sent.
		 * If it goes to the Session's UI, this Messenger already has the Socket to which the message should be written.
		 * If it goes to another Session, the Session will have to get that Session from the Session Manager, and then
		 * get that Session's Messenger, and then get that Messenger's myQ (the internal Message Queue monitored by
		 * that Session).
		 * If the Session Manager doesn't have that Session,
		 * write a log entry (unlikely)
		 * If the Messenger or myQ doesn't exist (returned as NULL),
		 * write a log entry (error condition, these should absolutely exist).
		 *
		 * We dynamically retrieve all of this from the Session Manager so that, in the event that any changed,
		 * (maybe the Socket got closed, and reopened as a different socket, if this is possible) then we would
		 * always be getting the most up-to-date information, directly from the source.
		 *
		 * If the destination parameter is NULL,
		 *    send it to my UI socket, by default.
		 * If myUI is NULL,
		 *    log entry indicating don't know where to send the message.
		 */
		int send(EvolMessage *message, int destination, bool shouldDelayAfterSending = true);
		int send(const char *msg, int msgLength, int destination, bool shouldDelayAfterSending = true);


		/**
		 * This method does not necessarily have to be public, since the Messenger is always looking to receive
		 * messages.  It would be called from this class' constructor
		 * (or from the run() method since the Messenger is threaded).
		 *
		 * This method needs to look for messages being sent to the Session.
		 * These could be received from this Messenger's message queue, or from the Socket connected to the UI.
		 * Check for messages in the Message Queue first.
		 * Do not block on receiving a message.
		 * If it receives a message
		 * {
		 * .   if the message is a response {
		 * .      if the message is in the tracking list {
		 * .         if logging messaging, log an entry as to message data (toString)
		 * .               and message duration (response received time - message submit time)
		 * .         We could also log an entry if a response is received for a message, but an "older"
		 * .               request is outstanding to the destination from which the current response came,
		 * .               and there was no response received for the "older" message.
		 * .         delete it from the tracking list.
		 * .      }
		 * .      else { (response for no request)
		 * .         log entry for this situation
		 * .      }
		 * .
		 * .      determine what to do with the message (notify listeners).
		 * .            automatically forward on to original requester if the request is a "redirect".
		 * .   }
		 * }
		 * else
		 * {
		 * .   (no messages in queue)
		 * .   check the tracking list for messages that have been there too long.
		 * .   if any exist {
		 * .      if the message's retry count == MAX_RETRIES {
		 * .         log entry indicating retry limit exceeded
		 * .                       (messageID, original sent time, most recent send time, retry count, msg data)
		 * .         delete it from the tracking list.
		 * .      }
		 * .      else {
		 * .         resend the message
		 * .         increase retry count by 1.
		 * .         set most recent send time to now.
		 *
		 * .         NOTE: Is this mechanism to resend a request really desired?
		 * .                    Could it corrupt the integrity of the information by:
		 * .                       submitting a request to look up a record from the database,
		 * .                       then issuing an update to the price, temporarily for the current run.
		 * .                       But, because the
		 * .      }
		 * .   }
		 * }
		 */
		SessionMessage *receive();

		bool isPostponingInput() const { return postponingInput; }
		void setPostponingInput(bool postpone) { postponingInput = postpone; }


	private:

		/**
		 * This list will allow the Messenger to keep track of messages that are
		 * forwarded to other processes for processing.  When a response is
		 * received from the forward process, the response can then be forwarded
		 * on to the originating process.
		 *
		 * When putting a Message in this list, it will have to contain the original
		 * MessageId as well as the new MessageID.  When a response is received,
		 * if the MessageId is found in this list, the message will be forwarded on to
		 * the original Message sender.
		 */
		////List forwardedMessageList;

		Hobart::CEvolMsgReceiver *mySession;

		CommMechanism *myQ;
		CommMechanism *myUI;

		std::vector<CommMechanism *> myComms;

		size_t nextCommMechanismID;
		//CommMechanism *myBackupFile;

#ifndef TORNADO_2_2
		// RWTimer msgTimer;
		bool msgReceived;
		int minutesElapsed;
#endif

		Util::CSmartCriticalSection  receiveCS;

		short consecutiveRTCMessages;

		bool postponingInput;

		HobString debugTraceHeader;

		void init();
		void removeAllCommMechanisms();

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //MESSENGER_H




