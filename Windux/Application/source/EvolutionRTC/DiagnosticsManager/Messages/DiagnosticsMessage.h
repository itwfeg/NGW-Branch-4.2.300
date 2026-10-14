/**
 * @file DiagnosticsMessage.h
 * @brief Base class for diagnostic messages.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSMESSAGE_H
#define DIAGNOSTICSMESSAGE_H

#include "DiagnosticsEvent.h"

#include <EvolMessage.h>

#include <vector>

namespace Hobart {
	class CEvolMsgReceiver;
}

#define APPEND_FIELD( var, defaultValue ) { \
	field f = { DiagnosticsMessage::typeOf( var ), &(var = defaultValue), sizeof(var) }; \
	fields.push_back( f ); }

#define SET_FIELDS \
	fieldVector fields; \
	appendFields( fields ); \
	setFields( fields );

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Base class for diagnostic messages.
class DiagnosticsMessage : public EvolMessage
{
	typedef DiagnosticsMessage _Myt;
	typedef EvolMessage _Mybase;

public:

	// ========================================================================
	// DTOR
	// ========================================================================

	/// Destructor.
	virtual ~DiagnosticsMessage();

	// ========================================================================
	// Methods
	// ========================================================================

	/// Deserialize message.
	/**
	 * @note
	 * Message must be in InternalFormat.
	 *
	 * @param[in] buffer  message buffer
	 * @return  buffer index
	 */
	virtual unsigned int deserialize( const char *buffer );

	/// Deserialize message.
	/**
	 * @param[in] buffer  message buffer
	 * @param[in] bufferLen  message buffer length
	 * @param[in] format  buffer format
	 * @return  buffer index
	 */
	virtual unsigned int deserialize( const char *buffer, size_t bufferLen, int format );

	/// Serialize message.
	/**
	 * @note
	 * Serialize to InternalFormat.
	 *
	 * @return  pointer to buffer
	 */
	virtual const char *serialize() const;

	/// Serialize message.
	/**
	 * @param[in] format  message format
	 * @return  pointer to buffer
	 */
	virtual const char *serialize( int format ) const;

	/// Show message.
	virtual void show();

protected:

	/// Message field types.
	enum fieldType
	{
		ftBoolean,
		ftInteger,
		ftUnsignedInteger,
		ftReal,
		ftString
	};

	/// Message field.
	struct field
	{
		fieldType type;
		void *ptr;
		size_t len;
	};

	/// Vector of message fields.
	typedef std::vector<field> fieldVector;

	// ========================================================================
	// CTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] event  event type
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsMessage( DiagEvent_t event, Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 );

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve fields.
	/**
	 * @return  message fields
	 */
	virtual fieldVector fields() const {return fields_;}

	/// Set fields.
	/**
	 * @param[in] fields  message fields
	 */
	virtual void setFields( const fieldVector &fields ) {fields_ = fields;}

	// ========================================================================
	// Methods
	// ========================================================================

	/// Append message specific fields.
	/**
	 * @param[in,out] fields  message fields
	 */
	virtual void appendFields( fieldVector& ) {}

	// ========================================================================
	// Static Methods
	// ========================================================================

	template <class T>
	fieldType typeOf( const T& );

private:

	fieldVector fields_;

#ifdef USE_ACCOUNTANT
	static int dataType_;
	static std::string dataTypeName_;
#endif

	static std::string category_;

	/// Cast field to string value.
	static std::string toString( const field &f, bool quoteForCSV = false );

};

template <>
inline DiagnosticsMessage::fieldType DiagnosticsMessage::typeOf( const bool& ) {return ftBoolean;}

template <>
inline DiagnosticsMessage::fieldType DiagnosticsMessage::typeOf( const char& ) {return ftInteger;}

template <>
inline DiagnosticsMessage::fieldType DiagnosticsMessage::typeOf( const short& ) {return ftInteger;}

template <>
inline DiagnosticsMessage::fieldType DiagnosticsMessage::typeOf( const int& ) {return ftInteger;}

template <>
inline DiagnosticsMessage::fieldType DiagnosticsMessage::typeOf( const long& ) {return ftInteger;}

template <>
inline DiagnosticsMessage::fieldType DiagnosticsMessage::typeOf( const unsigned char& ) {return ftUnsignedInteger;}

template <>
inline DiagnosticsMessage::fieldType DiagnosticsMessage::typeOf( const unsigned short& ) {return ftUnsignedInteger;}

template <>
inline DiagnosticsMessage::fieldType DiagnosticsMessage::typeOf( const unsigned int& ) {return ftUnsignedInteger;}

template <>
inline DiagnosticsMessage::fieldType DiagnosticsMessage::typeOf( const unsigned long& ) {return ftUnsignedInteger;}

template <>
inline DiagnosticsMessage::fieldType DiagnosticsMessage::typeOf( const float& ) {return ftReal;}

template <>
inline DiagnosticsMessage::fieldType DiagnosticsMessage::typeOf( const double& ) {return ftReal;}

template <>
inline DiagnosticsMessage::fieldType DiagnosticsMessage::typeOf( const std::string& ) {return ftString;}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSMESSAGE_H
