#ifndef EVOLMESSAGE_H
#define EVOLMESSAGE_H

#include <algorithm>
#include <string>
#include "PAL_platformheader.h"
#include "Event.h"


namespace Hobart
{
	class CEvolMsgReceiver;
}



class EvolMessage
{
	public:
		/* --- Constructors and destructor --- */
		EvolMessage( const EvolMessage& );		
		EvolMessage(EvolDeviceType deviceType, UINT8 eventType,
				Hobart::CEvolMsgReceiver * pSource = NULL, unsigned int seqNumber = 0);
		EvolMessage(const char * buffer);
		virtual ~EvolMessage();


		/* --- Getters and setters --- */
		EvolDeviceType getDeviceType() const;
		void setDeviceType(EvolDeviceType deviceType);

		UINT8 getEventType() const;
		void setEventType(UINT8 eventType);

		Hobart::CEvolMsgReceiver *getSourceObject();
		virtual void setSourceObject( Hobart::CEvolMsgReceiver *sourceObject);

		unsigned int getSequenceNumber() const { return sequenceNumber; }
		void setSequenceNumber(unsigned int seqNumber) { sequenceNumber = seqNumber; }


		/* --- Instance function members --- */
		virtual unsigned int deserialize(const char * buffer);

		virtual const char *serialize() const;
		virtual const char *serialize(int format) const;
		virtual UINT32 getSerializedLength() const;

		virtual void show() const;


		/* --- Class function members --- */
		static UINT16 getEvolMessageLengthInBuffer();

		static int getShortestEvolMessageLength() { return 13; /* return (int)( sizeof(deviceType) + sizeof(eventType) + sizeof(sourceObject) + sizeof(sequenceNumber) ); */ }


	protected:
		/* --- Instance data members --- */
		// format of message is 1st 2 bytes = deviceType, eventType
		// next 4 bytes is source object pointer.
		// rest of message is from derived class!!
		EvolDeviceType deviceType;
		UINT8 eventType;
		Hobart::CEvolMsgReceiver *sourceObject;
		unsigned int sequenceNumber;

		mutable std::string serializedData;


		/* --- Constructors and destructor --- */
		EvolMessage();


	private:
		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif //EVOLMESSAGE_H

