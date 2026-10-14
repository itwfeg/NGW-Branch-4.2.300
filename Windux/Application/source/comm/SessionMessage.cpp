/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SessionMessage.h"
#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "CMReaderWriterEvolTag.h"
#include "CMReaderWriterUltimaDisk.h"
#include "CMReaderWriterSyncTag.h"
#include "CMReaderWriterDBIndependent.h"
#include "CMReaderWriterGCD.h"
#include "CMReaderWriterHandsFreeDevice.h"
#include "DataManager.h"
#include "HandsFreeDeviceEvent.h"

#include "Integer.h"
#include "Long.h"
#include "ULong.h"
#include "Hex.h"

#define MAX_MESSAGE_DATA_LEN 10000


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int SessionMessage::_dataType_ = 0;
	std::string SessionMessage::_dataTypeName_("SessionMessage"); 
#endif

/*
 * For now, I'm not going to make this constructor private.  When using the Factory pattern,
 * I would normally make this private, so that any "user" wanting to create a SessionMessage
 * or one of its subclasses, would have to go through the factory 'create' method.
 * 
 * This constructor allows the application to create SessionMessage objects so it can send
 * messages to the UI (Browser or SM), or to the RTC.
 *
 * Remember, all parameters have default values, so none are required to be passed in.
 */
SessionMessage::SessionMessage(EvolDeviceType deviceType, UINT8 eventType, Hobart::CEvolMsgReceiver *pHobThread)
	: EvolMessage(deviceType, eventType, pHobThread),
	  originalBuff(""),
	  formatType(UnknownFormat),
	  hfcObject((HfcObject *)NULL),
	  hfcObjectCreatedInternally(false),
	  messageHandler((DataManager *)NULL)

{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

}


/* NOTE:
 * This constructor will only work for internal messages from the RTC, since the data passed
 * into the EvolMessage constructor is assumed to be in the internal format.
 *
 * This will generate a potentially invalid EvolMessage when the message is from
 * the Browser or SM system due to the format of the data.
 */
// private
#ifdef LINUX_FIX
SessionMessage::SessionMessage(const char *msgData, int UNUSED(msgLen))
#else
SessionMessage::SessionMessage(const char *msgData, int msgLen)
#endif      // LINUX
	: EvolMessage(msgData),
	  originalBuff(""),
	  formatType(UnknownFormat),
	  hfcObject((HfcObject *)NULL),
	  hfcObjectCreatedInternally(false),
	  messageHandler((DataManager *)NULL)
{
	UNREFERENCED_PARAMETER(msgLen);

#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

}

SessionMessage::~SessionMessage()
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif

	if (hfcObjectCreatedInternally && hfcObject)
		delete hfcObject;
}


/**
 * This class uses the Factory pattern to create SessionMessage objects.
 * by using the static creator method in the base class, described by Eckel in Thinking in C++, Version 2
 * This method is the creator method for creating SessionMessage objects.
 * For an example of where this is used, refer to the Messenger class' receive() method.
 * Once it reads a message from its socket or message queue, it creates a
 * SessionMessage for the Session to process.
 */
