/**
 * @file event.h
 * @brief Abstract Event object.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef EVENT_H
#define EVENT_H

#include "devicetype.h"
#include "events_global.h"

#include <QByteArray>
#include <QVector>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Abstract Event object.
class EVENTS_EXPORT Event
{
    using _Myt = Event;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] dataLen  data length
     */
    Event( const QVector<uint>& dataLen = QVector<uint>() );

    /// Constructor.
    /**
     * @param[in] device  device
     * @param[in] event  event
     * @param[in] dataLen  data length
     */
    Event( DeviceType device, uint event, const QVector<uint>& dataLen = QVector<uint>() );

    /// Constructor.
    /**
     * @param[in] a  raw event buffer
     */
    Event( const QByteArray& a );

    /// Constructor.
    /**
     * @param[in] other  other object
     */
    Event( const _Myt& other );

    /// Destructor.
    virtual ~Event();

    // ========================================================================
    // Operators
    // ========================================================================

    /// Equality operator.
    /**
     * @param[in] rhs  object to compare
     * @return  @c true if equal, @c false otherwise
     */
    bool operator == ( const _Myt& rhs ) const;

    /// Equality operator.
    /**
     * @param[in] rhs  object to compare
     * @return  @c true if NOT equal, @c false otherwise
     */
    bool operator != ( const _Myt& rhs ) const;

    /// Assignment operator.
    /**
     * @param[in] rhs  object to assign to
     * @return  reference to this
     */
    _Myt& operator = ( const _Myt& rhs );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if event is valid.
    /**
     * @return  @c true if value, @c false otherwise
     */
    virtual bool isValid() const;

    /// Retreive device number.
    /**
     * @return  device
     */
    virtual DeviceType device() const;

    /// Set device number.
    /**
     * @param[in]  device
     */
    virtual void setDevice( DeviceType value );

    /// Retreive event number.
    /**
     * @return  event
     */
    virtual uint event() const;

    /// Set event number.
    /**
     * @param[in]  event
     */
    virtual void setEvent( uint value );

    /// Retreive sender's service name.
    /**
     * @return  service name
     */
    virtual QString senderService() const;

    /// Set sender's service name.
    /**
     * @param[in]  service name
     */
    virtual void setSenderService( const QString& value );

    /// Retreive sender's object path.
    /**
     * @return  object path
     */
    virtual QString senderObjectPath() const;

    /// Set sender's object path.
    /**
     * @param[in]  object path
     */
    virtual void setSenderObjectPath( const QString& value );

    /// Retrieve event data.
    /**
     * @tparam T  data type
     * @param[in] offset  data offset
     * @return  data
     */
    template <typename T>
    T data( uint offset ) const;

    /// Set event data.
    /**
     * @tparam T  data type
     * @param[in] offset  data offset
     * @param[in] value  data
     */
    template <typename T>
    void setData( uint offset, const T& value );

    /// Set event data.
    /**
     * @param[in] offset  data offset
     * @param[in] value  null terminated string
     */
    void setData( uint offset, const char *value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Serialize event.
    /**
     * @return  buffer
     */
    virtual QByteArray serialize() const;

protected:

    DeviceType device_;                             ///< Device type.
    uint event_;                                    ///< Event number.

    QString senderService_;                         ///< Sender service name (DBUS service).
    QString senderObjectPath_;                      ///< Sender object path (DBUS object path).

    QVector<uint> dataLen_;                         ///< Event data lengths.
    QVector<QByteArray> data_;                      ///< Event data.

    // ========================================================================
    // Methods
    // ========================================================================

    /// Deserialize event.
    /**
     * @param[in] a  buffer to parse
     * @param[in,out] pos  position
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool deserialize( const QByteArray& a, uint& pos );

private:

    /// Write data to array.
    static void write( QByteArray& a, uint value );

    /// Write data to array.
    static void write( QByteArray& a, const QByteArray& value );

    /// Read data from array.
    static bool read( const QByteArray& a, uint& pos, uint& value );

    /// Read data from array.
    static bool read( const QByteArray& a, uint& pos, QByteArray& value, uint len = 0 );

};

template <>
inline QByteArray Event::data( uint offset ) const
{
    if ( offset < (uint) data_.size() )
        return data_[offset];

    return QByteArray();
}

template <>
inline QString Event::data( uint offset ) const
{
    if ( offset < (uint) data_.size() )
        return data_[offset];

    return QString();
}

template <typename T>
inline T Event::data( uint offset ) const
{
    T value;

    // cast data to requested type
    if ( offset < (uint) data_.size() )
    {
        if ( sizeof(T) <= (unsigned) data_[offset].size() )
            value = *((const T*)( data_[offset].constData() ));
    }

    return value;
}


template <>
inline void Event::setData( uint offset, const QByteArray& value  )
{
    if ( (uint) data_.size() <= offset )
        data_.resize( offset+1 );

    data_[offset] = value;
}

template <>
inline void Event::setData( uint offset, const QString& value  )
{
    setData( offset, value.toLatin1() );
}

template <typename T>
inline void Event::setData( uint offset, const T& value )
{
    setData( offset, QByteArray( (const char*) &value, sizeof(T) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Event Template.
/**
 * @tparam DEVICE  device
 * @tparam EVENT  event
 * @tparam DATA_LEN  vector of data length
 */
template <DeviceType DEVICE, uint EVENT, const QVector<uint>& DATA_LEN>
class EventT : public Event
{
    using _Myt = EventT<DEVICE, EVENT, DATA_LEN>;
    using _Mybase = Event;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    EventT() : _Mybase( DEVICE, EVENT, DATA_LEN ) {}

};


///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // EVENT_H
