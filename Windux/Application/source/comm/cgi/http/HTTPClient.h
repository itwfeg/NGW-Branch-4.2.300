/**
 * @file HTTPClient.h
 * HTTP client thread class.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Santos Juan
 * @author Randy Blankley
 */

#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "CSmartPtr.h"
#include "CTLSThread.h"
#include "HTTPClientEvent.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "HobSocket.h"

#include "StateMachine/StateMachine.h"
#include "StateMachine/StateMachineController.h"

#include "util/time/dbtime.h"

#include <string>

namespace comm
{

class CGICallerI;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HTTP client thread class.
class HTTPClient : public Util::CRunnableClass, public IStateMachineController
{
	typedef HTTPClient _Myt;
	typedef Util::CRunnableClass _Mybase;

	typedef IStateMachineController _Myimpl;

public:

	typedef std::list< Util::CSmartPtr<HTTPRequest> > pendingList_type;

	typedef enum
	{
		INITIALIZING_STATE,
		WAITING_STATE,
		CONNECTING_STATE,
		SENDING_REQUEST_STATE,
		SENDING_ENTITY_STATE,
		READING_RESPONSE_STATE,
		READING_ENTITY_STATE,
		PROCESSING_RESPONSE_STATE,
		DISCONNECTING_STATE,
		CONCLUDING_STATE,
		PROCESSING_ERROR_STATE

	} stateId_type;

	typedef HTTPClientEvent event_type;

	typedef StateMachine<HTTPClient> stateMachine_type;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	HTTPClient(CGICallerI& inter);
	virtual ~HTTPClient();

	// ========================================================================
	// Properties
	// ========================================================================

	size_t pendingHTTPRequestsSize() const {
		Util::CLockCriticalSection lock(pendingMutex_);
		return pending_.size();
	}

	// ========================================================================
	// Methods
	// ========================================================================

	void addHTTPRequest(Util::CSmartPtr<HTTPRequest> request) {
		Util::CLockCriticalSection lock(pendingMutex_);
		pending_.push_back(request);

		writeOfflineRequests( pending_ );
	}

	DWORD Run();

private:

	/* --- Instance Data Members --- */
	CGICallerI& interface_;
	Util::CSmartPtr<Util::CTLSThread> checkingThread_;
	Util::CSmartPtr<stateMachine_type> fsm_;

	pendingList_type pending_;
	mutable Util::CSmartCriticalSection pendingMutex_;

	bool socketConnected_;
	bool requestSent_;
	bool receivedStatusLine_;
	bool receivedResponse_;
	int responseEntityLength_, currentEntityLength_;
	std::string responseLine_;
	util_time::Time readingResponseTime_, lastReadTime_;
	Util::CSmartPtr<HTTPRequest> currentHTTPRequest_;
	Util::CSmartPtr<HTTPResponse> currentHTTPResponse_;
	Util::CSmartPtr<Socket> socket_;

	size_t despoolTime_;


	/* --- Constructors and destructor --- */
	// Copy constructor is not available
	HTTPClient(const HTTPClient&);


	/* --- Instance Function Members --- */
	// Do not allow the default assignment operator.
	HTTPClient operator=(const HTTPClient& rhs);

	size_t readOfflineRequests( pendingList_type& list ) const;
	bool writeOfflineRequests( const pendingList_type& list ) const;

	void setupStateMachine();
	const std::string& getStateName(const stateMachine_type::state_type* state);

	Util::CSmartPtr<HTTPRequest> extractNextHTTPRequest() {
		Util::CLockCriticalSection lock(pendingMutex_);
		return pending_.front();
	}

	void removeNextHTTPRequest() {
		Util::CLockCriticalSection lock(pendingMutex_);
		pending_.pop_front();

		writeOfflineRequests( pending_ );
	}

	// Action functions
	bool processingInitializingState(const HTTPClientEvent& event);
	bool processingWaitingState(const HTTPClientEvent& event);
	bool processingConnectingState(const HTTPClientEvent& event);
	bool processingSendingRequestState(const HTTPClientEvent& event);
	bool processingSendingEntityState(const HTTPClientEvent& event);
	bool enteringReadingResponseState(const HTTPClientEvent& event);
	bool processingReadingResponseState(const HTTPClientEvent& event);
	bool enteringReadingEntityState(const HTTPClientEvent& event);
	bool processingReadingEntityState(const HTTPClientEvent& event);
	bool processingProcessingResponseState(const HTTPClientEvent& event);
	bool processingDisconnectingState(const HTTPClientEvent& event);
	bool processingConcludingState(const HTTPClientEvent& event);
	bool processingProcessingErrorState(const HTTPClientEvent& event);


	/* --- Class Data Members --- */
	static int _waitForRequestDelay_;
	static int _readResponseTimeout_;
	static int _readSomeResponseDelay_;
	static int _readNoResponseDelay_;
	static unsigned int _maxResponseLineLength_;

	static bool _useOfflineSpooling_;
	static unsigned int _offlineSpoolRetry_;
	static unsigned int _offlineSpoolMaxRequestAge_;
	static std::string _offlineSpoolFile_;

	static const std::string _ThreadName_;
	static const int _DefaultHTTPServerPort_;

	static const std::string _statenameInitializing_;
	static const std::string _statenameWaiting_;
	static const std::string _statenameConnecting_;
	static const std::string _statenameSendingRequest_;
	static const std::string _statenameSendingEntity_;
	static const std::string _statenameReadingResponse_;
	static const std::string _statenameReadingEntity_;
	static const std::string _statenameProcessingResponse_;
	static const std::string _statenameDisconnecting_;
	static const std::string _statenameConcluding_;
	static const std::string _statenameProcessingError_;
	static const std::string _statenameUnknown_;

#ifdef USE_ACCOUNTANT
	static int _dataType_;
	static std::string _dataTypeName_;
#endif

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // comm

#endif //HTTPCLIENT_H
