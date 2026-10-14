/* Now Serving Queue Management
 * COPYRIGHT (c) 2014.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "NowServingQueueManagementMessage.h"

#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Session.h"

#include "Configuration.h"
#include "PythonScripter.h"

#include "utilstring.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NowServingQueueManagementMessage::dataType = 0;
std::string NowServingQueueManagementMessage::dataTypeName("NowServingQueueManagementMessage");
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
NowServingQueueManagementMessage::NowServingQueueManagementMessage( const char *data, int length, int formatType ) :
	_Mybase()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch ( formatType )
	{
	case BrowserFormat:
	{
		int f( 1 );

		HobString temp( data, length);
		HobTokenizerCSV tok( &temp, BrowserFormatSeparator );
		HobString token;

		std::list<std::string> valueList;

		while ( -1 != tok.next( &token ) )
		{
			setReceived( f, (0 == token.length()) ? false : true );

			switch ( f++ )
			{
			case fieldAction:
				setAction( (actionType) Integer::valueOf( token ) );
				break;
			case fieldResultCode:
				setResultCode( Integer::valueOf( token ) );
				break;
			case fieldValue1:
			case fieldValue2:
			case fieldValue3:
			case fieldValue4:
				valueList.push_back( token );
				break;
			default:
				break;
			}
		}

		setValues( valueList );

		break;
	}

	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
NowServingQueueManagementMessage::NowServingQueueManagementMessage( const _Myt& rhs ) :
	_Mybase( *this )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	copy( rhs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
NowServingQueueManagementMessage::~NowServingQueueManagementMessage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
NowServingQueueManagementMessage& NowServingQueueManagementMessage::operator = ( const _Myt& rhs )
{
	copy( rhs );
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool NowServingQueueManagementMessage::process( Session *session )
{
	Configuration *config( Configuration::getInstance() );
	actionMap::iterator a;

	bool result( false );

	// check for licensed feature, exit if not present
	if ( !Configuration::getInstance()->isPackageLicensed( "SCALE_NOW_SERVING" ) )
	{
		Debug::trace( "NowServing",
			HobString( "Feature is not licensed!" ) );

		values_.clear();
		resultCode_ = -1;
	}

	// these actions can only be performed from primary session
	else if (( !session->isPrimarySession() ) &&
		(( ServingNextCustomer == action() ) ||
		 ( ServeSpecificCustomer == action() ) ||
		 ( PrintCustomerNumber == action() )))
	{
		Debug::trace( "NowServing",
			HobString( "Action restricted to primary session only: " ) + Integer::toString( (int) action() ) );

		values_.clear();
		resultCode_ = -2;
	}

	// internal action
	else if ( internalActions_.end() != (a = internalActions_.find( action() )) )
		result = processInternalAction( session, a->second );

	// external action
	else if ( externalActions_.end() != (a = externalActions_.find( action() )) )
		result = processExternalAction( session, a->second );

	// unknown action
	else
	{
		Debug::trace( "NowServing",
			HobString( "Unable to process message: " ) + Integer::toString( (int) action() ) +
			HobString( " [primary " ) + Integer::toString( session->isPrimarySession() ) + HobString( "]" ) );
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const char *NowServingQueueManagementMessage::serialize( int format ) const
{
	serializedData = "";

	if ( UnknownFormat != getFormatType() )
		format = getFormatType();

	std::list<std::string> valueList( values() );

	switch ( format )
	{
	case BrowserFormat:

		for ( int f( 1 ); f < MAX_FIELDS; ++f )
		{
			if ( 1 < f )
				serializedData.append( BrowserFormatSeparator );

			switch ( f )
			{
			case fieldAction:
				serializedData.append( Integer::toString( (int) action() ) );
				break;
			case fieldResultCode:
				serializedData.append( Integer::toString( resultCode() ) );
				break;
			case fieldValue1:
			case fieldValue2:
			case fieldValue3:
			case fieldValue4:
				if ( !valueList.empty() )
				{
					serializedData.append( valueList.front() );
					valueList.pop_front();
				}
				break;
			default:
				break;
			}
		}

		break;

	default:
		break;
	}

	return serializedData.c_str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NowServingQueueManagementMessage::init()
{
	action_ = Unknown;
	values_.clear();
	resultCode_ = 0;

	// internal actions
	internalActions_[DismissCustomerAttractDialog] = "dismiss_gcd_dialog";

	// external actions
	externalActions_[PrintCustomerNumber] = "print_next_ticket";
	externalActions_[ServingNextCustomer] = "serve_next_customer";
	externalActions_[SetNextCustomerNumber] = "set_serving_number";
	externalActions_[GetNextCustomerInLine] = "get_next_customer";
	externalActions_[GetCustomerQueueSize] = "get_queue_size";
	externalActions_[DeleteAllCustomersInLine] = "clear_queue";
	externalActions_[GetNextCustomerNumber] = "get_serving_number";
	externalActions_[ServeSpecificCustomer] = "serve_customer";
	externalActions_[CancelServeCustomer] = "cancel_serve_customer";
	externalActions_[IncrementCustomerNumber] = "increment_serving_number";
	externalActions_[ResetAllQueues] = "reset_all_queues";
	externalActions_[ResetQueue] = "reset_queue";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NowServingQueueManagementMessage::copy( const _Myt& rhs )
{
	action_ = rhs.action_;
	values_ = rhs.values_;
	resultCode_ = rhs.resultCode_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool NowServingQueueManagementMessage::processInternalAction( Session *session, const std::string& a )
{
	// dismiss the gcd attract overlay
	if ( "dismiss_gcd_dialog" == a )
		return true;

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool NowServingQueueManagementMessage::processExternalAction( Session *session, const std::string& a )
{
	std::string response;

	// go!
	bool result( connectToServer( a, values_, response ) );

	std::list<std::string> returnValues;
	int returnCode;

	// parse response
	if ( parseResponse( response, returnCode, returnValues ) )
	{
		std::list<std::string> prevValue( values_ );

		values_ = returnValues;
		resultCode_ = returnCode;

		// for serve specific customer, return back the customer number
		if (( 0 == resultCode_ ) && ( ServeSpecificCustomer == action_ ))
			values_ = prevValue;
	}
	else
	{
		Debug::trace( "NowServing",
			HobString( "Bad response from server!" ) );

		values_.clear();
		resultCode_ = -20;
		result = false;
	}

done:

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool NowServingQueueManagementMessage::connectToServer( const std::string& command, const std::list<std::string>& params, std::string& out )
{
	Configuration *config( Configuration::getInstance() );

	bool result( false );

	std::string ip( config->toStringCfg( fieldOffsetCfgNowServingQueueIPAddress ) );
	std::string port( config->toStringCfgI( fieldOffsetCfgISmBackendPort ) );
	std::string queue( "default" );

	// make sure python turned on
	if ( !config->toBoolCfgI( CfgInternal::PYTHONENABLED ) )
	{
		Debug::trace( "NowServing", HobString( "Python not enabled!" ) );
		out = "error|-10|Invalid configuration";
	}

	// validate server
	else if (( 0 == ip.length() ) || ( "0.0.0.0" == ip ) || ( "..." == ip ))
	{
		Debug::trace( "NowServing", HobString( "Server not configured" ) );
		out = "error|-11|Invalid server address";
	}

	else
	{
		static const std::string scriptName( "nowServingClient.py" );

		// build up our queue function call
		std::string fn;
		fn += command;
		fn += "|";
		fn += queue;

		for ( std::list<std::string> valueList( params ); valueList.size(); valueList.pop_front() )
		{
			fn += "|";
			fn += valueList.front();
		}

		// build python script parameters
		std::string scriptParam;
		scriptParam += ip;
		scriptParam += " ";
		scriptParam += port;
		scriptParam += " ";
		scriptParam += "\"";
		scriptParam += fn;
		scriptParam += "\"";

		// build python script command
		const std::string cmd( PythonScripter::buildPythonCall( scriptName, -1, scriptParam ) );

		Debug::trace( "NowServing.script", cmd );

		// go!
		int rc( PAL_absclass::PAL_executeWithResults( cmd, out ) );

		Debug::trace( "NowServing.script", Integer::toString( rc ) + HobString( " - " ) + HobString( out ) );

		if ( !(result = (0 == rc)) )
		{
			std::string err( Integer::toString( rc ) );

			Debug::trace( "NowServing", HobString( "Script error: " ) + err );
			out = "error|-12|Script execution error: " + err;
		}
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool NowServingQueueManagementMessage::parseResponse( const std::string& response, int& returnCode, std::list<std::string>& returnValues )
{
	std::string responseTrimmed( response );
	util_string::trim( responseTrimmed );

	std::vector<std::string> v( util_string::split( responseTrimmed, '|' ) );

	// log out response
	if ( Debug::isTracing( "NowServing.script"  ) )
	{
		for ( std::vector<std::string>::size_type i( 0 ); i < v.size(); ++i )
			Debug::trace( "NowServing.script",
				HobString( "'" ) + v[i] + HobString( "'" ) );
	}

	// validate response
	if ( 1 <= v.size() )
	{
		returnValues.clear();

		if ( "error" == v[0] )
		{
			if ( 3 <= v.size() )
			{
				returnCode = atoi( v[1].c_str() );
				returnValues.push_back( v[2] );
				return true;
			}
		}
		else if ( "success" == v[0] )
		{
			for ( std::vector<std::string>::size_type i( 1 ); i < v.size(); ++i )
				returnValues.push_back( v[i] );

			returnCode = 0;
			return true;
		}
	}

	return false;
}

