/**
 * @file HTTPClient.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Santos Juan
 * @author Randy Blankley
 */

#include "PAL_platformheader.h"

#include "Configuration.h"
#include "Debug.h"
#include "HTTPClient.h"
#include "HTTPRequest.h"
#include "HTTPClientError.h"

#include "cgi/CGICallerI.h"

#ifndef __linux__
#include <io.h>
#endif

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

namespace comm
{

int HTTPClient::_waitForRequestDelay_ = 1000;						// 1 sec (= 1000 ms)
int HTTPClient::_readResponseTimeout_ = 30;							// 30 seconds
int HTTPClient::_readSomeResponseDelay_ = 50;						// 50 milliseconds
int HTTPClient::_readNoResponseDelay_ = 500;						// 500 milliseconds
unsigned int HTTPClient::_maxResponseLineLength_ = 2048;			// 2048 octets

bool HTTPClient::_useOfflineSpooling_ = false;
unsigned int HTTPClient::_offlineSpoolRetry_ = 1800;				// 30 minutes
unsigned int HTTPClient::_offlineSpoolMaxRequestAge_ = 1209600;		// 14 days
std::string HTTPClient::_offlineSpoolFile_;

const std::string HTTPClient::_statenameInitializing_("Initializing");
const std::string HTTPClient::_statenameWaiting_("Waiting");
const std::string HTTPClient::_statenameConnecting_("Connecting");
const std::string HTTPClient::_statenameSendingRequest_("Sending Request");
const std::string HTTPClient::_statenameSendingEntity_("Sending Entity");
const std::string HTTPClient::_statenameReadingResponse_("Reading Response");
const std::string HTTPClient::_statenameReadingEntity_("Reading Entity");
const std::string HTTPClient::_statenameProcessingResponse_("Processing Response");
const std::string HTTPClient::_statenameDisconnecting_("Disconnecting");
const std::string HTTPClient::_statenameConcluding_("Concluding");
const std::string HTTPClient::_statenameProcessingError_("Processing Error");
const std::string HTTPClient::_statenameUnknown_("Unknown");

const std::string HTTPClient::_ThreadName_("tHTTPClient");
const int HTTPClient::_DefaultHTTPServerPort_ = 80;             // HTTP well-known port

#ifdef USE_ACCOUNTANT
int HTTPClient::_dataType_ = 0;
std::string HTTPClient::_dataTypeName_("comm::HTTPClient");
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPClient::HTTPClient(CGICallerI& inter) :
	interface_(inter),
	despoolTime_( 0 )
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HTTPClient", "Initializing HTTP client");

