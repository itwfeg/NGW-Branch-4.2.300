/**
 * @file DiagnosticsMessage.cpp
 * @brief Base class for diagnostic messages.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "DiagnosticsMessage.h"

#include <MessageFormat.h>

#include <Bool.h>
#include <Double.h>
#include <HobString.h>
#include <HobTokenizerCSV.h>
#include <Long.h>
#include <ULong.h>

#include <evolution.h>
#include <utilstring.h>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int DiagnosticsMessage::dataType_ = 0;
std::string DiagnosticsMessage::dataTypeName_( "rtc.diagnosticsmanager.messages.diagnosticsmessage" );
#endif

std::string DiagnosticsMessage::category_( "DiagnosticsManager.messages" );

#define TRACE( logline ) Debug::trace( category_, (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagnosticsMessage::DiagnosticsMessage( DiagEvent_t event, Hobart::CEvolMsgReceiver *thread, unsigned int seqNumber ) :
	_Mybase( Diagnostics, event, thread, seqNumber )
{
#ifdef USE_ACCOUNTANT
	if ( !dataType )
		dataType_ = util_acc::registerDataType( dataTypeName_ );

	util_acc::addInstance( dataType_ );
#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagnosticsMessage::~DiagnosticsMessage()
{

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int DiagnosticsMessage::deserialize( const char *buffer )
{
	return deserialize( buffer, 0, InternalFormat );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int DiagnosticsMessage::deserialize( const char *buffer, size_t bufferLen, int format )
{
	size_t index( 0 );

	if ( !buffer )
		return 0;

	if ( InternalFormat == format )
	{
		index = _Mybase::deserialize( buffer );

		for ( fieldVector::const_iterator field( fields_.begin() ); field != fields_.end(); ++field )
		{
			if ( ftString == field->type )
			{
				size_t len( *reinterpret_cast<const size_t*>( &buffer[index] ) );
				index += sizeof(size_t);

				if ( !len )
					reinterpret_cast<std::string*>( field->ptr )->clear();
				else
				{
					reinterpret_cast<std::string*>( field->ptr )->assign( &buffer[index], len );
					index += len;
				}
			}
			else
			{
				size_t len( field->len );
				char *c( (char*) field->ptr );

				while ( len-- )
					*c++ = buffer[index++];
			}
		}
	}
	else if ( BrowserFormat == format )
	{
		if ( !bufferLen )
			bufferLen = strlen( buffer );

		HobString s( buffer, bufferLen );

		HobTokenizerCSV tokenizer( &s, BrowserFormatSeparator );
		HobString token;

		size_t i( 0 );

		while (( 0 == tokenizer.next( &token ) ) && ( i < fields_.size() ))
		{
			field *f( &fields_[i++] );

			// handle field by type
			if ( ftBoolean == f->type )
			{
				*reinterpret_cast<bool*>( f->ptr ) = Bool::valueOf( token.c_str() );
			}
			else if ( ftInteger == f->type )
			{
				if ( sizeof(char) == f->len )
					*reinterpret_cast<char*>( f->ptr ) = (char) Long::valueOf( token.c_str() );
				else if ( sizeof(short) == f->len )
					*reinterpret_cast<short*>( f->ptr ) = (short) Long::valueOf( token.c_str() );
				else if ( sizeof(long) == f->len )
					*reinterpret_cast<long*>( f->ptr ) = (long) Long::valueOf( token.c_str() );
			}
			else if ( ftUnsignedInteger == f->type )
			{
				if ( sizeof(unsigned char) == f->len )
					*reinterpret_cast<unsigned char*>( f->ptr ) = (unsigned char) ULong::valueOf( token.c_str() );
				else if ( sizeof(unsigned short) == f->len )
					*reinterpret_cast<unsigned short*>( f->ptr ) = (unsigned short) ULong::valueOf( token.c_str() );
				else if ( sizeof(unsigned long) == f->len )
					*reinterpret_cast<unsigned long*>( f->ptr ) = (unsigned long) ULong::valueOf( token.c_str() );
			}
			else if ( ftReal == f->type )
			{
				if ( sizeof(float) == f->len )
					*reinterpret_cast<float*>( f->ptr ) = (float) Double::valueOf( token.c_str() );
				else if ( sizeof(double) == f->len )
					*reinterpret_cast<double*>( f->ptr ) = (double) Double::valueOf( token.c_str() );
			}
			else if ( ftString == f->type )
			{
				token.unQuoteForCSV();

				*reinterpret_cast<std::string*>( f->ptr ) = token;
			}
		}

		index = bufferLen;
	}

	return (unsigned int) index;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const char *DiagnosticsMessage::serialize() const
{
	return serialize( InternalFormat );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const char *DiagnosticsMessage::serialize( int format ) const
{
	if ( InternalFormat == format )
	{
		_Mybase::serialize();

		for ( fieldVector::const_iterator field( fields_.begin() ); field != fields_.end(); ++field )
		{
			if ( ftString == field->type )
			{
				const size_t len( reinterpret_cast<std::string*>( field->ptr )->length() );

				serializedData.append( (const char*) &len, sizeof(len) );

				if ( len )
					serializedData.append( *reinterpret_cast<std::string*>( field->ptr ) );
			}
			else
			{
				serializedData.append( (const char*) field->ptr, field->len );
			}
		}
	}
	else if ( BrowserFormat == format )
	{
		serializedData.clear();
		serializedData.append( util_string::longtostring( getDeviceType() ) );
		serializedData.append( BrowserFormatSeparator );
		serializedData.append( util_string::longtostring( getEventType() ) );

		for ( fieldVector::const_iterator f( fields_.begin() ); f != fields_.end(); ++f )
		{
			serializedData.append( BrowserFormatSeparator );
			serializedData.append( toString( (*f), true ) );
		}
	}

	return serializedData.data();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsMessage::show()
{
	TRACE( "DiagnosticsMessage values:" );
	TRACE( "  " + util_string::longtostring( getDeviceType() ) );
	TRACE( "  " + util_string::longtostring( getEventType() ) );

	for ( fieldVector::const_iterator f( fields_.begin() ); f != fields_.end(); ++f )
		TRACE( "  " + toString( *f ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string DiagnosticsMessage::toString( const field &f, bool quoteForCSV )
{
	static const std::string false_value( "0" );
	static const std::string true_value( "1" );

	// handle field by type
	if ( ftBoolean == f.type )
		return ( *reinterpret_cast<bool*>( f.ptr ) ? true_value : false_value );
	else if ( ftInteger == f.type )
	{
		if ( sizeof(char) == f.len )
			return util_string::longtostring( *reinterpret_cast<char*>( f.ptr ) );
		else if ( sizeof(short) == f.len )
			return util_string::longtostring( *reinterpret_cast<short*>( f.ptr ) );
		else if ( sizeof(long) == f.len )
			return util_string::longtostring( *reinterpret_cast<long*>( f.ptr ) );
	}
	else if ( ftUnsignedInteger == f.type )
	{
		if ( sizeof(unsigned char) == f.len )
			return util_string::ulongtostring( *reinterpret_cast<unsigned char*>( f.ptr ) );
		else if ( sizeof(unsigned short) == f.len )
			return util_string::ulongtostring( *reinterpret_cast<unsigned short*>( f.ptr ) );
		else if ( sizeof(unsigned long) == f.len )
			return util_string::ulongtostring( *reinterpret_cast<unsigned long*>( f.ptr ) );
	}
	else if ( ftReal == f.type )
	{
		if ( sizeof(float) == f.len )
			return util_string::doubletostring( *reinterpret_cast<float*>( f.ptr ), 1, 3 );
		else if ( sizeof(double) == f.len )
			return util_string::doubletostring( *reinterpret_cast<double*>( f.ptr ), 1, 3 );
	}
	else if ( ftString == f.type )
	{
		HobString s( *reinterpret_cast<std::string*>( f.ptr ) );

		if ( quoteForCSV )
			s.quoteForCSV();

		return s;
	}

	TRACE( "unhandled field type " + util_string::longtostring( f.type ) + " of length " + util_string::longlongtostring( f.len ) );
	return std::string();
}
