/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SESSIONMESSAGE_H
#define SESSIONMESSAGE_H

#include "evolution.h"
#include "EvolMessage.h"
#include "HfcObject.h"

class DataManager;


class SessionMessage : public EvolMessage
{
	public:

		SessionMessage(EvolDeviceType deviceType = UnknownDevice, UINT8 eventType = 0, Hobart::CEvolMsgReceiver * receiver = NULL);

		~SessionMessage();

		/**
		 * This class uses the Factory pattern to create SessionMessage objects.
		 * This method is the creator method for creating SessionMessage objects.
		 */
		static SessionMessage *create(const char *data, int len, int formatType = InternalFormat);
		static SessionMessage *create(const HobString &data, int formatType = InternalFormat);


		const HobString &getOriginalBuff() const;
		int getOriginalLength() const;

		int getFormatType() const;
		void setFormatType(int type);

		virtual HfcObject *getHfcObject();
		virtual void setHfcObject(HfcObject *h);

		//SessionMessage *getForwardedMessage();
		//void setForwardedMessage(SessionMessage *forwardedMessage);

		/**
		 * This method is used to set the hfcObjectCreatedInternally flag from the
		 * create() method in this class, since it creates a new one and returns the
		 * pointer to it.
		 *
		 * It generally should not be called by any other class in the system.
		 */
		void setHfcObjectCreatedInternally(bool hoci);


		DataManager *getMessageHandler();
		void setMessageHandler(DataManager *messageHandler);

		virtual void setSourceObject( Hobart::CEvolMsgReceiver *sourceObject);

		virtual const char *serialize() const;
		// don't need to override - UINT16 getSerializedLength() const;

		virtual const char *toString();


	private:

		HobString originalBuff;
		//void setOriginalBuff(const char *buff, int length);
		void setOriginalBuff(const HobString &buff);

		int formatType;

		HfcObject *hfcObject;

		/**
		 * This property is used for detecting if the hfcObject contained within
		 * this message was created by this SessionMessage object, or was created
		 * by another part of the application and then set in this SessionMessage
		 * object.
		 *
		 * If this SessionMessage object created it, then the hfcObject
		 * should be deleted when this SessionMessage is deleted. If the SessionMessage
		 * object created it, and the application wants to take ownership of the HfcObject
		 * while processing the message, the application should call the
		 * setHfcObjectCreatedInternally method, passing in false.
		 *
		 * If it was created externally and then set into this SessionMessage, then
		 * the class/method that created the hfcObject is responsible for deleting
		 * the hfcObject.
		 */
		bool hfcObjectCreatedInternally;


		// Original idea not currently being used.
		//SessionMessage *forwardedMessage;

		/* A pointer to a dataManager for who should handle the message,
		 * if other than the Session with which this Messenger is associated.
		 */
		DataManager *messageHandler;

		/**
		 * This constructor is used to construct a message from one that has been sent from another source.
		 * For example, to send a message through a Message Queue, the Message properties will be
		 * converted, and combined, into a string.  Upon receiving the message, the receiving process will
		 * convert the data back into a Message object, for the application to continue processing.
		 *
		 * It is private because the create factory is the only user of this constructor.
		 */
		SessionMessage(const char *msgData, int msgLen);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //SESSIONMESSAGE_H
