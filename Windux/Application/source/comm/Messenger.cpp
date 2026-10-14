/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Messenger.h"
#include "Session.h"
#include "SessionMessage.h"
#include "GenericHfcObject.h"
#include "Configuration.h"


#include "Integer.h"
/*#include "Double.h" */
#include "Hex.h"

#include <errno.h>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Messenger::_dataType_ = 0;
std::string Messenger::_dataTypeName_("Messenger");
#endif

    Messenger::Messenger(Hobart::CEvolMsgReceiver *session, CommMechanism *ui)
: mySession(session),
    myUI(ui)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    init();
}

void Messenger::init()
{
    myQ = (CommMechanism *)NULL;
    nextCommMechanismID = 0;
    consecutiveRTCMessages = 0;
	postponingInput = false;
    msgReceived = false;
}


Messenger::~Messenger()
{
    // Don't delete the Session here!
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif

    if (myUI)
    {
        delete myUI;
        myUI = NULL;
    }

    // Cleanup any comm mechanisms still in the list.
    removeAllCommMechanisms();
}


void Messenger::setMySession(Hobart::CEvolMsgReceiver *session)
{
	Util::CLockCriticalSection lock( receiveCS ); // SOF-1407

    if ( (mySession = session) )
    {
    	Session *s;

    	if ( (s = dynamic_cast<Session*>( session )) )
    		debugTraceHeader = s->getName() + HobString(" - ");
    }

#if 0
    else
    {
        TestBrowser *t = (TestBrowser *)NULL;
        t = dynamic_cast<TestBrowser *> (session);
        if (t)
            debugTraceHeader = HobString(((TestBrowser *)session)->hcn) + HobString(" - ");
    }
#endif
}

CommMechanism *Messenger::getMyQ()
{
    return myQ;
}
void Messenger::setMyQ(CommMechanism *myQ)
{
	Util::CLockCriticalSection lock( receiveCS ); // SOF-1407

	this->myQ = myQ;
}

CommMechanism *Messenger::getMyUI()
{
    return myUI;
}
void Messenger::setMyUI(CommMechanism *myUI)
{
	Util::CLockCriticalSection lock( receiveCS ); // SOF-1407

    this->myUI = myUI;

#ifndef TORNADO_2_2
    if (this->myUI != (CommMechanism *)NULL)
    {
        minutesElapsed = 0;
        //msgTimer.reset();
        msgReceived = false;
    }
#endif
}

int Messenger::addCommMechanism(CommMechanism *cm)
{
	if (cm)
		myComms.push_back(cm);
    nextCommMechanismID = myComms.size();
    return ((int)myComms.size() - 1);
}

void Messenger::removeCommMechanism(unsigned int cmId)
{
    if (myComms.empty() || cmId >= myComms.size())
        return;

    Debug::trace("Messenger", HobString("removeCommMechanism - supposed to remove offset ") + HobString(Integer::toString(cmId)));

    // Removing an element other than the last in the vector.
    if (cmId < myComms.size()-1)
    {
        myComms[cmId] = (CommMechanism *)NULL;
        Debug::trace("Messenger", "removeCommMechanism - offset wasn't the last, but was set to null");
    }
    // Removing the last element in the vector.
    else
    {
        myComms.pop_back();
        Debug::trace("Messenger", "removeCommMechanism - offset was the last in the vector");
        if (myComms.size())
        {
            size_t i = myComms.size() - 1;
            while (i != 0 && myComms[i] == (CommMechanism *)NULL)
            {
                Debug::trace("Messenger", HobString("removeCommMechanism - while removing the last, removed a previously removed mechanism at offset ") + HobString(Integer::toString((int)i)));
                myComms.pop_back();
                i--;
            }
        }
        nextCommMechanismID = myComms.size();
    }
}

void Messenger::removeCommMechanism(CommMechanism *cm)
{
    if (myComms.empty() || !cm)
        return;

    unsigned int i = 0;
    while (i < myComms.size())
    {
        if (myComms[i] == cm)
        {
            removeCommMechanism(i);
            break;
        }
    }
}

void Messenger::removeAllCommMechanisms()
{
    if (myComms.empty())
        return;
    for (unsigned int i=(unsigned int)myComms.size(); i > 0; i--)
        removeCommMechanism(i-1);
}


int Messenger::send(EvolMessage *message, int destination, bool shouldDelayAfterSending)
{
    int bytesSent = -1;

    if (message)
    {
        const char *serializedData = message->serialize();
        int serializedLength = message->getSerializedLength();
        bytesSent = send(serializedData, serializedLength, destination, shouldDelayAfterSending);
    }
    else
    {
        Debug::trace("Session.send","ERROR: Messenger::send(): message is NULL! Ignoring");
        // todo - log warning message, trying to send a message without passing in a Message to send.
    }
    return bytesSent;
}