// static
SessionMessage *SessionMessage::create(const char *data, int len, int formatType)
{
	const char *dataToParse = (const char *)NULL;
	int lengthToParse = 0;

	SessionMessage *newMsg = new SessionMessage();

	if (newMsg)
	{
		newMsg->setFormatType(formatType);

		if (data && (len >= 0) && (len < MAX_MESSAGE_DATA_LEN))
		{
			newMsg->setOriginalBuff(HobString(data, len));
			if (Debug::isTracing("SessionMessage"))
				Debug::trace("SessionMessage", HobString("New SessionMessage original buffer: ") + HobString(Hex::toString(data, len)));
		}

		// Determine the deviceType and eventType for this SessionMessage.
		// Determining these fields depends on the formatType of the received data.
		EvolDeviceType device = UnknownDevice;
		UINT8 event = 0;
		Hobart::CEvolMsgReceiver *sender = NULL;

		HobToken *browserDataToken = (HobToken *)NULL;
		HobTokenTagged *evolFormatDataToken = (HobTokenTagged *)NULL;

		switch (formatType)
		{
			case InternalFormat:
			{
				EvolMessage *tempEvol = new EvolMessage(data);
				if (tempEvol)
				{
					device = tempEvol->getDeviceType();
					event = tempEvol->getEventType();
					sender = tempEvol->getSourceObject();
					if ( len > EvolMessage::getShortestEvolMessageLength() )
					{
						lengthToParse = len;
						dataToParse = data;
					}
					delete tempEvol;
				}
				break;
			}

			case BrowserFormat:
			{
				HobString temp(data, len);
				if (Debug::isTracing("SessionMessage"))
					Debug::trace("SessionMessage", HobString("create - message is in browser format : temp=") + temp);
				HobTokenizerCSV tok(&temp, BrowserFormatSeparator, 2);

				HobString token;
				if (tok.next(&token) != -1)
					device = (EvolDeviceType)(Integer::valueOf(token));
				if (tok.next(&token) != -1)
					event = (UINT8)(Integer::valueOf(token));

				browserDataToken = HobToken::create("");
				if (browserDataToken && (dynamic_cast<HobTokenizer *> (&tok))->next(dynamic_cast<HobToken *> (browserDataToken)) != -1)
				{
					if (browserDataToken->getRawData().length() > 0)
					{
						dataToParse = (char *)browserDataToken->getRawData().c_str();
						lengthToParse = (int) browserDataToken->getRawData().length();
					}
				}
				break;
			}

			case EvolutionTaggedFormat:
			{
/*				HobString temp(data, len);
				if (Debug::isTracing("SessionMessage"))
					Debug::trace("SessionMessage", HobString("create - message is in EvolutionTagged format : temp=") + temp);
				HobTokenizerUS tok(&temp, UnitSeparator, 1);

				// The first tag of these records is expected to be the record type - RT.
				HobString hsTok;
				evolFormatDataToken = dynamic_cast<HobTokenTagged *> (HobToken::create(hsTok,HobTaggedToken));
				if (tok.next(evolFormatDataToken) != -1)
				{
					if (evolFormatDataToken->getTag() == CMReaderWriterEvolTag::EVOL_TAGGED_IDENTIFIER)
					{
						char rtData[10];
						memset(rtData, 0, 10);
						CMReaderWriterEvolTag::getRTData(data, rtData);
						CMReaderWriterEvolTag::determineDeviceAndEvent(rtData, &device, &event);

						if (tok.next(evolFormatDataToken) != -1)
						{
							if (!evolFormatDataToken->getRawData().empty())
							{
								dataToParse = (char *)evolFormatDataToken->getRawData().c_str();
								lengthToParse = evolFormatDataToken->getRawData().length();
							}
						}
					}
				}

*/
				if (strncmp(data, CMReaderWriterEvolTag::EVOL_TAGGED_IDENTIFIER, 2) == 0)
				{
					int i = 0;
					char rtData[10];
					memset(rtData, 0, 10);
					CMReaderWriterEvolTag::getRTData(data, rtData);
					CMReaderWriterEvolTag::determineDeviceAndEvent(rtData, &device, &event);

					for (i = 0; ((i < len) && (data[i] != 0x1F)); i++);

					i++; //advance to next char past unit separator

					if (i < len)//not at end of msg
					{

						dataToParse = (char *) & data[i];
						lengthToParse = len - i;
					}
				}

				break;
			}

			case DBIndependentFormatFormat:
			{
				if (strncmp(data, CMReaderWriterEvolTag::EVOL_TAGGED_IDENTIFIER, 2) == 0)
				{
					int i = 0;
					char rtData[10];
					memset(rtData, 0, 10);
					CMReaderWriterDBIndependent::getRTData(data, rtData);
					CMReaderWriterDBIndependent::determineDeviceAndEvent(rtData, &device, &event);

					for (i = 0; ((i < len) && (data[i] != 0x1F)); i++);

					i++; //advance to next char past unit separator

					if (i < len)//not at end of msg
					{

						dataToParse = (char *) & data[i];
						lengthToParse = len - i;
					}
				}

				break;
			}

			case UltimaTaggedFormat:
			{
				HobString temp(data, len);
				if (Debug::isTracing("SessionMessage"))
					Debug::trace("SessionMessage", HobString("create - message is in UltimaDisk format : temp=") + temp);

				// The first two bytes of these records are expected to be the record type data.
				char rtData[3];
				CMReaderWriterUltimaDisk::getRTData(data, rtData);
				CMReaderWriterUltimaDisk::determineDeviceAndEvent(rtData, &device, &event);

				if (len > 2)
				{
					dataToParse = data + 2;
					lengthToParse = len - 2;
				}
				break;
			}

			case SynchronizationTaggedFormat:
			{
				if (strncmp(data, CMReaderWriterSyncTag::getTaggedIdentifier(), CMReaderWriterSyncTag::getTaggedIdentifierLength()) == 0)
				{
					int i = 0;
					char rtData[10];
					memset(rtData, 0, 10);
					CMReaderWriterSyncTag::getRTData(data, rtData);
					CMReaderWriterSyncTag::determineDeviceAndEvent(rtData, &device, &event);

					for (i = 0; ((i < len) && (data[i] != 0x1F)); i++);

					i++; //advance to next char past unit separator

					if (i < len)//not at end of msg
					{

						dataToParse = (char *) & data[i];
						lengthToParse = len - i;
					}
				}

				break;
			}

			case GcdFormat:
			{
				// The first two bytes of these records are expected to be the record type data.
				char rtData[CMReaderWriterGCD::SIZEOF_RTDATA];
				CMReaderWriterGCD::getRTData(data, rtData);
				CMReaderWriterGCD::determineDeviceAndEvent(rtData, &device, &event);

				if (len > CMReaderWriterGCD::SIZEOF_RTDATA)
				{
					dataToParse = data + CMReaderWriterGCD::SIZEOF_RTDATA;
					lengthToParse = len - CMReaderWriterGCD::SIZEOF_RTDATA;
				}
				break;
			}

			case HandsFreeDeviceFormat:
			{
				// Messenger was designed to setup and return a SessionMessage to the Session for processing.
				// The Hands-free device messages were not set up as traditional SessionMessage-type messages, with HfcObjects to hold the messages' data,
				// and I didn't feel the need to create additional HfcObjects for all those messages.
				// So I'm creating a SessionMessage to hold the data for these messages, so I can continue to use the existing Messenger.
				// I'm setting up the device and event for this message only.  The message's data has already been saved into the SessionMessage::originalBuff.
				// Once returned to the Session for processing, it will take the originalData and instantiate the appropriate Hands-free message,
				// using the messagefactory class that Randy B. created for the hfpdd process.
				Debug::trace("HandsFree", "SessionMessage::create(): creating a HandsFreeDevice | HFBGenericEvent message");
				Debug::trace("HandsFree", std::string("New SessionMessage original buffer: ") + Hex::toString(data, len));
				device = HandsFreeDevice;
				event = HFDGenericEvent;
				break;
			}

			default:
				break;
		}

		// Set the deviceType, eventType, and sourceObject into the new SessionMessage.
		newMsg->setDeviceType(device);
		newMsg->setEventType(event);
		newMsg->setSourceObject(sender);

		if (Debug::isTracing("SessionMessage"))
		{
			HobString hd = HobString(Integer::toString(device));
			HobString he = HobString(Integer::toString(event));
			HobString hf = HobString(Integer::toString(formatType));
			Debug::trace("SessionMessage", HobString("create() - device:") + hd +
														HobString("  event:") + he +
														HobString("  format:") + hf);
		}

		/* Now create the HfCObject for this SessionMessage.  If the message is in internal format,
		 * (i.e. from the RTC) then the HfcObject will be one of our wrappers around the RTC objects.
		 * In this case,
		 */
		if (dataToParse)
		{
			newMsg->setHfcObject(HfcObject::create(device, event, dataToParse, lengthToParse, formatType));
			newMsg->setHfcObjectCreatedInternally(true);
		}

		if (browserDataToken)
			delete browserDataToken;
		if (evolFormatDataToken)
			delete evolFormatDataToken;
	}

	return newMsg;
}

