/**
 * @file cansocket.h
 * CAN Socket.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CANSOCKET_H
#define CANSOCKET_H

#include <linux/can.h>

#include <QThread>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// CAN Socket.
class canSocket : public QThread
{
    Q_OBJECT

    typedef canSocket _Myt;
    typedef QThread _Mybase;

Q_SIGNALS:

    /// Receive CAN frame.
    /**
     * @param[in] frame  CAN frame
     */
    void recvFrame( const can_frame *frame );

    //void doNothing();

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] iface  CAN interface
     * @param[in] parent  parent object
     */
    canSocket( const QString &iface, QObject *parent = 0 );

    /// Destructor.
    virtual ~canSocket();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if socket is open.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool isOpen() const {return (0 <= fd_);}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Send CAN frame.
    /**
     * @param[in] frame  CAN frame
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool sendFrame( const can_frame &frame );

    /// Send CAN frames.
    /**
     * @param[in] frames  CAN frames
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool sendFrames( const QList<can_frame> &frames );

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Thread run method.
    void run();

private:

    sockaddr_can addr_;
    int fd_;
    bool terminate_;

    // not implemented
    canSocket( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CANSOCKET_H
