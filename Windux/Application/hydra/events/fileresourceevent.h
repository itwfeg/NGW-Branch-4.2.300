/**
 * @file fileresourceevent.h
 * @brief File Resource Event.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef FILERESOURCEEVENT_H
#define FILERESOURCEEVENT_H

#include "event.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// File Resource Event.
class EVENTS_EXPORT FileResourceEvent : public Event
{
    using _Myt = FileResourceEvent;
    using _Mybase = Event;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] type  type metadata
     * @param[in] subtype  subtype metadata
     */
    FileResourceEvent( uint type, uint subtype );

    /// Constructor.
    /**
     * @param[in] type  type metadata
     * @param[in] subtype  subtype metadata
     * @param[in] filename  filename
     * @param[in] data  file data
     */
    FileResourceEvent( uint type, uint subtype, const QString& filename, const QByteArray& data = QByteArray() );

    /// Constructor.
    /**
     * @param[in] device  device
     * @param[in] event  event
     * @param[in] type  type metadata
     * @param[in] subtype  subtype metadata
     */
    FileResourceEvent( DeviceType device, uint event, uint type, uint subtype );

    /// Constructor.
    /**
     * @param[in] device  device
     * @param[in] event  event
     * @param[in] type  type metadata
     * @param[in] subtype  subtype metadata
     * @param[in] filename  filename
     * @param[in] data  file data
     */
    FileResourceEvent( DeviceType device, uint event, uint type, uint subtype, const QString& filename, const QByteArray& data = QByteArray() );

    /// Constructor.
    /**
     * @param[in] a  raw event buffer
     */
    FileResourceEvent( const QByteArray& a );

    /// Destructor.
    virtual ~FileResourceEvent();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve file type metadata.
    /**
     * @return  file type; or zero (0) if unknown
     */
    virtual uint type() const;

    /// Set file type metadata.
    /**
     * @param[in] value  file type; or zero (0) if unknown
     */
    virtual void setType( uint value );

    /// Retrieve file subtype metadata.
    /**
     * @return  file subtype; or zero (0) if unknown
     */
    virtual uint subtype() const;

    /// Set file subtype metadata.
    /**
     * @param[in] value  file subtype; or zero (0) if unknown
     */
    virtual void setSubtype( uint value );

    /// Retrieve filename.
    /**
     * @return  filename
     */
    virtual QString filename() const;

    /// Set filename.
    /**
     * @param[in] value  filename
     */
    virtual void setFilename( const QString& value );

    /// Retrieve file data.
    /**
     * @return  data
     */
    virtual QByteArray data() const;

    /// Set file data.
    /**
     * @return  data
     */
    virtual void setData( const QByteArray& value );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // FILERESOURCEEVENT_H
