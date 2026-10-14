/**
 * @file EdgePacket.h
 * @brief Edge device packet for gateway communication.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef EDGECOMMEDGEPACKET_H
#define EDGECOMMEDGEPACKET_H

#include "Header.h"

/// Edge Comms namespace.
namespace edgecomm
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Edge device packet for gateway communication.
struct EdgePacket
{
	typedef EdgePacket _Myt;

public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	EdgePacket() {initialize();}

	/// Constructor.
	/**
	 * @param[in] other  object to copy
	 */
	EdgePacket( const _Myt &other ) {copy( other );}

	/// Destructor.
	virtual ~EdgePacket() {}

	// ========================================================================
	// Operators
	// ========================================================================

	/// Copy operator.
	/**
	 * @param[in] rhs  object to copy
	 * @return  reference to this
	 */
	_Myt & operator = ( const _Myt &rhs ) {copy( rhs ); return *this;}

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve packet size.
	/**
	 * @return  packet size (excluding transmit length prefix)
	 */
	virtual size_t size() const {return writeOffset_;}

	/// Retrieve transmit buffer.
	/**
	 * @return  buffer
	 */
	virtual const char *transmitBuffer() const {return buffer_;}

	/// Retrieve transmit size.
	/**
	 * @return  size of transmit buffer
	 */
	virtual size_t transmitSize() const {return (writeOffset_ + LENGTH_PREFIX_SIZE);}

	// ========================================================================
	// Methods
	// ========================================================================

	/// Append buffer to packet.
	/**
	 * @param[in] buffer  buffer to append
	 * @param[in] bufferLen  size of buffer
	 */
	virtual void append( const void *buffer, size_t bufferLen ) {update( writeOffset_, buffer, bufferLen );}

	/// Append message to packet.
	/**
	 * @param[in] msg  message to append
	 */
	template <typename MSG>
	void append( const MSG &msg )
	{
		offsetList fields;
		msg.offsets( fields );

		for ( offsetList::const_iterator i( fields.begin() ); i != fields.end(); ++i )
			append( i->first, i->second );
	}

	/// Update packet with buffer data.
	/**
	 * @param[in] offset  offset to update
	 * @param[in] buffer  buffer to append
	 * @param[in] bufferLen  size of buffer
	 */
	virtual void update( size_t offset, const void *buffer, size_t bufferLen );

	/// Update packet with buffer data.
	/**
	 * @param[in] offset  offset to update
	 * @param[in] msg  message to append
	 */
	template <typename MSG>
	void update( size_t offset, const MSG &msg )
	{
		offsetList fields;
		msg.offsets( fields );

		for ( offsetList::const_iterator i( fields.begin() ); i != fields.end(); ++i )
		{
			update( offset, i->first, i->second );
			offset += i->second;
		}
	}

	// ========================================================================
	// Static Methods
	// ========================================================================

	/// Calculate sizeof message.
	/**
	 * @param[in] msg  message to calculate size of
	 * @return  size
	 */
	template <typename MSG>
	static size_t sizeofMsg( const MSG &msg )
	{
		size_t s( 0 );

		offsetList fields;
		msg.offsets( fields );

		for ( offsetList::const_iterator i( fields.begin() ); i != fields.end(); ++i )
			s += i->second;

		return s;
	}

private:

	static const size_t LENGTH_PREFIX_SIZE = sizeof(uint16_t);
	static const size_t MAX_PACKET_SIZE = 65535 + LENGTH_PREFIX_SIZE;

	char buffer_[MAX_PACKET_SIZE];
	size_t writeOffset_;

	/// Initialize.
	void initialize();

	/// Copy object.
	void copy( const _Myt &other );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // EDGECOMMEDGEPACKET_H
