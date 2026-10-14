/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCOBJECT_H
#define HFCOBJECT_H


#include <vector>


#include "evolution.h"
#include "Event.h"
#include "MessageFormat.h"
#include "HobString.h"

class HfcError;


class HfcObject
{
	public:

		virtual ~HfcObject();

		/**
		 * This class uses the Factory pattern to create hfc objects.
		 * This method is the creator method for creating HfcObjects.
		 */
		static HfcObject *create(EvolDeviceType devType, UINT8 eventType, const char *data, int len, int formatType);

		int getFormatType() const;
		void setFormatType(int formatType);

		bool isReceived(int fieldOffset) const;
		void setReceived(int fieldOffset, bool received);
		void setAllReceived(bool received);

		virtual const char *serialize() const {return serialize(InternalFormat);}
		virtual const char *serialize(int format) const;
		virtual const HobString &getSerializedData() const;
		virtual UINT32 getSerializedLength() const;

		virtual const char *toString();

		HfcError *firstError();
		HfcError *nextError();
		bool hasErrors();
		void addError(int errorCode, const HobString &failedTag);
		void clearErrors();

		virtual HobString getMyKeyValue() const;
		virtual EvolDeviceType getMyEvolDevice() const;
		//void removeError(int errorCode, EvolDeviceType device, const HobString &failedTag, const HobString &keyValue);


	protected:

		mutable HobString serializedData;

		/**
		 * It is not intended that objects of this class itself be constructed.
		 * Normally, I would create a pure virtual function to enforce this.
		 * However, I currently don't have any other methods that I can make pure virtual.
		 * Therefore, I am making the constructors protected so that any subclass can still use them.
		 */
		HfcObject();

		/**
		 * This constructor is used to construct a message from one that has been sent from another source.
		 * For example, to send a message through a Message Queue, the Message properties will be
		 * converted, and combined, into a string.  Upon receiving the message, the receiving process will
		 * convert the data back into a Message object, for the application to continue processing.
		 */
		HfcObject(const char *msgData, int msgLen);

		HfcObject(const HfcObject& );

	private:

		/**
		 * This format attribute allows us to override the format specified by the SessionMessage
		 * when serializing the HfcObject contained within a SessionMessage.  For example, when sending a
		 * WeigherStatus (wrapped by the HfcWeigherStatus class) to the Browser, when in Operate mode we want
		 * just display the weight represented by the counts.  When is service mode, we want to display both
		 * the counts and raw counts.  Both messages are being sent to the Browser, so the SessionMessage's
		 * formatType will be BrowserFormat; and this is the formatType being passed into the HfcObject's
		 * serialize method.  By having the formatType attribute in this class also allows me to specifically
		 * set the formatType for the serialize method, thus, overriding the format passed in by SessionMessage.
		 *
		 * Initially, this formatType will be UnknownFormat, indicating that there is no override to the format
		 * specified by SessionMessage.  This allows backwards compatibility; if the HfcObject's format is
		 * not specifically set (to something other than UnknownFormat), then we'll just use the format passed
		 * into the serialize method.
		 *
		 * Currently, this parameter is only used when serializing data to be sent out.  It is not used when
		 * constructing some HfcObject when receiving a message (when a SessionMessage is created from a received
		 * message).
		 */
		int formatType;

		/**
		 * The following properties are used to determine if the object received contained data
		 * for a particular field.  This is used when receiving data from the UI (Browser or SM)
		 * Since not all fields need to contain data, I need to know if a particular field
		 * was received in order to update the data with the data received.  If no data was
		 * received, I don't update the field.
		 */
		/* I have changed the implementation from each byte representing a field has been received,
		 * to each bit representing a field has been received.  As such, received[0] is used for
		 * fieldOffsets 0 through 7, received[1] is used for fieldOffsets 8 through 15, etc.
		 * Within a received[x] byte, the low order bit represents the received flag for the lowest
		 * fieldOffset in the range.  Thus, for received[1], bit 0 represents fieldOffset 8 and bit 7
		 * represents fieldOffset 15.
		 * LAP - 1/16/2004
		 */
		// With the new implementation (1/16/04) the following holds:
		// Assumption: The array size of 75 indicates that no data type contains more than 600 fields.
		// Currently, Configuration contains the largest number of fields, which are defined in CfgBaseFieldOffsets.h.
		static const short maxFieldOffsetReceivedBytes = 75;
		unsigned char receivedFlags[maxFieldOffsetReceivedBytes];


		//std::list<HfcError *> errorList;
		//std::list<HfcError *>::iterator errorListIterator;
		std::vector<HfcError *> *errorList;
		std::vector<HfcError *>::iterator errorListIterator;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //HFCOBJECT_H