	checkingThread_ = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, _ThreadName_.c_str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPClient::~HTTPClient()
{
	if (checkingThread_)
	{
		checkingThread_->StopThread();
		checkingThread_->WaitForCompletion();
	}

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DWORD HTTPClient::Run()
{
	Debug::trace("HTTPClient", "HTTP Client thread is now running");


	// Initialize and setup the State Machine
	fsm_ = new stateMachine_type(*this);
	setupStateMachine();

	fsm_->start(new HTTPClientEvent(HTTPClientEvent::START_EVENT), INITIALIZING_STATE);


	fsm_->runStateMachine();
	if ( (*fsm_)() != stateMachine_type::STOPPED_STATE )
	{
		Debug::trace("HTTPClient", "StateMachine stopped running unexpectedly (" +
				fsm_->getStateName() + "). Last state is " + getStateName(fsm_->getCurrentState()) +
				" and last event is " + util_string::inttostring(fsm_->getCurrentEvent() ? fsm_->getCurrentEvent()->getId() : -1));
	}

	if ( pending_.size() )
	{
		Debug::trace("HTTPClient", "StateMachine is not running anymore, but there are " +
			util_string::inttostring(pending_.size()) + " pending HTTP Requests that will be dropped!!!");
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPClient::processingInitializingState(const HTTPClientEvent& )
{
#ifdef __linux__
	static const std::string sep( "/" );
#else
	static const std::string sep( "\\" );
#endif

	Configuration* config = Configuration::getInstance();

	_waitForRequestDelay_ = config->toIntCfgI(CfgInternal::HTTPWAITINGDELAY);
	_readResponseTimeout_ = config->toIntCfgI(CfgInternal::HTTPREADRESPONSETIMER);
	_readSomeResponseDelay_ = config->toIntCfgI(CfgInternal::HTTPREADSOMERESPONSEDELAY);
	_readNoResponseDelay_ = config->toIntCfgI(CfgInternal::HTTPREADNORESPONSEDELAY);
	_maxResponseLineLength_ = config->toIntCfgI(CfgInternal::HTTPMAXRESPONSELINELENGTH);

	Debug::trace("HTTPClient", "Initializing State: set up the following protocol-controlling parameters:");
	Debug::trace("HTTPClient", "\tDelay to wait for a request = " + util_string::inttostring(_waitForRequestDelay_) + " ms");
	Debug::trace("HTTPClient", "\tTimeout reading a response = " + util_string::inttostring(_readResponseTimeout_) + " s");
	Debug::trace("HTTPClient", "\tDelay to wait for a request after reading some data = " + util_string::inttostring(_readSomeResponseDelay_) + " ms");
	Debug::trace("HTTPClient", "\tDelay to wait for a request after not reading data = " + util_string::inttostring(_readNoResponseDelay_) + " ms");
	Debug::trace("HTTPClient", "\tMax response line length = " + util_string::uinttostring(_maxResponseLineLength_) + " octets");

	_useOfflineSpooling_= config->toBoolCfgI(CfgInternal::HTTPUSEOFFLINESPOOL);
	_offlineSpoolRetry_ = config->toIntCfgI(CfgInternal::HTTPOFFLINESPOOLRETRY) * 60;
	_offlineSpoolMaxRequestAge_ = config->toIntCfgI(CfgInternal::HTTPOFFLINESPOOLMAXREQUESTAGE) * 24 * 60 * 60;

	_offlineSpoolFile_ = config->toStringCfgI(CfgInternal::DBLOCATION);
	_offlineSpoolFile_.append( sep );
	_offlineSpoolFile_.append( "fppoffline.spool" );

	Debug::trace("HTTPClient", "\tOffline spooling enabled = " + util_string::inttostring(_useOfflineSpooling_) );
	Debug::trace("HTTPClient", "\tSpool retry time = " + util_string::inttostring(_offlineSpoolRetry_) + " s");
	Debug::trace("HTTPClient", "\tSpool max request age = " + util_string::inttostring(_offlineSpoolMaxRequestAge_) + " s");
	Debug::trace("HTTPClient", "\tSpool file location = " + _offlineSpoolFile_);

	// read in offline requests
	pendingList_type offlineRequests;

	if ( readOfflineRequests( offlineRequests ) )
	{
		Util::CLockCriticalSection lock(pendingMutex_);
		for ( pendingList_type::const_reverse_iterator i( offlineRequests.rbegin() ); i != offlineRequests.rend(); ++i )
			pending_.push_front( *i );
	}

	fsm_->postEvent(new event_type(event_type::INITIALIZED_OK_EVENT));

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPClient::processingWaitingState(const HTTPClientEvent& )
{
	const time_t now( time( NULL ) );

	if ( despoolTime_ <= (size_t) now )
	{
		while ( pendingHTTPRequestsSize() )
		{
			currentHTTPRequest_ = extractNextHTTPRequest();

			Debug::trace("HTTPClient", "Waiting State: succeeded (received new HTTP Request)");
			currentHTTPRequest_->showDebug();

			// make sure this request is not too old
			if ( _useOfflineSpooling_ )
				if ( currentHTTPRequest_->getTimestamp() < (now - _offlineSpoolMaxRequestAge_) )
				{
					Debug::trace("HTTPClient", "Waiting State: dropping request that has expired");
					removeNextHTTPRequest();
					continue;
				}

			fsm_->postEvent(new event_type(event_type::RECEIVED_REQUEST_EVENT));

			return true;
		}
	}

	PAL_absclass::PAL_sleep(_waitForRequestDelay_);
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPClient::processingConnectingState(const HTTPClientEvent& )
{
	util_time::Time t0;

	Debug::trace("HTTPClient", "Connecting State: started");

	if ( socket_.notNull() )
		Debug::trace("HTTPClient", "Connecting State: before connecting is not null?????");


	/* Establish a connection with the web server. */
	socketConnected_ = false;
	socket_ = new Socket();
	if ( !socket_->isValid() )
	{
		Debug::trace("HTTPClient", "Connecting State: failed. Unable to create socket.");

		fsm_->postEvent(new HTTPClientError(event_type::HTTPERROR_CANNOT_CREATE_SOCKET));

		return true;
	}

	if ( currentHTTPRequest_->isHostIPAddress() )
		socket_->setAddress(currentHTTPRequest_->getHost().c_str());
	else
		socket_->setDnsName(currentHTTPRequest_->getHost());
	const std::string& port = currentHTTPRequest_->getPort();
	socket_->setPort(port.size() ? util_string::stringtoint(port) : _DefaultHTTPServerPort_);
	socket_->setNonBlockingIO();

	Debug::trace("HTTPClient", "Connecting State: attempting to connect to " + currentHTTPRequest_->getHost() +
		" at port " + util_string::inttostring(socket_->getPort()));
	socket_->sConnect2();
	if ( !socket_->isValid() )
	{
		Debug::trace("HTTPClient", "Connecting State: failed. Unable to connect to target scale through \'" +
			currentHTTPRequest_->getHost() + "\'");

		fsm_->postEvent(new HTTPClientError(event_type::HTTPERROR_CANNOT_CONNECT_SOCKET));
	}
	else
	{
		util_time::Time t1;

		Debug::trace("HTTPClient", "Connecting State: successful. Connected to target scale through \'" +
				currentHTTPRequest_->getHost() + "\' (took " + util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " ms)");

		fsm_->postEvent(new event_type(event_type::CONNECTED_OK_EVENT));

		socketConnected_ = true;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPClient::processingSendingRequestState(const HTTPClientEvent& )
{
	util_time::Time t0;
	const std::string& requestBuffer = currentHTTPRequest_->buildRequestBuffer();
	int bytesToSend = (int)requestBuffer.size();

	Debug::trace("HTTPClient", "Going to send HTTP request of " +
		util_string::inttostring(bytesToSend) + " bytes:\n" + requestBuffer);


	int bytesSent = socket_->cmSend(requestBuffer.data(), bytesToSend);
	if ( bytesSent < bytesToSend )
	{
		Debug::trace("HTTPClient", "Sending Request State: attempted to send " +
				util_string::inttostring(bytesToSend) + " bytes but only sent " +
				util_string::inttostring(bytesSent) + " bytes.");

		Debug::trace("HTTPClient", "Sending Request State: failed to send data.");

		fsm_->postEvent(new HTTPClientError(event_type::HTTPERROR_WRITING_TO_SOCKET));
	}
	else
	{
		util_time::Time t1;

		Debug::trace("HTTPClient", "Sending Request State: completed successfully (took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " ms).");

		fsm_->postEvent(new event_type(event_type::REQUEST_SENT_OK_EVENT));
	}


	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPClient::processingSendingEntityState(const HTTPClientEvent& )
{
	if ( currentHTTPRequest_->isRequestEntity() )
	{
		/* We don't handle this case yet. */
		Debug::trace("HTTPClient", "Sending Entity State: succeeded (case not handled!!!)");

		requestSent_ = true;

		fsm_->postEvent(new event_type(event_type::ENTITY_SENT_OK_EVENT));
	}
	else
	{
		Debug::trace("HTTPClient", "Sending Entity State: succeeded (nothing to do)");

		requestSent_ = true;

		fsm_->postEvent(new event_type(event_type::ENTITY_SENT_OK_EVENT));
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPClient::enteringReadingResponseState(const HTTPClientEvent& event)
{
	if ( event.getId() == event_type::REQUEST_SENT_OK_EVENT )
	{
		responseLine_.clear();
		receivedResponse_ = false;
		receivedStatusLine_ = false;
		readingResponseTime_.getCurrentTime();
		lastReadTime_.getCurrentTime();
		currentHTTPResponse_ = new HTTPResponse(currentHTTPRequest_->getMethod());

		Debug::trace("HTTPClient", "Reading Response State: starting to read response");
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPClient::processingReadingResponseState(const HTTPClientEvent& event)
{
	bool entityComplete = ( event.getId() == event_type::ENTITY_SENT_OK_EVENT );

	if ( !receivedResponse_ )
	{
		bool readSomething = false;
		util_time::Time time1;
		if ( (time1 - readingResponseTime_).getTotalSeconds() > _readResponseTimeout_ )
		{
			if ( (time1 - lastReadTime_).getTotalSeconds() > 2 )
			{
				Debug::trace("HTTPClient", "Reading Response State: failed (timed out)");

				fsm_->postEvent(new HTTPClientError(event_type::HTTPERROR_TIMEOUT_READING_RESPONSE));

				return true;
			}
		}

		if ( socket_->cmNumMsgs() > 0 )
		{
			char buffer[1024];

			int bytesReceived = socket_->receive(buffer, sizeof(buffer));
			if ( bytesReceived > 0 )
			{
				bool cr = false, eol = false;
				std::string::size_type position, length = responseLine_.size();

				// Read something so take note of it and append it to the current line
				readSomething = true;
				lastReadTime_.getCurrentTime();
				responseLine_.append(buffer, bytesReceived);
				if ( length ) ++bytesReceived;
				length = responseLine_.size();

				position = length - bytesReceived;
				while ( position < length )
				{
					switch ( responseLine_[position] )
					{
					case '\x0A':
						eol = true;
						break;
					case '\x0D':
						if ( !cr )
						{
							cr = true;
							if ( position + 1 < length && responseLine_[position + 1] == '\x0A' )
								eol = true;
							break;
						}
						// else continue below
					default:
						if ( cr )
						{
							if ( !receivedStatusLine_ )
								Debug::trace("HTTPClient", "Reading Response State: failed (invalid character in response status line)");
							else
								Debug::trace("HTTPClient", "Reading Response State: failed (invalid character in response header line)");

							fsm_->postEvent(new HTTPClientError(event_type::HTTPERROR_INVALID_CHAR_RESPONSE));

							return true;
						}
					}

					if ( eol )
					{
						if ( position ) // If not an empty line
						{
							bool valid;

							if ( !receivedStatusLine_ )
							{
								valid = currentHTTPResponse_->processStatusLine(responseLine_.substr(0, position));
								receivedStatusLine_ = true;
							}
							else
								valid = currentHTTPResponse_->processHeaderLine(responseLine_.substr(0, position));

							if ( !valid )
							{
								Debug::trace("HTTPClient", "Reading Response State: failed (received invalid response)");
								currentHTTPResponse_->showDebug();

								fsm_->postEvent(new HTTPClientError(event_type::HTTPERROR_INVALID_RESPONSE));

								return true;
							}
						}
						else
						{
							if ( receivedStatusLine_ )
							{
								util_time::Time t1;
								Debug::trace("HTTPClient", "Reading Response State: succeeded (successfully read response in " +
										util_string::inttostring((t1 - readingResponseTime_).getTotalMilliseconds()) + " ms)");
								currentHTTPResponse_->showDebug();

								receivedResponse_ = true;
							}
							else
								Debug::trace("HTTPClient", "Reading Response State: received an empty line before the status line (skipping ...)");
						}

						responseLine_.erase(0, position + (cr ? 2 : 1));
						length = responseLine_.size();
						position = 0;
						cr = false;
						eol = false;

						if ( receivedResponse_ )
						{
							if ( length )
								Debug::trace("HTTPClient", "Reading Response State: there is more data in buffer after reading response headers.");

							if ( entityComplete )
								fsm_->postEvent(new event_type(event_type::READ_RESPONSE_OK_EVENT));
							else
								fsm_->postEvent(new event_type(event_type::SEND_ENTITY_EVENT));

							return true;
						}
					}
					else
						++position;
				}

				if ( length > _maxResponseLineLength_ )
				{
					Debug::trace("HTTPClient", "Reading Response State: failed (line too long)");

					fsm_->postEvent(new HTTPClientError(event_type::HTTPERROR_RESPONSE_LINE_TOO_LONG));

					return true;
				}
			}
		}

		if ( entityComplete )
		{
			PAL_absclass::PAL_sleep(readSomething ? _readSomeResponseDelay_ : _readNoResponseDelay_);
			return false;
		}
		else
		{
			fsm_->postEvent(new event_type(event_type::SEND_ENTITY_EVENT));
			return true;
		}
	}
	else
	{
		if ( entityComplete )
			fsm_->postEvent(new event_type(event_type::READ_RESPONSE_OK_EVENT));
		else
			fsm_->postEvent(new event_type(event_type::SEND_ENTITY_EVENT));

		return true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPClient::enteringReadingEntityState(const HTTPClientEvent& )
{
	responseEntityLength_ = currentHTTPResponse_->getContentLength();
	currentEntityLength_ = 0;
	readingResponseTime_.getCurrentTime();
	lastReadTime_.getCurrentTime();

	Debug::trace("HTTPClient", "Reading Entity State: starting to read entity");

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPClient::processingReadingEntityState(const HTTPClientEvent& )
{
	if ( currentHTTPResponse_->isResponseEntity() &&
		(currentEntityLength_ < responseEntityLength_ || responseEntityLength_ == -1) )
	{
		bool readSomething = false;
		util_time::Time time1;
		if ( (time1 - readingResponseTime_).getTotalSeconds() > _readResponseTimeout_ )
		{
			if ( (time1 - lastReadTime_).getTotalSeconds() > 2 )
			{
				Debug::trace("HTTPClient", "Reading Entity State: failed (timed out)");

				fsm_->postEvent(new HTTPClientError(event_type::HTTPERROR_TIMEOUT_READING_ENTITY));

				return true;
			}
		}

		std::string::size_type length = responseLine_.size();
		if ( length )
		{
			readSomething = true;
			currentEntityLength_ += (int)length;

			responseLine_.clear();
		}
		else
		{
			char buffer[1024];
			int bytesToRead = (responseEntityLength_ == -1 || responseEntityLength_ - currentEntityLength_ > 1024) ?
					1024 : responseEntityLength_ - currentEntityLength_;
			int bytesReceived = socket_->receive(buffer, bytesToRead);
			switch ( bytesReceived )
			{
			case 0:
				{
					if ( responseEntityLength_ == -1 )
					{
						readSomething = true;
						responseEntityLength_ = currentEntityLength_;

						break;
					}
					// else continue below
				}
			case -1:
				{
					// Error
					Debug::trace("HTTPClient", "Reading Entity State: failed");

					fsm_->postEvent(new HTTPClientError(event_type::HTTPERROR_READING_FROM_SOCKET));

					return true;
				}
			default:
				{
					if ( bytesReceived > 0 )
					{
						readSomething = true;
						currentEntityLength_ += bytesReceived;
					}

					break;
				}
			}
		}

		if ( readSomething )
		{
			if ( currentEntityLength_ < responseEntityLength_ || responseEntityLength_ == -1 )
			{
				PAL_absclass::PAL_sleep(_readSomeResponseDelay_);
				return false;
			}
			else
			{
				util_time::Time t1;

				Debug::trace("HTTPClient", "Reading Entity State: succeeded (took " +
						util_string::inttostring((t1 - readingResponseTime_).getTotalMilliseconds()) + " ms)");

				fsm_->postEvent(new event_type(event_type::ENTITY_READ_OK_EVENT));

				return true;
			}
		}
		else
		{
			PAL_absclass::PAL_sleep(_readNoResponseDelay_);
			return false;
		}
	}
	else
	{
		Debug::trace("HTTPClient", "Reading Entity State: succeeded (nothing to do)");

		fsm_->postEvent(new event_type(event_type::ENTITY_READ_OK_EVENT));
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPClient::processingProcessingResponseState(const HTTPClientEvent& )
{
	bool success = false;

	switch ( currentHTTPResponse_->getStatusCode()/100 )
	{
	case 1: // 1xx Informational
		Debug::trace("HTTPClient.response", "Processing Response State: Received an Information status code of " +
				util_string::inttostring(currentHTTPResponse_->getStatusCode()));

		break;

	case 2: // 2xx Success
		Debug::trace("HTTPClient.response", "Processing Response State: Received a Success status code of " +
				util_string::inttostring(currentHTTPResponse_->getStatusCode()));

		success = true;
		break;

	case 3: // 3xx Redirection

		Debug::trace("HTTPClient.response", "Processing Response State: Received a Redirection status code of " +
				util_string::inttostring(currentHTTPResponse_->getStatusCode()));

		break;

	case 4: // 4xx Client Error
		Debug::trace("HTTPClient.response", "Processing Response State: Received a Client Error status code of " +
				util_string::inttostring(currentHTTPResponse_->getStatusCode()));

		break;

	case 5: // 5xx Server Error
		Debug::trace("HTTPClient.response", "Processing Response State: Received a Server Error status code of " +
				util_string::inttostring(currentHTTPResponse_->getStatusCode()));

		break;

	default:
		Debug::trace("HTTPClient.response", "Processing Response State: Received an Unknown status code of " +
				util_string::inttostring(currentHTTPResponse_->getStatusCode()));

		break;
	}

	if ( success )
	{
		interface_.reportResult(success);

		// remove request from pending
		removeNextHTTPRequest();

		fsm_->postEvent(new event_type(event_type::RESPONSE_PROCESSED_OK_EVENT));
	}
	else
		fsm_->postEvent(new HTTPClientError(event_type::HTTPERROR_RESPONSE_NOT_SUCCESS));

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPClient::processingDisconnectingState(const HTTPClientEvent& )
{
	socket_->cmClose();
	socket_ = 0;
	socketConnected_ = false;

	currentHTTPRequest_ = 0;
	currentHTTPResponse_ = 0;

	Debug::trace("HTTPClient", "Disconnecting State: succeeded");

	fsm_->postEvent(new event_type(event_type::DISCONNECTED_OK_EVENT));

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPClient::processingConcludingState(const HTTPClientEvent& )
{
	/* Goodbye */
	fsm_->postEvent(new event_type(event_type::END_EVENT));

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPClient::processingProcessingErrorState(const HTTPClientEvent& event)
{
	bool needToDisconnect = false;
	bool needToRemove = true;

	const HTTPClientError* error = dynamic_cast<const HTTPClientError*>(&event);

	if ( error )
		Debug::trace("HTTPClient.error", "Processing Error State: error received " +
			error->getErrorText());
	else
		Debug::trace("HTTPClient.error", "Processing Error State: error received unknown");

	/* Report error condition to application. */
	interface_.reportResult( false );

	/* Do we need to remove request from pending */
	if ( _useOfflineSpooling_ )
	{
		if ( error )
		{
			switch ( error->getError() )
			{
			case HTTPClientError::HTTPERROR_CANNOT_CREATE_SOCKET:
			case HTTPClientError::HTTPERROR_CANNOT_CONNECT_SOCKET:
			case HTTPClientError::HTTPERROR_WRITING_TO_SOCKET:
			case HTTPClientError::HTTPERROR_TIMEOUT_READING_RESPONSE:
			case HTTPClientError::HTTPERROR_INVALID_CHAR_RESPONSE:
			case HTTPClientError::HTTPERROR_RESPONSE_LINE_TOO_LONG:
			case HTTPClientError::HTTPERROR_INVALID_RESPONSE:
			case HTTPClientError::HTTPERROR_EMPTY_RESPONSE:
			case HTTPClientError::HTTPERROR_TIMEOUT_READING_ENTITY:
			case HTTPClientError::HTTPERROR_READING_FROM_SOCKET:

				// we need to go offline for a period of time... stop sending requests!
				despoolTime_ = (size_t) time( NULL ) + _offlineSpoolRetry_;

				Debug::trace("HTTPClient.error", "Going offline until: " +
					util_string::inttostring(despoolTime_) );

				needToRemove = false;
				break;

			// According to Walmart (Sujit Behera) bad result codes are considered okay, they will be
			// handling these in their middleware layer.
			case HTTPClientError::HTTPERROR_RESPONSE_NOT_SUCCESS:
			default:
				needToRemove = true;
				break;
			}
		}
	}

	if ( needToRemove )
	{
		Debug::trace("HTTPClient.error", "Removing request from pending...");
		removeNextHTTPRequest();
	}

	/* Do we need to disconnect socket */
	if ( socketConnected_ )
	{
		needToDisconnect = true;

		if ( error )
		{
			switch ( error->getError() )
			{
			case HTTPClientError::HTTPERROR_CANNOT_CREATE_SOCKET:
			case HTTPClientError::HTTPERROR_CANNOT_CONNECT_SOCKET:
				needToDisconnect = false;
				break;

			default:
				needToDisconnect = true;
				break;
			}
		}
	}

	/* Continue to next state. */
	if ( needToDisconnect )
		fsm_->postEvent(new event_type(event_type::ERROR_DISCONNECT_EVENT));
	else
		fsm_->postEvent(new event_type(event_type::ERROR_WAIT_EVENT));

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t HTTPClient::readOfflineRequests( pendingList_type& list ) const
{
	// offline spooling disabled
	if ( !_useOfflineSpooling_ )
		return 0;

	// read all requests
	std::ifstream spool( _offlineSpoolFile_.c_str(), std::ios::in | std::ios::binary );

	if ( !spool.is_open() )
		return 0;

	try
	{
		size_t numRecords( 0 );
		spool.read( (char*) &numRecords, sizeof(numRecords) );

		while ( numRecords-- )
		{
			Util::CSmartPtr<HTTPRequest> ptr( new HTTPRequest );
			spool >> (*ptr);

			list.push_back( ptr );
		}
	}
	catch ( ... )
	{
		Debug::trace("HTTPClient", "Error reading spool file" );
	}

	// close file
	spool.close();

	// return number of requests
	return list.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPClient::writeOfflineRequests( const pendingList_type& list ) const
{
	// offline spooling disabled
	if ( !_useOfflineSpooling_ )
		return true;

	std::string spoolfile( _offlineSpoolFile_ );
	std::string spoolfileBackup( spoolfile + ".bak" );

	int rc;

	// remove backup
	if ( -1 != access( spoolfileBackup.c_str(), 0 ) )
		remove( spoolfileBackup.c_str() );

	// backup old file
	if ( -1 != access( spoolfile.c_str(), 0 ) )
		if ( 0 != (rc = rename( spoolfile.c_str(), spoolfileBackup.c_str() )) )
		{
			Debug::trace("HTTPClient", "Failed to backup spool file: " + util_string::inttostring(rc) );
			return false;
		}

	// write all requests
	std::ofstream spool( spoolfile.c_str(), std::ios::out | std::ios::trunc | std::ios::binary );

	if ( !spool.is_open() )
	{
		Debug::trace("HTTPClient", "Failed to backup spool file: " + util_string::inttostring(rc) );
		goto recovery;
	}
	else
	{
		size_t numRecords( list.size() );
		spool.write( (char*) &numRecords, sizeof(numRecords) );

		for ( pendingList_type::const_iterator i( list.begin() ); i != list.end(); ++i )
			spool << *(*i);
	}

	spool.close();

	return true;

recovery:

	// restore backup
	if ( 0 != (rc = rename( spoolfileBackup.c_str(), spoolfile.c_str() )) )
		Debug::trace("HTTPClient", "Failed to restore backup: " + util_string::inttostring(rc) );

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPClient::setupStateMachine()
{
	// Add states
	fsm_->addState(INITIALIZING_STATE, &HTTPClient::processingInitializingState);
	fsm_->addState(WAITING_STATE, &HTTPClient::processingWaitingState);
	fsm_->addState(CONNECTING_STATE, &HTTPClient::processingConnectingState);
	fsm_->addState(SENDING_REQUEST_STATE, &HTTPClient::processingSendingRequestState);
	fsm_->addState(SENDING_ENTITY_STATE, &HTTPClient::processingSendingEntityState);
	fsm_->addState(READING_RESPONSE_STATE, &HTTPClient::processingReadingResponseState, &HTTPClient::enteringReadingResponseState);
	fsm_->addState(READING_ENTITY_STATE, &HTTPClient::processingReadingEntityState, &HTTPClient::enteringReadingEntityState);
	fsm_->addState(PROCESSING_RESPONSE_STATE, &HTTPClient::processingProcessingResponseState);
	fsm_->addState(DISCONNECTING_STATE, &HTTPClient::processingDisconnectingState);
	fsm_->addState(CONCLUDING_STATE, &HTTPClient::processingConcludingState);
	fsm_->addState(PROCESSING_ERROR_STATE, &HTTPClient::processingProcessingErrorState);

	// Add INITIALIZING_STATE transitions
	fsm_->addTransition(INITIALIZING_STATE, HTTPClientEvent::INITIALIZED_OK_EVENT, WAITING_STATE);

	// Add WAITING_STATE transitions
	fsm_->addTransition(WAITING_STATE, HTTPClientEvent::RECEIVED_REQUEST_EVENT, CONNECTING_STATE);
	fsm_->addTransition(WAITING_STATE, HTTPClientEvent::CONCLUDE_EVENT, CONCLUDING_STATE);

	// Add CONNECTING_STATE transitions
	fsm_->addTransition(CONNECTING_STATE, HTTPClientEvent::CONNECTED_OK_EVENT, SENDING_REQUEST_STATE);

	// Add SENDING_REQUEST_STATE transitions
	fsm_->addTransition(SENDING_REQUEST_STATE, HTTPClientEvent::REQUEST_SENT_OK_EVENT, READING_RESPONSE_STATE);

	// Add SENDING_ENTITY_STATE transitions
	fsm_->addTransition(SENDING_ENTITY_STATE, HTTPClientEvent::SENDING_ENTITY_OK_EVENT, READING_RESPONSE_STATE);
	fsm_->addTransition(SENDING_ENTITY_STATE, HTTPClientEvent::ENTITY_SENT_OK_EVENT, READING_RESPONSE_STATE);

	// Add READING_RESPONSE_STATE transitions
	fsm_->addTransition(READING_RESPONSE_STATE, HTTPClientEvent::SEND_ENTITY_EVENT, SENDING_ENTITY_STATE);
	fsm_->addTransition(READING_RESPONSE_STATE, HTTPClientEvent::READ_RESPONSE_OK_EVENT, READING_ENTITY_STATE);

	// Add READING_ENTITY_STATE transitions
	fsm_->addTransition(READING_ENTITY_STATE, HTTPClientEvent::ENTITY_READ_OK_EVENT, PROCESSING_RESPONSE_STATE);

	// Add PROCESSING_RESPONSE_STATE transitions
	fsm_->addTransition(PROCESSING_RESPONSE_STATE, HTTPClientEvent::NEXT_REQUEST_EVENT, SENDING_REQUEST_STATE);
	fsm_->addTransition(PROCESSING_RESPONSE_STATE, HTTPClientEvent::RESPONSE_PROCESSED_OK_EVENT, DISCONNECTING_STATE);

	// Add PROCESSING_ERROR_STATE transitions
	fsm_->addTransition(PROCESSING_ERROR_STATE, HTTPClientEvent::ERROR_DISCONNECT_EVENT, DISCONNECTING_STATE);
	fsm_->addTransition(PROCESSING_ERROR_STATE, HTTPClientEvent::ERROR_WAIT_EVENT, WAITING_STATE);

	// Add DISCONNECTING_STATE transitions
	fsm_->addTransition(DISCONNECTING_STATE, HTTPClientEvent::DISCONNECTED_OK_EVENT, WAITING_STATE);

	// Add CONCLUDING_STATE transitions
	fsm_->addTransition(CONCLUDING_STATE, HTTPClientEvent::END_EVENT);

	// Add default transitions
	fsm_->addDefaultTransition(HTTPClientEvent::ERROR_EVENT, PROCESSING_ERROR_STATE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const std::string& HTTPClient::getStateName(const stateMachine_type::state_type* state)
{
	if ( state )
	{
		switch ( state->getId() )
		{
		case INITIALIZING_STATE:
			return _statenameInitializing_;
		case WAITING_STATE:
			return _statenameWaiting_;
		case CONNECTING_STATE:
			return _statenameConnecting_;
		case SENDING_REQUEST_STATE:
			return _statenameSendingRequest_;
		case SENDING_ENTITY_STATE:
			return _statenameSendingEntity_;
		case READING_RESPONSE_STATE:
			return _statenameReadingResponse_;
		case READING_ENTITY_STATE:
			return _statenameReadingEntity_;
		case PROCESSING_RESPONSE_STATE:
			return _statenameProcessingResponse_;
		case DISCONNECTING_STATE:
			return _statenameDisconnecting_;
		case CONCLUDING_STATE:
			return _statenameConcluding_;
		case PROCESSING_ERROR_STATE:
			return _statenameProcessingError_;
		default:
			break;
		}
	}

	return _statenameUnknown_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // comm