/*****************************************************************************
 *
 * Send a message to my connected UI (Browser, SM, ...)
 *
 * returns: the number of bytes sent
 *                      bytesSent will be 0 if no bytes were sent and no error occurred.
 *                              This could happen if no message was passed in, or no destination
 *                      existed (not really error conditions).
 *                              bytesSent will be -1 if an error occurred in actually trying to
 *                              send the message.
 * private
 *
 ****************************************************************************/
int Messenger::send(const char *msg, int msgLength, int destination, bool shouldDelayAfterSending)
{
    int bytesSent = 0;

    if (msg && msgLength > 0)
    {
        switch (destination)
        {
            case DEST_RTC:
                mySession->dispatchEvent(msg, msgLength);
                break;


            case DEST_UI:
                if (myUI)
                {
                    bytesSent = myUI->cmSend(msg, msgLength);
                    Debug::trace("Messenger.send", HobString("sent to UI:  bytesSent=") + HobString(Integer::toString(bytesSent)) +
                            HobString("  message=") + HobString(Hex::toString(msg, msgLength)));
                }
                else
                {
                    // todo - log error message, trying to send a message without the destination being set up.
                }
                break;


            default:
                if (!myComms.empty() && destination >= 0 && (unsigned int)destination < myComms.size() && myComms[destination])
                {
                    CommMechanism *dest = myComms[destination];
                    if (dest->isOutputEnabled())
                    {
                        bytesSent = dest->cmSend(msg, msgLength);
                        if (bytesSent < msgLength)
                        {
                            if (Debug::isTracing("Messenger.error"))
                            {
                                Debug::trace("Messenger.error", "*******************************************************************************");
                                Debug::trace("Messenger.error", HobString("send error - errno=") + HobString(Integer::toString(errno)) +
                                        HobString("  message=") + HobString(Hex::toString(msg, msgLength)));
                                Debug::trace("Messenger.error", "*******************************************************************************");
                            }

                            bytesSent = -1;
                            break;  // out of while loop, don't need to retry the message
                        }
                        else
                        {
                            if (Debug::isTracing("Messenger.send"))
                            {
                                HobString cm(Integer::toString(destination));
                                Debug::trace("Messenger.send", HobString("sent to cmId:") + cm +
                                        HobString("  bytesSent=") + HobString(Integer::toString(bytesSent)) +
                                        HobString("  message=") + HobString(Hex::toString(msg, msgLength)));
                            }
                        }
                    }
                    else
                    {
                        //todo - write log message that the application tried to write to
                        //       a CommMechanism that didn't have its mode set for output.
                    }
                }

                // todo - log an error, caller sent in an unknown destination.
                break;
        };
    }
    else
    {
        // todo - log warning message, trying to send a null message or length <= 0.
    }

    if (shouldDelayAfterSending)
    {
#ifdef LINUX_FIX
        PAL_absclass::PAL_sleep( 0 );
#else
        Sleep( 0 );
#endif
    }
    return bytesSent;
}


/*****************************************************************************
 *
 * Read a message from either the Message Queue (RTC, other Sessions), or
 * from my connected UI (Browser, SM, ...).
 *
 * returns: a SessionMessage containing the message received
 *
 * NOTE:
 * The Message that is instantiated by this method is to be deleted by the object
 * (Session) that calls this method.
 *
 ****************************************************************************/
