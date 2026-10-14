/**
 * @file serialiowin.h
 * Serial communication class.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SERIALIOWIN
#define SERIALIOWIN

#ifndef __linux__

#include <Windows.h>

#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Serial communication class.
class serialComm
{
    typedef serialComm _Myt;

    friend unsigned int __stdcall _SerialCommThreadFn( void *param );

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
    virtual bool open( const std::string &portName, DWORD baudRate = 9600, BYTE parity = 0, BYTE stopBits = 1, BYTE byteSize = 8 );

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

    enum {COMM_WAIT_TIMEOUT_WRITE = 250};

    HANDLE comm_;

    HANDLE thread_;
    HANDLE threadStarted_;
    HANDLE threadTerminated_;

    HANDLE m_;

    std::string buffer_;

    /// Close handle.
    static void close( HANDLE &handle );

    /// Invalidate handle.
    static void invalidate( HANDLE &handle );

    // not implemented
    serialComm( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __linux__

#endif // SERIALIOWIN

