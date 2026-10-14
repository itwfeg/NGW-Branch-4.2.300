/**
 * @file devicemsg.h
 * Device message base classes.
 *
 * Device messages have both a device and event.  This file defines all device types and also
 * base classes for device messages.  Each device type has its own set of events.  Those events
 * are defined in a seperate header for each device.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup communicationsCsu
 * @{
 */

#ifndef DEVICEMSG_H
#define DEVICEMSG_H

#include "msg.h"

#include <QList>
#include <QString>

#include <QTextStream>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Device Message Types.
/**
 * Enumeration class of devices.
 */
class deviceMsgType
{
public:

    /// Device Types.
    enum type
    {
        Unknown                             = 0,

        Weigher                             = 1,
        WeigherSystem,
        Event,
        OperationMode,
        LabelApplier,                                   // 5
        LabelApplierSystem,
        Printer,
        PrinterSystem,
        Wrapper,
        WrapperSystem,                                  // 10
        System,
        LSMNtoUSB,
        Renderer,
        TouchScreen,
        RFIDReader,                                     // 15

        Session                             = 100,
        Product,
        Operator,
        Error,
        TotalType,
        Graphic,                                        // 105
        ExpandedText,
        SpecialMessage,
        ProductNotes,
        BackupRestore,
        Class,                                          // 110
        LabelType,
        Configuration,
        Font,
        UltimaDisk,
        VirtualPeripheral,                              // 115
        ReportManager,
        Nutrifacts,
        Timer,
        GenericText,
        VersionManager,                                 // 120
        Category,
        Network,
        Totals,
        Specials,
        PresentationMethod,                             // 125
        ProductStatistics,
        CategoryStatistics,
        InitialProduct,
        StreamingGraphic,
        OperatorNotes,                                  // 130
        OperatorChanges,
        ProductDefaults,
        EmailManager,
        EmailAlerts,
        COOL,                                           // 135
        Softkey,
        MarqueeText,
        Peripheral,
        Numeric,
        Marquee,                                        // 140
        ESCPrinter,
        NumericSystem,
        MarqueeSystem,
        Import,
        NetworkConnectivity,                            // 145
        Synchronization,
        SynchronizationChange,
        Scale,
        Batch,
        CGICallManager,                                 // 150
        Transaction,
        TransactionType,
        HourlyTotal,
        OperatorTotal,
        ClassTotal,                                     // 155
        ProductTotal,
        COOLTotal,
        WindowsConfiguration,
        CutTest,
        CutTestTotal,                                   // 160
        GCDSession,
        GCD
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Device Message base class.
class deviceMsg : public QList<QString>
{
    friend class deviceMsgFactory;

    typedef deviceMsg _Myt;
    typedef QList<QString> _Mybase;

public:

    /// Destructor.
    virtual ~deviceMsg() {}

    /// Message device.
    /**
     * @return  device type of this message
     */
    virtual deviceMsgType::type device() const = 0;

    /// Message event.
    /**
     * @return  event of this message
     */
    virtual unsigned int event() const = 0;

    /// Retrieve field from message.
    /**
     * Fields can be of any type.  For example:
     * @code
     *   int number = msg.field<int>( 2 );
     *   QString text = msg.field<QString>( 3 );
     *   float decimal = msg.field<float>( 4 );
     * @endcode
     *
     * @param[in] index  index of field to retrieve
     * @return  field at index @p index
     *
     * @tparam T  field type
     */
    template <class T>
    T field( unsigned int index ) const
    {
        QString temp = at( index );

        T f;
        QTextStream( &temp ) >> f;

        return f;
    }

    /// Set field in message.
    /**
     * Fields can be of any type.  For example:
     * @code
     *   int number = 42;
     *   msg.setField( 2, number );
     *   QString text = "example";
     *   msg.setField( 3, text );
     *   msg.setField( 4, 123.45f );
     * @endcode
     *
     * @param[in] index  index of field to set
     * @param[in] f  value to set
     *
     * @tparam T  field type
     */
    template <class T>
    void setField( unsigned int index, const T& f )
    {
        QString field;
        QTextStream( &field ) << f;

        replace( index, field );
    }

    /// Add field to message.
    /**
     * This function will append a field to the end of the message.
     *
     * Fields can be of any type.  For example:
     * @code
     *   int number = 42;
     *   msg.appendField( number );
     *   QString text = "example";
     *   msg.appendField( text );
     *   msg.appendField( 123.45f );
     * @endcode
     *
     * @param[in] f  value to append
     *
     * @tparam T  field type
     */
    template <class T>
    void appendField( const T& f )
    {
        QString field;
        QTextStream( &field ) << f;

        push_back( field );
    }

    /// Assignment operator.
    /**
     * @param[in] rhs  value to assign from
     * @return  reference to @c this
     */
    _Myt& operator = ( const _Myt& rhs ) {copy( rhs ); return *this;}