SessionMessage *Messenger::receive()
{
	Util::CLockCriticalSection lock( receiveCS ); // SOF-1407

    /* **************************************************************************************************
     * If tracking the time to process a message, and we received a message,
     * display the amount of time it took to process the message.
     * **************************************************************************************************
     */
#ifndef TORNADO_2_2
    if (msgReceived)
    {
        msgReceived = false;
        /*msgTimer.stop();
          double msgTime = msgTimer.elapsedTime();
          Debug::trace("Messenger.msgTimer", debugTraceHeader + HobString("last message processing time : ") + HobString(Double::toString(msgTime,15,15)));
          msgTimer.reset();
          msgTimer.start(); */
    }
#endif

    //  Debug::trace("Messenger.receive", "Entered receive method.");

    /* **************************************************************************************************
     * Before checking for any messages, check if there are any messages in
     * the queue from my UI.  If there are, don't check for timeout. We still
     * may not read those messages yet, since messages from the real-time
     * controller have a higher priority.  We want to initially check if
     * there are any messages from the UI, since we still want to timeout if
     * we receive messages from the RTC, but not from the UI.
     * **************************************************************************************************
     */
    if (( myUI ) && ( mySession ))
    	if ( 0 < myUI->cmNumMsgs() )
        {
            Session *s;

            if ( (s = dynamic_cast<Session*>( mySession )) )
                s->manageAutoLogoutTimer();
        }

    /* **************************************************************************************************
     * Check my internal message queue for any messages.  Internal messages, those from
     * the real-time controller or from another session, have a higher priority than
     * messages from my associated UI.
     *
     * However, we don't want to completely starve the UI for attention if we just constantly get
     * messages in the Message Queue.  So, I've built in a maximum number of Message Queue messages
     * we'll receive when we will finally check the socket for messages.
     *
     * **************************************************************************************************
     */
    if (myQ == (CommMechanism *)NULL)
    {
        Debug::trace("Messenger.void", "receive - myQ is null and void.");
    }
    else if (consecutiveRTCMessages > Configuration::getInstance()->getMaxConsecutiveRTCMessages())
    {
        Debug::trace("Messenger.consecutive", debugTraceHeader + HobString("receive - Too many consecutive RTC messages: ") + HobString(Integer::toString(consecutiveRTCMessages)));

        // todo - write out Event Log message, just to track how often this is happening.
    }
    else
    {
        int numMsgs = myQ->cmNumMsgs();
        if (numMsgs > 0)
        {
            Debug::trace("Messenger.receive", debugTraceHeader + HobString("receive - myQ number of msgs=") + HobString(Integer::toString(numMsgs)));

            consecutiveRTCMessages++;

            HobString data = "";
            int len = myQ->cmReceive(data);

            /* If tracking the time to process a message, set the flag indicating we received
             * a message, and reset the timer.
             */
#ifndef TORNADO_2_2
            if (Debug::isTracing("Messenger.msgTimer"))
            {
                msgReceived = true;
                /*msgTimer.stop();
                  msgTimer.reset();
                  msgTimer.start(); */
            }
#endif

            Debug::trace("Messenger.receive", debugTraceHeader + HobString("receive - myQ length=") + HobString(Integer::toString(len))
                    + HobString("  data=") + HobString(Hex::toString(data.data(), (int)data.size())));

            SessionMessage *newMsg = SessionMessage::create(data);
            return newMsg;

            //newMsg->setSource("Internal");
        }
        else
            Debug::trace("Messenger.nomsg", "receive - myQ no messages received.");
    }


    /* Reset the 'consecutive' counter, regardless of whether a message from the UI is waiting
     * to be read or not.  We've had the chance to look for UI messages, so this restarts the
     * 'consecutive' counter.
     */
    consecutiveRTCMessages = 0;

    Session *s = (Session *)NULL;
    s = dynamic_cast<Session *> (mySession);
	if ( postponingInput ||
		 (s && (s->getCurrentModeOp() == Session::OP_READALL || s->getCurrentModeOp() == Session::OP_WAITFORDBCLOSEDTEMPORARILY)) )
	{
		/* This happens when the Session received a message to process a "Read All" (from DG).
		 * While it is processing this, it shouldn't read and process any more commands until
		 * the "Read All" finishes.
		 */
		Debug::trace("Messenger.readall", "Session currently postponing input or performing a READALL, won't read any messages from socket until completed.");
		return 0;
	}

    /* **************************************************************************************************
     * No internal messages received, so check for any messages from my associated UI.
     * **************************************************************************************************
     */
    if (myUI == (CommMechanism *)NULL)
    {
        Debug::trace("Messenger.void", "receive - myUI is null and void.");
    }
    else
    {
        int numMsgs = myUI->cmNumMsgs();
        if (numMsgs > 0)
        {
            // todo - set up a constant for this, the maximum number of bytes
            //                       that will ever be in a serialized message stream.
            SessionMessage *newMsg = (SessionMessage *)NULL;
            HobString data = "";

            int len = myUI->cmReceive(data);
            if (len > 0)
            {
                if (Debug::isTracing("Messenger.receive"))
                {
                    Debug::trace("Messenger.receive", debugTraceHeader + HobString("receive - myUI received msg: len=") + HobString(Integer::toString(len)) +
                            HobString("  data=") + HobString(Hex::toString(data.data(), (int)data.size())));
                }

                int msgType = UnknownFormat;
                CMReaderWriter *readerWriter = myUI->getMyReaderWriter();
                if (readerWriter)
                {
                    switch (readerWriter->getReaderWriterId())
                    {
                        case CMReaderWriter::EVOL_UI_ID:
                            msgType = BrowserFormat;
                            break;
                        case CMReaderWriter::EVOL_TAGGED_ID:
                            msgType = EvolutionTaggedFormat;
                            break;
                        case CMReaderWriter::ULTIMA_DISK_ID:
                            msgType = UltimaTaggedFormat;
                            break;
                        case CMReaderWriter::SYNC_TAGGED_ID:
                            msgType = SynchronizationTaggedFormat;
							break;
						case CMReaderWriter::GCD_ID:
							msgType = GcdFormat;
							break;
						case CMReaderWriter::HANDSFREEDEVICE_ID:
							Debug::trace("HandsFree", "Messenger received a hands-free device message");
							msgType = HandsFreeDeviceFormat;
							break;
                        default:
                            break;
                    }
                }
                newMsg = SessionMessage::create(data, msgType);
            }
            return newMsg;
        }
        else if ( numMsgs < 0 )
		{
			// The socket has probably been closed.
            Debug::trace("Messenger.nomsg", "receive - numMsgs < 0 - socket problem!");

			SessionMessage *newMsg = new SessionMessage(Error, ErrEOF);
			return newMsg;
		}
		else
            Debug::trace("Messenger.nomsg", "receive - myUI no messages received.");
    }


    /* **************************************************************************************************
     * No UI messages received, so check for any messages from any other CommMechanism
     * that has been registered (via addCommMechanism()).
     * **************************************************************************************************
     */
    /* todo - The following will look for input from the first CommMechanism in the list that has input enabled,
     *        but how am I going to proceed onto the next CommMechanism in the list that has input enabled???
     */
    if (!myComms.empty())
    {
        for (unsigned int source = 0; source < myComms.size(); source++)
        {
            Debug::trace("Messenger.receive", "Trying to receive from CommMechanism.");

            CommMechanism *src = myComms[source];
            if (src && src->isInputEnabled())
            {
                //                              Debug::trace("Messenger.receive", "CommMechanism has input enabled.");

                int numMsgs = src->cmNumMsgs();
                if (numMsgs > 0)
                {
					SessionMessage *newMsg = (SessionMessage *)NULL;
					HobString data = "";

					int len = src->cmReceive(data);
					if (len > 0)
					{
						/* If tracking the time to process a message, set the flag indicating we received
						 * a message, and reset the timer.
						 */
#ifndef TORNADO_2_2
						if (Debug::isTracing("Messenger.msgTimer"))
						{
							msgReceived = true;
							/*msgTimer.stop();
							  msgTimer.reset();
							  msgTimer.start();*/
						}
#endif

						if (Debug::isTracing("Messenger.receive"))
						{
							HobString s = HobString(Integer::toString(source));
							Debug::trace("Messenger.receive", debugTraceHeader +
									HobString("receive - myComm:") + s +
									HobString("  len=") + HobString(Integer::toString(len)) +
									HobString("  data=") + HobString(Hex::toString(data.data(), (int)data.size())));
						}

						int msgType = UnknownFormat;
						CMReaderWriter *readerWriter = src->getMyReaderWriter();
						if (readerWriter)
						{
							switch (readerWriter->getReaderWriterId())
							{
								case CMReaderWriter::EVOL_UI_ID:
									msgType = BrowserFormat;
									break;
								case CMReaderWriter::EVOL_TAGGED_ID:
									msgType = EvolutionTaggedFormat;
									break;
								case CMReaderWriter::ULTIMA_DISK_ID:
									msgType = UltimaTaggedFormat;
									break;
								case CMReaderWriter::DB_INDEPENDENT_ID:
									msgType = DBIndependentFormatFormat;
									break;
								case CMReaderWriter::GCD_ID:
									msgType = GcdFormat;
									break;
								default:
									break;
							}
						}

						newMsg = SessionMessage::create(data, msgType);
						newMsg->setMessageHandler(src->getMessageHandler());
					}

					return newMsg;
				} // numMsgs > 0

				// FT-634: no ErrEOF message unless they want it
                else if ( src->getSendEmptyMessage() )  // numMsgs == 0
                {
                    /* Nothing to read, but I need to notify the message handler if we reached
                     * the EOF; so return a blank message.
                     */
                    SessionMessage *newMsg = new SessionMessage(Error, ErrEOF);
                    newMsg->setMessageHandler(src->getMessageHandler());
                    return newMsg;
                }

            }
        } // for loop
    } // myComms not empty
    //  else
    //          Debug::trace("Messenger.receive", "No CommMechanisms available.");

    return (SessionMessage *)NULL;
}
