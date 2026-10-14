/* Now Serving Queue Management
 * COPYRIGHT (c) 2014.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef NOWSERVINGQUEUEMANAGEMENTMESSAGE_H
#define NOWSERVINGQUEUEMANAGEMENTMESSAGE_H

#include "HfcObject.h"

#include <list>
#include <map>
#include <string>

class Session;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Message for handling now serving/take a number queues
class NowServingQueueManagementMessage : public HfcObject
{
	typedef NowServingQueueManagementMessage _Myt;
	typedef HfcObject _Mybase;

public:

	/// Message action types.
	enum actionType
	{
		Unknown = 0,								///< Unknown (or invalid) action type.

		// customer queue actions
		ServingNextCustomer = 1,					///< Display on GCD now serving customer, pop customer off the queue.
		GetNextCustomerInLine,						///< Retrieve next customer in line, do not adjust the queue.
		DeleteAllCustomersInLine,					///< Delete all queued customers.
		GetCustomerQueueSize,						///< Get customer queue size.
		ServeSpecificCustomer,						///< Display on GCD now serving customer X, remove that customer from queue.
	    DismissCustomerAttractDialog,				///< Dismiss customer attract dialog box on GCD.
	    CancelServeCustomer,						///< Cancel serve of customer, add them to front of queue.

		// printer actions
		PrintCustomerNumber = 26,					///< PRINT next customer number, increment number.
		SetNextCustomerNumber,						///< Set next printed customer number to X.
		GetNextCustomerNumber,						///< Get next printed customer number, DONT INCREMENT, DONT PRINT.
		IncrementCustomerNumber,					///< PULL customer number, increment number. Used for paper rolls.

		// reset action
		ResetAllQueues = 51,						///< Resets all queues.
		ResetQueue									///< Reset next printed customer to X, delete all queued customers.
	};

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] data  pointer to data
	 * @param[in] length  data length
	 * @param[in] formatType  format type
	 */
	NowServingQueueManagementMessage( const char *data, int len, int formatType );

	/// Constructor.
	/**
	 * @param[in] rhs  object
	 */
	NowServingQueueManagementMessage( const _Myt& rhs );

	/// Destructor.
	~NowServingQueueManagementMessage();

	// ========================================================================
	// Operators
	// ========================================================================

	/// Assignment operator.
	/**
	 * @param[in] rhs  object
	 * @return  reference to this
	 */
	_Myt& operator = ( const _Myt& rhs );

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve queue action type.
	/**
	 * @return  action type
	 */
	actionType action() const {return action_;}

	/// Retrieve result code.
	/**
	 * @return  value
	 */
	int resultCode() const {return resultCode_;}

	/// Retrieve queue values.
	/**
	 * @return  values
	 */
	std::list<std::string> values() const {return values_;}

	/// Set queue action type.
	/**
	 * @param[in] value  action type
	 */
	void setAction( actionType value ) {action_ = value;}

	/// Set result code.
	/**
	 * @param[in] value  result code
	 */
	void setResultCode( int value ) {resultCode_ = value;}

	/// Set queue values.
	/**
	 * @param[in] value  values
	 */
	void setValues( const std::list<std::string>& value ) {values_ = value;}

	// ========================================================================
	// Methods
	// ========================================================================

	/// Process message.
	/**
	 * @param[in] session  session object
	 * @return  @c true on success, @c false otherwise
	 */
	virtual bool process( Session *session );

	/// Serialize message.
	/**
	 * @param[in] format  format type
	 * @return  buffer
	 */
	virtual const char *serialize( int format = BrowserFormat ) const;

private:

#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif

	enum fieldTypes
	{
		fieldAction = 1,
		fieldResultCode,
		fieldValue1,
		fieldValue2,
		fieldValue3,
		fieldValue4,
		MAX_FIELDS
	};

	actionType action_;
	std::list<std::string> values_;

	int resultCode_;

	typedef std::map<actionType, std::string> actionMap;
	actionMap internalActions_;
	actionMap externalActions_;

	/// Initialize.
	void init();

	/// Copy object.
	void copy( const _Myt& rhs );

	/// Process internal action.
	bool processInternalAction( Session *session, const std::string& a );

	/// Process external action.
	bool processExternalAction( Session *session, const std::string& a );

	/// Connect to server.
	static bool connectToServer( const std::string& command, const std::list<std::string>& params, std::string& result );

	/// Parse server response.
	static bool parseResponse( const std::string& response, int& returnCode, std::list<std::string>& returnValues );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // NOWSERVINGQUEUEMANAGEMENTMESSAGE_H