    /// Validate message.
    /**
     * Determine if data within @p data is a @c deviceMsg or not.  If valid @p device and
     * @p event will be set.
     *
     * @param[in] data  data to check
     * @param[out] device  device type of message if valid
     * @param[out] event  event of message if valid
     * @return  @c true if data is valid, @c false otherwise
     */
    static bool valid( const QByteArray& data, deviceMsgType::type& device, unsigned int& event );

    /// Decorate a string field (i.e. for sending).
    /**
     * This function formats a string into something compatible for sending.  It will add quotes
     * and modify special characters.
     *
     * @param[in] s  string to decorate
     * @return  decorated string
     */
    static QString decorateStringField( const QString& s );

    /// Remove decorations from string (i.e. string receieved).
    /**
     * This function takes a string received and strips decoration.  It removes quotes and special
     * characters used for sending.
     *
     * @param[in] s  decorated string
     * @return  undecorated string
     */
    static QString undecorateStringField( const QString& s );

protected:

    enum {PREFIX_LEN = 6};

    enum {DELIMITER = ','};
    enum {TEXT_FIELD_INDICATOR = '\"'};

    /// Constructor.
    deviceMsg() : _Mybase() {}

    /// Constructor.
    /**
     * @param[in] rhs  message to copy
     */
    deviceMsg( const _Myt& rhs ) : _Mybase( rhs ) {}

    /// Calculate length of message.
    /**
     * This function will determine length of message based on prefix length.
     *
     * @param[in] data  raw data
     * @return  length of message in bytes
     */
    static unsigned int calculateLength( const QByteArray& data );

    /// Split data into fields.
    /**
     * @param[in] data  data to split
     * @return  list of fields split on delimiter @c DELIMITER
     */
    static QList<QByteArray> split( const QByteArray& data );

private:

    /// Copy message.
    void copy( const _Myt& rhs );

};

/// Retrieve string field from message.
/**
 * This specialization undecorates the string before giving to caller.
 *
 * @param[in] index  index of field to retrieve
 * @return  field at index @p index
 */
template <>
inline QString deviceMsg::field<QString>( unsigned int index ) const
{
    return undecorateStringField( at( index ) );
}

/// Sets a string field in message.
/**
 * This specialization decorates the string before setting.
 *
 * @param[in] index  index of field to set
 * @param[in] f  value to set
 */
template <>
inline void deviceMsg::setField<QString>( unsigned int index, const QString& f )
{
    replace( index, decorateStringField( f ) );
}

/// Add string field to message.
/**
 * This specialization decorates the string before appending.
 *
 * @param[in] f  value to append
 */
template <>
inline void deviceMsg::appendField<QString>( const QString& f )
{
    push_back( decorateStringField( f ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Device Message Template.
/**
 * This is helper class that will implement base functions based on passed in template parameters.
 *
 * @tparam _Device  device type
 * @tparam _Event  event number
 */
template <deviceMsgType::type _Device, unsigned int _Event>
class deviceMsgT : public deviceMsg
{
    typedef deviceMsgT<_Device, _Event> _Myt;

    typedef deviceMsg _Mybase;

public:

    enum {DEVICE = _Device};
    enum {EVENT = _Event};

    /// Destructor.
    virtual ~deviceMsgT() {}

    /// Message device.
    /**
     * @return  device type of message
     */
    virtual deviceMsgType::type device() const {return (deviceMsgType::type) DEVICE;}

    /// Message event.
    /**
     * @return  event number of message
     */
    virtual unsigned int event() const {return EVENT;}

    /// Check message type.
    /**
     * Evaluates data within @p data and determines if its this message or not.
     *
     * @param[in] data  data to evaluate
     * @return  @c true if data is this message type, @c false otherwise
     */
    static bool typeOf( const QByteArray& data )
    {
        deviceMsgType::type device;
        unsigned int event;

        // check device and event fields
        if ( valid( data, device, event ) )
            return typeOf( device, event );

        return false;
    }

    /// Check message type.
    /**
     * Checks device and event passed in and determines if its this message or not.
     *
     * @param[in] device  a device type
     * @param[in] event  an event number
     * @return  @c true if @p device and @p event match this messages device and event, @c false otherwise
     */
    static bool typeOf( deviceMsgType::type device, unsigned int event )
    {
        return (( (deviceMsgType::type) DEVICE == device ) && ( EVENT == event ));
    }

protected:

    /// Constructor.
    deviceMsgT() : _Mybase() {}

    /// Constructor.
    /**
     * @param[in] rhs  message to copy
     */
    deviceMsgT( const _Myt& rhs ) : _Mybase( rhs ) {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Device Message Request.
/**
 * This class represents a received message and implements parsing functionality for a device and event.
 *
 * @tparam _Device  device type
 * @tparam _Event  event number
 */
template <deviceMsgType::type _Device, unsigned int _Event>
class deviceMsgRequest : public msgRequest, virtual public deviceMsgT<_Device, _Event>
{
    typedef deviceMsgRequest<_Device, _Event> _Myt;

    typedef msgRequest _Mybase;
    typedef deviceMsgT<_Device, _Event> _Myimpl;

public:

    /// Constructor.
    deviceMsgRequest() : _Myimpl(), _Mybase() {}

    /// Constructor.
    /**
     * @param[in] rhs  message to copy
     */
    deviceMsgRequest( const _Myt& rhs ) : _Myimpl( rhs ), _Mybase( rhs ) {}

    /// Destructor.
    virtual ~deviceMsgRequest() {}

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
        // reason it cannot figure out that "deviceMsg" is a base class.  Due to this I have
        // added the "_Myimpl::" code below.

        if ( _Myimpl::typeOf( data ) )
        {
            dataLen = _Myimpl::calculateLength( data );

            // make sure all our message is there
            if ( dataLen )
            {
                // split message into fields
                QList<QByteArray> tokens = _Myimpl::split( data.mid( _Myimpl::PREFIX_LEN, dataLen - _Myimpl::PREFIX_LEN ) );

                // clear out existing data
                _Myimpl::clear();

                // add fields to list (ignoring the first two)
                foreach ( const QByteArray &token, tokens )
                    _Myimpl::push_back( token );

                return true;
            }
        }

        return false;
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Device Message Response.
/**
 * This class represents a sendable message and implements a serialization routine.
 *
 * @tparam _Device  device type
 * @tparam _Event  event number
 */
template <deviceMsgType::type _Device, unsigned int _Event>
class deviceMsgResponse : public msgResponse, virtual public deviceMsgT<_Device, _Event>
{
    typedef deviceMsgResponse<_Device, _Event> _Myt;

    typedef msgResponse _Mybase;
    typedef deviceMsgT<_Device, _Event> _Myimpl;

public:

    /// Constructor.
    deviceMsgResponse() :  _Myimpl(), _Mybase() {initialize();}

    /// Constructor.
    /**
     * @param[in] rhs  message to copy
     */
    deviceMsgResponse( const _Myt& rhs ) : _Myimpl( rhs ), _Mybase( rhs ) {}

    /// Destructor.
    virtual ~deviceMsgResponse() {}

    /// Serialize message into raw data suitable for sending.
    /**
     * @return  raw data that is sendable over a socket (or otherwise).
     */
    virtual QByteArray serialize() const
    {
        int fields = 0;

        // Note:
        // The mingw32 or qmake compiler seems very picky compiling this function... for some
        // reason it cannot figure out that "deviceMsg" is a base class.  Due to this I have
        // added the "_Myimpl::" code below.

        QByteArray prefix;
        QByteArray data;

        // another mingw32 workaround... it cannot resolve this nested within the for loop...
        typedef typename _Myimpl::const_iterator iterator_t;

        // add fields onto message
        for ( iterator_t i = _Myimpl::begin(); i != _Myimpl::end(); i++ )
        {
            if ( fields++ )
                data.append( _Myimpl::DELIMITER );

            data.append( *i );
        }

        // create prefix length
        prefix = QByteArray::number( _Myimpl::PREFIX_LEN + data.length() );

        while ( prefix.length() < _Myimpl::PREFIX_LEN )
            prefix.prepend( '0' );

        // return result
        return prefix + data;
    }

private:

    /// Initialize message with device and event.
    void initialize()
    {
        _Myimpl::appendField( (unsigned int) _Myimpl::DEVICE );
        _Myimpl::appendField( (unsigned int) _Myimpl::EVENT );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Device Message BiDirectional.
/**
 * This class represents a message that is both sendable and receivable.
 *
 * @tparam _Device  device type
 * @tparam _Event  event number
 */
template <deviceMsgType::type _Device, unsigned int _Event>
class deviceMsgBiDirectional :
    public deviceMsgRequest<_Device, _Event>,
    public deviceMsgResponse<_Device, _Event>
{
    typedef deviceMsgBiDirectional<_Device, _Event> _Myt;

    typedef deviceMsgRequest<_Device, _Event> _RequestType;
    typedef deviceMsgResponse<_Device, _Event> _ResponseType;

    typedef deviceMsgT<_Device, _Event> _Myimpl;

public:

    /// Constructor.
    deviceMsgBiDirectional() : _Myimpl(), _RequestType(), _ResponseType() {}

    /// Constructor.
    /**
     * @param[in] rhs  message to copy
     */
    deviceMsgBiDirectional( const _Myt& rhs ) : _Myimpl( rhs ) , _RequestType( rhs ), _ResponseType( rhs ) {}

    /// Destructor.
    virtual ~deviceMsgBiDirectional() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // DEVICEMSG_H
