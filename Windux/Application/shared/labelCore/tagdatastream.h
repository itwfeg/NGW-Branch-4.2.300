/**
 * @file tagdatastream.h
 * Data stream of Hobart tag data.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef TAGDATASTREAM_H
#define TAGDATASTREAM_H

#include <QDataStream>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class tagDataStream
 * @brief Data stream of Hobart tag data.
 *
 * This is just a wrapper class around @c QDataStream.
 */
class tagDataStream : public QDataStream
{
    typedef tagDataStream _Myt;
    typedef QDataStream _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructs a data stream that has no I/O device.
     *
     * @sa setDevice()
     */
    tagDataStream();

    /**
     * Constructs a data stream that uses the I/O device @p d.
     *
     * @warning
     * If you use @c QSocket or @c QSocketDevice as the I/O device @p d for reading data, you must
     * make sure that enough data is available on the socket for the operation to successfully
     * proceed; @c tagDataStream does not have any means to handle or recover from short-reads.
     *
     * @param[in] d  i/o device to use
     *
     * @sa setDevice()
     * @sa device()
     */
    tagDataStream( QIODevice *d );

    /**
     * Constructs a data stream that operates on a byte array, @p a. The @p mode describes how the
     * device is to be used.
     *
     * Alternatively, you can use tagDataStream( const QByteArray& ) if you just want to read from
     * a byte array.
     *
     * Since tagDataStream is not a @c QIODevice subclass, internally a @c QBuffer is created to
     * wrap the byte array.
     *
     * @param[in] a  byte array to operate on
     * @param[in] mode  how to use the device
     */
    tagDataStream( QByteArray *a, QIODevice::OpenMode mode );

    /**
     * Constructs a read-only data stream that operates on byte array @p a. Use
     * tagDataStream( QByteArray*, int ) if you want to write to a byte array.
     *
     * Since @c tagDataStream is not a @c QIODevice subclass, internally a @c QBuffer is created to
     * wrap the byte array.
     *
     * @param[in] a  byte array to operate on
     */
    tagDataStream( const QByteArray& a );

    /**
     * Destroys the data stream.
     *
     * The destructor will not affect the current I/O device, unless it is an internal I/O device
     * (e.g. a @c QBuffer) processing a @c QByteArray passed in the constructor, in which case the
     * internal I/O device is destroyed.
     */
    virtual ~tagDataStream();

    ///////////////////////////////////////////////////////////////////////////
    // Operators

    /**
     * Data extraction operator.
     *
     * @param [in] rhs  object to serialize
     * @returns  reference to @p lhs
     */
    template <typename T>
    tagDataStream& operator << ( const T& rhs ) {
        _Mybase::operator << ( rhs );
        return *this;
    }

    /**
     * Data insertion operator.
     *
     * @param[out] rhs  object to parse into
     * @returns  reference to @p lhs
     */
    template <typename T>
    tagDataStream& operator >> ( T& rhs ) {
        _Mybase::operator >> ( rhs );
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Check passed in device to see if it is a type of @c tagDataStream.
     *
     * This method will peek at the first few bytes of @p d and determine if the stream type
     * matches a @c tagDataStream type.
     *
     * If the device @p d is indeed a @c tagDataStream, you can map it into the class using
     * the following code:
     * @code
     *   // assuming there exists a QDataStream called stream already
     *   if ( tagDataStream::isDeviceTypeOf( stream.device() ) ) {
     *       tagDataStream taggedStream( stream.device() );
     *       // do something with taggedStream...
     *   }
     * @endcode
     *
     * @param[in] d  stream to check
     * @returns  @c true if @p d appears to be a @c tagDataStream, @c false otherwise
     */
    static bool isDeviceTypeOf( QIODevice *d );

private:

    /**
     * Initialize object.
     */
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // TAGDATASTREAM_H
