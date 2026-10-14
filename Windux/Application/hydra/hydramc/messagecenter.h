/**
 * @file messagecenter.h
 * @brief Class that instantiates message center on DBUS.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef MESSAGECENTER_H
#define MESSAGECENTER_H

#include <QMutex>
#include <QObject>

class QByteArray;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Message Center Object
class MessageCenter : public QObject
{
    Q_OBJECT

    using _Myt = MessageCenter;
    using _Mybase = QObject;

signals:

    /// Process event message signal.
    /**
     * @param[in] a  event data
     */
    void processEvent( const QByteArray& a );

public slots:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Post event message.
    /**
     * @param[in] a  event data
     */
    void post( const QByteArray& a );

public:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Retrieve global instance.
    /**
     * @return  pointer to instance
     */
    static _Myt *instance();

private:

    static _Myt *instance_;
    static QMutex instanceMutex_;

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    MessageCenter();

    // not implemented
    MessageCenter( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MESSAGECENTER_H