// static
SessionMessage *SessionMessage::create(const HobString &data, int formatType)
{
	return SessionMessage::create(data.data(), (int) data.size(), formatType);
}


const HobString &SessionMessage::getOriginalBuff() const
{
	return originalBuff;
}
// private
void SessionMessage::setOriginalBuff(const HobString &buff)
{
	originalBuff = buff;
}
// private
/*
void  SessionMessage::setOriginalBuff(const char *buff, int length)
{
	if (originalBuff)
		delete originalBuff;
	originalBuff = new HobString(buff, length);
}
*/
int SessionMessage::getOriginalLength() const
{
	return (int) originalBuff.size();
}


int SessionMessage::getFormatType() const
{
	return formatType;
}
void SessionMessage::setFormatType(int type)
{
	formatType = type;
	// todo - when setting this (at least if setting to a different format than already set to,
	// should we clear any data that has been serialized, since it will be in the wrong format?
}


HfcObject *SessionMessage::getHfcObject()
{
	return hfcObject;
}
/* If the HfcObject is manually set in the SessionMessage, then the programmer maintains ownership of the HfcObject;
 * they must make sure the memory gets freed when it is no longer needed.  The SessionMessage object won't delete it.
 */
void SessionMessage::setHfcObject(HfcObject *h)
{
	hfcObject = h;
	hfcObjectCreatedInternally = false;
}


