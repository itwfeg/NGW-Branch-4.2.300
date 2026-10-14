/**
 * @file tageventadapter.h
 * @brief Event adapter for tag based comms.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef TAGEVENTADAPTER_H
#define TAGEVENTADAPTER_H

#include "eventadapter.h"

#include <QPair>

struct TagFieldMap;

class FileResourceEvent;
class JsonDocumentEvent;

class QJsonDocument;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Adapter for tag based comms.
class EVENTADAPTERS_EXPORT TagEventAdapter : public EventAdapter
{
    using _Myt = TagEventAdapter;
    using _Mybase = EventAdapter;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    TagEventAdapter();

    /// Destructor.
    virtual ~TagEventAdapter();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve if we should ignore/drop unknown field tags versus erroring out.
    /**
     * @return  @c true to ignore, @c false otherwise
     */
    virtual bool ignoreUnknownFieldTags() const;

    /// Retrieve if we should ignore/drop unknown record tags versus erroring out.
    /**
     * @return  @c true to ignore, @c false otherwise
     */
    virtual bool ignoreUnknownRecTags() const;

    /// Set if we should ignore/drop unknown field tags versus erroring out.
    /**
     * @param[in]  @c true to ignore, @c false otherwise
     */
    virtual void setIgnoreUnknownFieldTags( bool value );

    /// Set if we should ignore/drop unknown record tags versus erroring out.
    /**
     * @param[in]  @c true to ignore, @c false otherwise
     */
    virtual void setIgnoreUnknownRecTags( bool value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Destroy allocated events.
    /**
     * @param[in] doomed  list of events
     */
    virtual void destroyEvents( const QList<Event*>& doomed ) override;

    /// Translate comm data into events.
    /**
     * @param[in,out] buffer  comm data
     * @param[out] valid  translation result
     * @return  list of events
     */
    virtual QList<Event*> translateComm( QByteArray& buffer, bool *valid = nullptr ) override;

    /// Translate event into comm data.
    /**
     * @param[in] e  event
     * @param[out] valid  translation result
     * @return  comm data
     */
    virtual QByteArray translateEvent( const Event *e, bool *valid = nullptr ) override;

protected:

    bool ignoreUnknownRecTags_;                         ///< Ignore/drop unknown tags.
    bool ignoreUnknownFieldTags_;                       ///< Ignore/drop unknown tags.

    QString prevRecTag_;                                ///< Previous tag.

private:

    using TagDataPair = QPair<QString, QByteArray>;
    using TagDataPairs = QVector<TagDataPair>;

    /// Convert buffer to event.
    Event *convert( const QByteArray& buffer );

    /// Convert record to event.
    Event *convertRecord( const TagDataPairs& data );

    /// Convert record fields to buffer.
    bool convertRecordFields( const TagDataPairs& data, const TagFieldMap *fields, QJsonDocument& doc );

    /// Convert event to buffer.
    QByteArray convert( const JsonDocumentEvent *e, const char *tag = nullptr );

    /// Convert record fields to buffer.
    bool convertRecordFields( const QJsonDocument& doc, const TagFieldMap *fields, QByteArray& a );

    // not implemented
    TagEventAdapter( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TAGEVENTADAPTER_H
