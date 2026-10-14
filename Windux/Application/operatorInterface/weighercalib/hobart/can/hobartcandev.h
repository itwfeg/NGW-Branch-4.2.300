/**
 * @file hobartcandev.h
 * Hobart Generic CAN Device.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOBARTCANDEV_H
#define HOBARTCANDEV_H

#include "candev.h"

#include <QByteArray>
#include <QMap>
#include <QMutex>
#include <QPair>

///////////////////////////////////////////////////////////////////////////////////////////////////

// Hobart Generic CAN Device.
class hobartCanDev : public canDev
{
    Q_OBJECT

    typedef hobartCanDev _Myt;
    typedef canDev _Mybase;

public:

    typedef QList<unsigned int> messageSizeList;

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~hobartCanDev();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Add message.
    /**
     * @param[in] range  range
     * @param[in] sizes  size of each frame
     */
    virtual void addMessage( const rangeType &range, const messageSizeList &sizes );

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] s  can socket
     */
    hobartCanDev( canSocket *s );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Receive data from device.
    /**
     * @param[in] id  can frame id
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     */
    virtual void recvData( unsigned int id, const unsigned char *buffer, size_t bufferLen ) = 0;

    /// Send data to device.
    /**
     * @param[in] id  can frame id
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool sendData( unsigned int id, const unsigned char *buffer, size_t bufferLen );

private Q_SLOTS:

    /// Slot for socket ready read.
    void onReadyRead();

private:

    typedef QPair<rangeType, unsigned int> messageInfo;
    typedef QMap<unsigned int, messageInfo> messageInfoMap;

    typedef QMap<unsigned int, QByteArray> messageMap;

    messageInfoMap messageInfo_;
    messageMap messages_;

    // not implemented
    hobartCanDev();

    // not implemented
    hobartCanDev( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOBARTCANDEV_H
