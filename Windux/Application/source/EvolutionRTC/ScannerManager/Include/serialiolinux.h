/**
 * @file serialiolinux.h
 * Serial communication class.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SERIALIOLINUX
#define SERIALIOLINUX

#ifdef __linux__

#include <pthread.h>

#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Serial communication class.
class serialComm
{
    typedef serialComm _Myt;

    friend void *_SerialCommThreadFn( void *param );

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    serialComm();

    /// Destructor.
    virtual ~serialComm();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check validity.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValid() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Close device.
    virtual void close();

    /// Open device.
    /**
     * @param[in] portName  port name
     * @param[in] baudRate  baud rate
     * @param[in] parity  parity
     * @param[in] stopBits  stop bits
     * @param[in] byteSize  byte size
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool open( const std::string &portName, unsigned int baudRate = 9600, unsigned char parity = 0, unsigned char stopBits = 1, unsigned char byteSize = 8 );

    /// Read data from device.
    /**
     * @return  buffered data
     */
    virtual std::string read();

    /// Write data to device.
    /**
     * @param[in] buffer  data to write
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool write( const std::string &buffer );

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if thread running.
    /**
     * @return  @c true if running, @c false otherwise
     */
    virtual bool isRunning() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Main thread loop.
    virtual void run();

    /// Start thread.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool start();

    /// Stop thread.
    virtual void stop();

private:

    enum {SLEEP_TIME = 250000};

    int comm_;

    pthread_t thread_;
    bool threadStarted_;
    bool threadTerminated_;

    pthread_mutex_t m_;

    std::string buffer_;

    // Set interface attributes.
    static bool setInterfaceAttrib( int fd, unsigned int baudRate = 9600, unsigned char parity = 0 );

    // Set blocking.
    static bool setBlocking( int fd, bool shouldBlock );

    // not implemented
    serialComm( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __linux__

#endif // SERIALIOLINUX

