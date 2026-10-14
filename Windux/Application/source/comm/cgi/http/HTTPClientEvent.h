/**
 * @file HTTPClientEvent.h
 * HTTP client event.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Santos Juan
 * @author Randy Blankley
 */

#ifndef HTTPCLIENTEVENT_H
#define HTTPCLIENTEVENT_H

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HTTP client event.
class HTTPClientEvent
{
	typedef HTTPClientEvent _Myt;

public:

	/// Event types.
	typedef enum
	{
		START_EVENT,

		INITIALIZED_OK_EVENT,
		RECEIVED_REQUEST_EVENT,
		CONNECTED_OK_EVENT,
		REQUEST_SENT_OK_EVENT,
		SENDING_ENTITY_OK_EVENT,
		ENTITY_SENT_OK_EVENT,
		SEND_ENTITY_EVENT,
		READ_RESPONSE_OK_EVENT,
		ENTITY_READ_OK_EVENT,
		NEXT_REQUEST_EVENT,
		RESPONSE_PROCESSED_OK_EVENT,
		DISCONNECTED_OK_EVENT,
		CONCLUDE_EVENT,

		ERROR_EVENT,
		ERROR_DISCONNECT_EVENT,
		ERROR_WAIT_EVENT,

		END_EVENT

	} eventId_type;

	/// HTTP error types.
	typedef enum
	{
		HTTPERROR_NO_ERROR,

		HTTPERROR_CANNOT_CREATE_SOCKET,
		HTTPERROR_CANNOT_CONNECT_SOCKET,
		HTTPERROR_WRITING_TO_SOCKET,
		HTTPERROR_TIMEOUT_READING_RESPONSE,
		HTTPERROR_INVALID_CHAR_RESPONSE,
		HTTPERROR_RESPONSE_LINE_TOO_LONG,
		HTTPERROR_INVALID_RESPONSE,
		HTTPERROR_EMPTY_RESPONSE,
		HTTPERROR_RESPONSE_NOT_SUCCESS,
		HTTPERROR_TIMEOUT_READING_ENTITY,
		HTTPERROR_READING_FROM_SOCKET,

		HTTPERROR_LAST

	} HTTPError;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	HTTPClientEvent(eventId_type id) : id_(id) {}
	virtual ~HTTPClientEvent() {}

	// ========================================================================
	// Properties
	// ========================================================================

	eventId_type getId() const { return id_; }

private:

	/* --- Instance Data Members --- */
	eventId_type id_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HTTPCLIENTEVENT_H
