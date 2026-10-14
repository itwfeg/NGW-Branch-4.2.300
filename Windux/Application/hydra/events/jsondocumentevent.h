/**
 * @file jsondocumentevent.h
 * @brief JSON Document Event.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef JSONDOCUMENTEVENT_H
#define JSONDOCUMENTEVENT_H

#include "event.h"

#include <QJsonDocument>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// JSON Document Event.
class EVENTS_EXPORT JsonDocumentEvent : public Event
{
    using _Myt = JsonDocumentEvent;
    using _Mybase = Event;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] doc  json document
     */
    JsonDocumentEvent( const QJsonDocument& doc = QJsonDocument() );

    /// Constructor.
    /**
     * @param[in] device  device
     * @param[in] event  event
     * @param[in] doc  json document
     */
    JsonDocumentEvent( DeviceType device, uint event, const QJsonDocument& doc = QJsonDocument() );

    /// Constructor.
    /**
     * @param[in] a  raw event buffer
     */
    JsonDocumentEvent( const QByteArray& a );

    /// Destructor.
    virtual ~JsonDocumentEvent();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve JSON document.
    /**
     * @return  document
     */
    virtual QJsonDocument document() const;

    /// Set JSON document.
    /**
     * @param[in]  document
     */
    virtual void setDocument( const QJsonDocument& value );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // JSONDOCUMENTEVENT_H
