/**
 * @file gcdmsg.h
 * GCD message base class and message classes.
 *
 * GCD messages control the graphical customer display.  They are in a length prefixed raw binary
 * format.
 *
 * +----------------+---------------------------+------------------------------+
 * | type (4 bytes) | payload length ( 4 bytes) | payload... (variable length) |
 * +----------------+---------------------------+------------------------------+
 *
 * Type definitions of all GCD messages may be found at the end of this file.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup communicationsCsu
 * @{
 */

#ifndef GCDMSG_H
#define GCDMSG_H

#include "msg.h"
#include "devicemsggcd.h"

#include <stdint.h>

#include <QList>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// GCD Message Types.
/**
 * Enumeration class of GCD messages.  Currently the scale app uses the GCD events as a type.
 */
class gcdMsgType
{
public:

    /// Message Types.
    enum type
    {
        // request messages
        Data                                = gcdDeviceMsgEventType::Data,
        LoadDisplayObjectXML                = gcdDeviceMsgEventType::LoadDisplayObjectXML,
        TestColor                           = gcdDeviceMsgEventType::TestColor,
        ShutdownDevice                      = gcdDeviceMsgEventType::ShutdownDevice,
        LanguageAndWeighMode                = gcdDeviceMsgEventType::LanguageAndWeighMode,
        SetVolume                           = gcdDeviceMsgEventType::SetVolume,
        Heartbeat                           = gcdDeviceMsgEventType::Heartbeat,
        GetStatus                           = gcdDeviceMsgEventType::GetStatus,
        GetFileInfo                         = gcdDeviceMsgEventType::GetFileInfo,
        FileUpload                          = gcdDeviceMsgEventType::FileUpload,
        LoadPlaylist                        = gcdDeviceMsgEventType::LoadPlaylist,
        StartFirmwareUpdate                 = gcdDeviceMsgEventType::StartFirmwareUpdate,
        StartPlaylistUpdate                 = gcdDeviceMsgEventType::StartPlaylistUpdate,
        SetPlaybackWaitTime                 = gcdDeviceMsgEventType::SetPlaybackWaitTime,
        SetTimeInfo                         = gcdDeviceMsgEventType::SetTimeInfo,
        TransferStart                       = gcdDeviceMsgEventType::TransferStart,
        TransferDone                        = gcdDeviceMsgEventType::TransferDone,
        TransferError                       = gcdDeviceMsgEventType::TransferError,
        SetUnitSeparator                    = gcdDeviceMsgEventType::SetUnitSeparator,
        SetCurrencySymbol                   = gcdDeviceMsgEventType::SetCurrencySymbol,
        NowServingCustomerNumber            = gcdDeviceMsgEventType::NowServingCustomerNumber,
        DismissNowServingCustomerNumber     = gcdDeviceMsgEventType::DismissNowServingCustomerNumber,

        // response messages
        Error                               = gcdDeviceMsgEventType::Error,
        HeartbeatResponse                   = gcdDeviceMsgEventType::HeartbeatResponse,
        LoadDisplayObjectXMLResponse        = gcdDeviceMsgEventType::LoadDisplayObjectXMLResponse,
        LoadPlaylistResponse                = gcdDeviceMsgEventType::LoadPlaylistResponse,
        Status                              = gcdDeviceMsgEventType::Status,
        FileInfo                            = gcdDeviceMsgEventType::FileInfo,
        RequestFileUpload                   = gcdDeviceMsgEventType::RequestFileUpload,
        FirmwareUpdateStaged                = gcdDeviceMsgEventType::FirmwareUpdateStaged,
        FirmwareUpdateCompleted             = gcdDeviceMsgEventType::FirmwareUpdateCompleted,
        PlaylistUpdateStaged                = gcdDeviceMsgEventType::PlaylistUpdateStaged,
        PlaylistUpdateCompleted             = gcdDeviceMsgEventType::PlaylistUpdateCompleted
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// GCD Message base class.
class gcdMsg : public QList<QString>
{
    friend class gcdMsgFactory;

    typedef gcdMsg _Myt;
    typedef QList<QString> _Mybase;

public:

    /// Destructor.
    virtual ~gcdMsg() {}

    /// Message type.
    /**
     * @return  type of this message
     */
    virtual gcdMsgType::type type() const = 0;

