/**
 * @file candev.h
 * CAN Device Filter.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CANDEV_H
#define CANDEV_H

#include <QMutex>
#include <QObject>
#include <QPair>

struct can_frame;

class canSocket;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// CAN Device Filter.
class canDev : public QObject
{
    Q_OBJECT

    typedef canDev _Myt;
    typedef QObject _Mybase;

Q_SIGNALS:

    /// Signal for data processing after read.
    void readyRead();

public:

    /// CAN Frame Id Range (min, numFrames)
    typedef QPair<unsigned int, unsigned int> rangeType;

    /// List of CAN frame id ranges.
    typedef QList<rangeType> rangeTypeList;

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~canDev();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Add device range.
    /**
     * Filter messages based on can id.
     *
     * @param[in] value  range
     */
    virtual void addDeviceRange( const rangeType &value );

    /// Add device ranges.
    /**
     * Filter messages based on can id.
     *
     * @param[in] value  ranges
     */
    virtual void addDeviceRange( const rangeTypeList &value );

    /// Retrieve can id range.
    /**
     * @return  range
     */
    virtual rangeTypeList deviceRanges() const;

    /// Retrieve frames available for reading.
    /**
     * @return  frames available
     */
    virtual size_t framesAvailable() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Peek frame.
    /**
     * @return  copy of frame pending
     */
    virtual can_frame peek() const;

    /// Peek frames.
    /**
     * @param[in] maxSize  maximum frames to peek
     * @return  copy of frames pending
     */
    virtual QList<can_frame> peek( size_t maxSize ) const;

    /// Read frame.
    /**
     * @return  frame pending
     */
    virtual can_frame read();

    /// Read frames.
    /**
     * @param[in] maxSize  maximum frames to read
     * @return  frames pending
     */
    virtual QList<can_frame> read( size_t maxSize );

    /// Read all frames available.
    /**
     * @return  frames pending
     */
    virtual QList<can_frame> readAll();

    /// Write frame.
    /**
     * @param[in] frame  frame to write
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool write( const can_frame &frame );

    /// Write frames.
    /**
     * @param[in] frames  frames to write
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool write( const QList<can_frame> &frames );

    /// Wait for frame to be available.
    /**
     * @param[in] msecs  how long to wait
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool waitForReadyRead( int msecs );

protected:

    canSocket *s_;                                  ///< CAN Socket.
    rangeTypeList deviceRanges_;                    ///< CAN Id Ranges.

    QList<can_frame> buffer_;                       ///< Receive buffer.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] s  can socket
     */
    canDev( canSocket *s );

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Check if can id in range.
    /**
     * @param[in] id  can id
     * @param[in] range  ranges
     * @return  @c true if in range, @c false otherwise
     */
    static bool validateRange( unsigned int id, const rangeType &range );

    /// Check if can id in range.
    /**
     * @param[in] id  can id
     * @param[in] ranges  list of ranges
     * @return  @c true if in range, @c false otherwise
     */
    static bool validateRange( unsigned int id, const rangeTypeList &ranges );

private Q_SLOTS:

    /// Slot for device destroyed.
    void onDeviceDestroyed( QObject *d );

    /// Receive CAN frame.
    void onRecvFrame(const can_frame *frame );

    //void onDoNothing();

private:

    mutable QMutex m_;

    // not implemented
    canDev();

    // not implemented
    canDev( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CANDEV_H
