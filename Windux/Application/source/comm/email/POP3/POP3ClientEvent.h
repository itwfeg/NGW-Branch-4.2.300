#ifndef POP3CLIENTEVENT_H
#define POP3CLIENTEVENT_H



class POP3ClientEvent
{
	public:
		// --- Types and subtypes ---
		typedef enum
		{
			START_EVENT,

			INITIALIZED_EVENT,

			CONNECTING_SUCCESS_EVENT,
			CONNECTING_FAILED_EVENT,
			CONNECTING_REJECTED_EVENT,

			FIRST_CAPABILITY_SUCCEEDED_EVENT,
			FIRST_CAPABILITY_FAILED_EVENT,
			SECOND_CAPABILITY_SUCCEEDED_EVENT,
			SECOND_CAPABILITY_FAILED_EVENT,

			AUTHENTICATION_SUCCEEDED_EVENT,
			TRY_NEXT_AUTHENTICATION_EVENT,
			AUTHENTICATION_FAILED_EVENT,

			TRANSACTING_SUCCEEDED_EVENT,
			NEXT_TRANSACTION_EVENT,
			TRANSACTION_COMMAND_FAILED_EVENT,

			CONCLUCING_SUCCEEDED_EVENT,
			CONCLUCING_FAILED_EVENT,

			COMM_ERROR_EVENT,
			TIMEDOUT_EVENT,

			END_EVENT,

		} eventId_type;


		// --- Constructors and destructors ---
		inline POP3ClientEvent(eventId_type id) : id_(id) {}
		virtual ~POP3ClientEvent() {}


		// --- Getters and setters ---
		inline eventId_type getId() const { return id_; }



	private:
		eventId_type id_;
};



#endif // POP3CLIENTEVENT_H