    /// Assignment operator.
    /**
     * @param[in] rhs  value to assign from
     * @return  reference to @c this
     */
    _Myt& operator = ( const _Myt& rhs ) {copy( rhs ); return *this;}

    /// Validate message.
    /**
     * Determine if data within @p data is a @c gcdMsg or not.  If valid @p type will be set.
     *
     * @param[in] data  data to check
     * @param[out] type  type of message if valid
     * @return  @c true if data is valid, @c false otherwise
     */
    static bool valid( const QByteArray& data, gcdMsgType::type& type );

protected:

    enum {DELIMITER = 0x19};

    enum {MSG_TYPE_OFFSET = 0};
    enum {MSG_LENGTH_OFFSET = 4};

    enum {MSG_HEADER_LENGTH = 8};

    /// Constructor.
    gcdMsg() : _Mybase() {}

    /// Constructor.
    /**
     * @param[in] rhs  message to copy
     */
    gcdMsg( const _Myt& rhs ) : _Mybase( rhs ) {}

    /// Parse message length from raw data.
    /**
     * @param[in] data  data to parse length from
     * @return  length of full message (i.e. header and payload)
     */
    static unsigned int parseLength( const QByteArray& data );

    /// Parse message type from raw data.
    /**
     * @param[in] data  data to parse type from
     * @return  gcd message type
     */
    static gcdMsgType::type parseType( const QByteArray& data );

    /// Read binary data from byte array.
    /**
     * This function will read a binary type from the array.
     *
     * For example, to read an @c int type from offset 12:
     * @code
     *   int value = gcdMsg::readArray<int>( array, 12 );
     * @endcode
     *
     * @param[in] a  array to read from
     * @param[in] offset  offset within arrary to read at
     * @return  value from array
     *
     * @tparam T  type to read
     */
    template <typename T>
    static T readArray( const QByteArray& a, int offset )
    {
        T result = 0;

        for ( int i = sizeof(T) - 1; i >= 0; --i )
        {
            result <<= 8;
            result |= a[offset+i];
        }

        return result;
    }

    /// Write binary data to byte array.
    /**
     * This function will write a binary type to the array.
     *
     * For example, to write an @c int type to offset 12:
     * @code
     *   int value = 42;
     *   gcdMsg::writeArray( array, 12, value );
     * @endcode
     *
     * @param[in,out] a  array to write to
     * @param[in] offset  offset within arrary to write to
     * @param[in] value  value to write into array
     *
     * @tparam T  type to write
     */
    template <typename T>
    static void writeArray( QByteArray& a, int offset, T value )
    {
        for ( int i = 0; i < (int) sizeof(T); ++i )
        {
            a[offset+i] = (value & 0xFF);
            value >>= 8;
        }
    }

private:

    /// Copy message.
    void copy( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// GCD Message Template.
/**
 * This is helper class that will implement base functions based on passed in template parameters.
 *
 * For example, the gcd message type @c gcdMsgType::Data would look like:
 * @code
 *   typedef gcdMsgT<gcdMsgType::Data> myDataMsg;
 * @endcode
 *
 * @tparam _Type  gcd message type
 */
template <gcdMsgType::type _Type>
class gcdMsgT : public gcdMsg
{
    typedef gcdMsgT<_Type> _Myt;

    typedef gcdMsg _Mybase;

public:

    enum {TYPE = _Type};

    /// Destructor.
    virtual ~gcdMsgT() {}

    /// Message type.
    /**
     * @return  message type
     */
    virtual gcdMsgType::type type() const {return (gcdMsgType::type) TYPE;}

    /// Check message type.
    /**
     * Evaluates data within @p data and determines if its this message or not.
     *
     * @param[in] data  data to evaluate
     * @return  @c true if data is this message type, @c false otherwise
     */
    static bool typeOf( const QByteArray& data )
    {
        gcdMsgType::type type;

        if ( valid( data, type ) )
            return typeOf( type );

        return false;
    }

    /// Check message type.
    /**
     * Checks gcd message type passed in and determines if its this message or not.
     *
     * @param[in] type  a gcd message type
     * @return  @c true if @p type matches this messages type, @c false otherwise
     */
    static bool typeOf( gcdMsgType::type type )
    {
        return ( (gcdMsgType::type) TYPE == type );
    }

protected:

    /// Constructor.
    gcdMsgT() : _Mybase() {}

    /// Constructor.
    /**
     * @param[in] rhs  message to copy
     */
    gcdMsgT( const _Myt& rhs ) : _Mybase( rhs ) {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// GCD Message Request.
/**
 * This class represents a received message and implements parsing functionality for a gcd message type.
 *
 * @tparam _Type  gcd message type
 */
template <gcdMsgType::type _Type>
class gcdMsgRequest : public msgRequest, virtual public gcdMsgT<_Type>
{
    typedef gcdMsgRequest<_Type> _Myt;

    typedef msgRequest _Mybase;
    typedef gcdMsgT<_Type> _Myimpl;

public:

    /// Constructor.
    gcdMsgRequest() : _Myimpl(), _Mybase() {}

    /// Constructor.
    /**
     * @param[in] rhs  message to copy
     */
    gcdMsgRequest( const _Myt& rhs ) : _Myimpl( rhs ), _Mybase( rhs ) {}

    /// Destructor.
    virtual ~gcdMsgRequest() {}

    /// Parse message.
    /**
     * Attempts to parse raw data within @p data.  If data contains a valid message, @p dataLen
     * will be set to the message length and @c true will be returned.
     *
     * @param[in] data  raw data from socket
     * @param[out] dataLen  length of message
     * @return  @c true if data is a valid message, @c false otherwise
     */
    virtual bool parse( const QByteArray& data, unsigned int& dataLen )
    {
        // Note:
        // The mingw32 or qmake compiler seems very picky compiling this function... for some
        // reason it cannot figure out that "gcdMsg" is a base class.  Due to this I have
        // added the "_Myimpl::" code below.

        if ( _Myimpl::typeOf( data ) )
        {
            dataLen = _Myimpl::parseLength( data );

            // get payload
            QByteArray payload = data.mid( _Myimpl::MSG_HEADER_LENGTH, dataLen - _Myimpl::MSG_HEADER_LENGTH );

            // split message payload into fields
            QList<QByteArray> tokens = payload.split( _Myimpl::DELIMITER );

            _Myimpl::clear();

            // add fields to list
            foreach ( const QByteArray &token, tokens )
                _Myimpl::push_back( token );

            return true;
        }

        return false;
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// GCD Message Response.
/**
 * This class represents a sendable message and implements a serialization routine.
 *
 * @tparam _Type  gcd message type
 */
template <gcdMsgType::type _Type>
class gcdMsgResponse : public msgResponse, virtual public gcdMsgT<_Type>
{
    typedef gcdMsgResponse<_Type> _Myt;

    typedef msgResponse _Mybase;
    typedef gcdMsgT<_Type> _Myimpl;

public:

    /// Constructor.
    gcdMsgResponse() : _Myimpl() , _Mybase() {}

    /// Constructor.
    /**
     * @param[in] rhs  message to copy
     */
    gcdMsgResponse( const _Myt& rhs ) : _Myimpl( rhs ), _Mybase( rhs ) {}

    /// Destructor.
    virtual ~gcdMsgResponse() {}

    /// Serialize message into raw data suitable for sending.
    /**
     * @return  raw data that is sendable over a socket (or otherwise).
     */
    virtual QByteArray serialize() const
    {
        // Note:
        // The mingw32 or qmake compiler seems very picky compiling this function... for some
        // reason it cannot figure out that "gcdMsg" is a base class.  Due to this I have
        // added the "_Myimpl::" code below.

        QByteArray data;

        data.resize( _Myimpl::MSG_HEADER_LENGTH );

        // another mingw32 workaround... it cannot resolve this nested within the for loop...
        typedef typename _Myimpl::const_iterator const_iterator_t;

        // format fields into payload
        for ( const_iterator_t i = _Myimpl::begin(); i !=  _Myimpl::end(); ++i )
        {
            data.append( *i );
            data.append( _Myimpl::DELIMITER );
        }

        // update header
        _Myimpl::writeArray( data, _Myimpl::MSG_LENGTH_OFFSET, (uint32_t) data.size() - _Myimpl::MSG_HEADER_LENGTH );
        _Myimpl::writeArray( data, _Myimpl::MSG_TYPE_OFFSET, (uint32_t) _Myimpl::TYPE );

        return data;
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// GCD Message BiDirectional.
/**
 * This class represents a message that is both sendable and receivable.
 *
 * @tparam _Type  gcd message type
 */
template <gcdMsgType::type _Type>
class gcdMsgBiDirectional : public gcdMsgRequest<_Type>, public gcdMsgResponse<_Type>
{
    typedef gcdMsgBiDirectional<_Type> _Myt;

    typedef gcdMsgRequest<_Type> _RequestType;
    typedef gcdMsgResponse<_Type> _ResponseType;

    typedef gcdMsgT<_Type> _Myimpl;

public:

    /// Constructor.
    gcdMsgBiDirectional() : _Myimpl(), _RequestType(), _ResponseType() {}

    /// Constructor.
    /**
     * @param[in] rhs  message to copy
     */
    gcdMsgBiDirectional( const _Myt& rhs ) : _Myimpl( rhs ) , _RequestType( rhs ), _ResponseType( rhs ) {}

    /// Destructor.
    virtual ~gcdMsgBiDirectional() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef gcdMsgRequest<gcdMsgType::Data> gcdDataMsg;
typedef gcdMsgRequest<gcdMsgType::LoadDisplayObjectXML> gcdLoadDisplayObjectXMLMsg;
typedef gcdMsgRequest<gcdMsgType::TestColor> gcdTestColorMsg;
typedef gcdMsgRequest<gcdMsgType::ShutdownDevice> gcdShutdownDeviceMsg;
typedef gcdMsgRequest<gcdMsgType::LanguageAndWeighMode> gcdLanguageAndWeighModeMsg;
typedef gcdMsgRequest<gcdMsgType::SetVolume> gcdSetVolumeMsg;
typedef gcdMsgRequest<gcdMsgType::Heartbeat> gcdHeartbeatMsg;
typedef gcdMsgRequest<gcdMsgType::GetStatus> gcdGetStatusMsg;
typedef gcdMsgRequest<gcdMsgType::GetFileInfo> gcdGetFileInfoMsg;
typedef gcdMsgRequest<gcdMsgType::FileUpload> gcdFileUploadMsg;
typedef gcdMsgRequest<gcdMsgType::LoadPlaylist> gcdLoadPlaylistMsg;
typedef gcdMsgRequest<gcdMsgType::StartFirmwareUpdate> gcdStartFirmwareUpdateMsg;
typedef gcdMsgRequest<gcdMsgType::StartPlaylistUpdate> gcdStartPlaylistUpdateMsg;
typedef gcdMsgRequest<gcdMsgType::SetPlaybackWaitTime> gcdSetPlaybackWaitTimeMsg;
typedef gcdMsgRequest<gcdMsgType::SetTimeInfo> gcdSetTimeInfoMsg;
typedef gcdMsgRequest<gcdMsgType::SetUnitSeparator> gcdSetUnitSeparatorMsg;
typedef gcdMsgRequest<gcdMsgType::SetCurrencySymbol> gcdSetCurrencySymbolMsg;
typedef gcdMsgRequest<gcdMsgType::NowServingCustomerNumber> gcdNowServingCustomerNumberMsg;
typedef gcdMsgRequest<gcdMsgType::DismissNowServingCustomerNumber> gcdDismissNowServingCustomerNumberMsg;

typedef gcdMsgResponse<gcdMsgType::Error> gcdErrorMsg;
typedef gcdMsgResponse<gcdMsgType::HeartbeatResponse> gcdHeartbeatResponseMsg;
typedef gcdMsgResponse<gcdMsgType::LoadDisplayObjectXMLResponse> gcdLoadDisplayObjectXMLResponseMsg;
typedef gcdMsgResponse<gcdMsgType::LoadPlaylistResponse> gcdLoadPlaylistResponseMsg;
typedef gcdMsgResponse<gcdMsgType::Status> gcdStatusMsg;
typedef gcdMsgResponse<gcdMsgType::FileInfo> gcdFileInfoMsg;
typedef gcdMsgResponse<gcdMsgType::RequestFileUpload> gcdRequestFileUploadMsg;
typedef gcdMsgResponse<gcdMsgType::FirmwareUpdateStaged> gcdFirmwareUpdateStagedMsg;
typedef gcdMsgResponse<gcdMsgType::FirmwareUpdateCompleted> gcdFirmwareUpdateCompletedMsg;
typedef gcdMsgResponse<gcdMsgType::PlaylistUpdateStaged> gcdPlaylistUpdateStagedMsg;
typedef gcdMsgResponse<gcdMsgType::PlaylistUpdateCompleted> gcdPlaylistUpdateCompletedMsg;

/** @} */

#endif // GCDMSG_H