/*
SessionMessage *SessionMessage::getForwardedMessage()
{
	return forwardedMessage;
}
void SessionMessage::setForwardedMessage(SessionMessage *forwardedMessage)
{
	this->forwardedMessage = forwardedMessage;
}
*/


void SessionMessage::setHfcObjectCreatedInternally(bool hoci)
{
	hfcObjectCreatedInternally = hoci;
}


DataManager *SessionMessage::getMessageHandler()
{
	return messageHandler;
}
void SessionMessage::setMessageHandler(DataManager *messageHandler)
{
	this->messageHandler = messageHandler;
}

void SessionMessage::setSourceObject( Hobart::CEvolMsgReceiver * sourceObject)
{
	EvolMessage::setSourceObject(sourceObject);

	EvolMessage *evolMessage = dynamic_cast<EvolMessage *> (getHfcObject());
	if (evolMessage)
	{
		evolMessage->setSourceObject(sourceObject);
	}

}

const char *SessionMessage::serialize() const
{
	/* Serialize the header, then serialize the data (the HfcObject).
	 * Concatenate together and return the buffer.
	 */

	serializedData = "";

	EvolMessage *e = (EvolMessage *)NULL;
	if (hfcObject)
		e = dynamic_cast<EvolMessage *> (hfcObject);

	/* If the HfcObject within this message extends from EvolMessage (as the wrappers around the
	 * RTC Objects do: HfcWeigherStatus, HfcPrinterStatus, etc.),
	 * then when the HfcObject is serialized, we will get the header as well.  If I also serialize
	 * the header here, I will have it twice.  Additionally, it is the same header.  In these cases,
	 * I have decided to ignore the SessionMessage's header and use the HfcObject's header (they will
	 * both be the same).  Therefore, skip serializing the header here.
	 */
	if (!e)
	{
		switch (formatType)
		{
			case InternalFormat:
				EvolMessage::serialize();

				if (Debug::isTracing("SessionMessage.serializeInternal"))
				{
					Debug::trace("SessionMessage.serializeInternal", HobString(" ") + EvolMessage::serializedData.c_str());
				}
				break;

			case BrowserFormat:
				serializedData = Integer::toString((int)getDeviceType());
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(Integer::toString((int)getEventType()));
				if (hfcObject)
					serializedData.append(BrowserFormatSeparator);
				break;

			case EvolutionTaggedFormat:
			{
				serializedData = CMReaderWriterEvolTag::EVOL_TAGGED_IDENTIFIER;
				
				char rtData[10];
				CMReaderWriterEvolTag::determineRecordType(rtData, getDeviceType(), getEventType());
				serializedData.append(rtData);

				if (hfcObject)
					serializedData.append(HobString(1, CMReaderWriterEvolTag::UNIT_SEPARATOR));
				break;
			}

			case DBIndependentFormatFormat:
			{
				serializedData = CMReaderWriterEvolTag::EVOL_TAGGED_IDENTIFIER;
				
				char rtData[10];
				CMReaderWriterDBIndependent::determineRecordType(rtData, getDeviceType(), getEventType());
				serializedData.append(rtData);

				if (hfcObject)
					serializedData.append(HobString(1, CMReaderWriterEvolTag::UNIT_SEPARATOR));
				break;
			}

			case UltimaTaggedFormat:
			{
				char rtData[10];
				CMReaderWriterUltimaDisk::determineRecordType(rtData, getDeviceType(), getEventType());
				serializedData.append(rtData);
				break;
			}

			case SynchronizationTaggedFormat:
			{
				serializedData = CMReaderWriterSyncTag::getTaggedIdentifier();
				
				char rtData[10];
				CMReaderWriterSyncTag::determineRecordType(rtData, getDeviceType(), getEventType());
				serializedData.append(rtData);

				if (hfcObject)
					serializedData.append(HobString(1, CMReaderWriterSyncTag::getUnitSeparator()));
				break;
			}

			default:
				break;
		}
	}

	if (hfcObject)
	{
		const char *hdata = hfcObject->serialize(formatType);
		int serializedLength = hfcObject->getSerializedLength(); 
		serializedData.append(hdata, serializedLength);
	}

	if (Debug::isTracing("SessionMessage.serialize"))
	{
		Debug::trace("SessionMessage.serialize", "(" + Long::toString( serializedData.length() ) + ") " + serializedData.c_str());
	}

	return serializedData.c_str();
}


const char *SessionMessage::toString()
{
	return serialize();
}

