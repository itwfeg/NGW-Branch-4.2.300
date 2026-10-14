#ifndef COMMUNICATIONSYNCHRONIZEREVENT_H
#define COMMUNICATIONSYNCHRONIZEREVENT_H



class CommunicationSynchronizerEvent
{
	public:
		// Types and subtypes
		typedef enum
		{
			START_EVENT,

			INITIALIZATION_COMPLETE_EVENT,

			CONNECTED_EVENT,

			HEADER_SENT_EVENT,

			HEADER_RECEIVED_EVENT,

			HEADER_PROCESSED_EVENT,

			COMMAND_SENT_EVENT,
			LAST_COMMAND_SENT_EVENT,

			COMMAND_RECEIVED_EVENT,
			ERROR_RECEIVED_EVENT,
			FOOTER_RECEIVED_EVENT,
			NOTHING_RECEIVED_EVENT,

			COMMAND_PROCESSED_EVENT,

			FOOTER_SENT_EVENT,

			FOOTER_PROCESSED_EVENT,

			ERROR_EVENT,
			ERROR_PROCESSED_SEND_FOOTER_EVENT,
			ERROR_PROCESSED_READ_FOOTER_EVENT,
			ERROR_PROCESSED_DISCONNECT_EVENT,
			ERROR_PROCESSED_CONCLUDE_EVENT,

			DISCONNECTED_EVENT,

			END_EVENT
		} eventId_type;


		// Constructors and destructors
		CommunicationSynchronizerEvent(eventId_type id) : id_(id) {}
		virtual ~CommunicationSynchronizerEvent() {}


		// Accessor methods
		eventId_type getId() const { return id_; }



	private:
		eventId_type id_;
};



#endif // COMMUNICATIONSYNCHRONIZEREVENT_H
