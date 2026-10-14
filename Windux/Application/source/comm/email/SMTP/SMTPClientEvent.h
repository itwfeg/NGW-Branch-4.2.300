#ifndef SMTPCLIENTEVENT_H
#define SMTPCLIENTEVENT_H



class SMTPClientEvent
{
	public:
		// Types and subtypes
		typedef enum
		{
			START_EVENT,
			END_EVENT,
			EMAIL_AVAILABLE_EVENT,
			CONNECT_OK_EVENT,
			CONNECT_FAILED_EVENT,
			HELO_OK_EVENT,
			HELO_FAILED_EVENT,
			MAIL_OK_EVENT,
			MAIL_FAILED_EVENT,
			RCPT_ONE_SUCCEEDED_EVENT,
			RCPT_ALL_FAILED_EVENT,
			DATA_OK_EVENT,
			DATA_FAILED_EVENT,
			BODY_OK_EVENT,
			BODY_FAILED_EVENT,
			QUIT_OK_EVENT,
			QUIT_FAILED_EVENT,
			RETRIES_OK_EVENT,
			TOO_MANY_RETRIES_EVENT,
			REJECTED_OK_EVENT
		} eventId_type;


		// Constructors and destructors
		SMTPClientEvent(eventId_type id) : id_(id) {}
		virtual ~SMTPClientEvent() {}


		// Accessor methods
		eventId_type getId() const { return id_; }



	private:
		eventId_type id_;
};



#endif // SMTPCLIENTEVENT_H
