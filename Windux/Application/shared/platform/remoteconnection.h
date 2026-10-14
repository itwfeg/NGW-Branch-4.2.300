/**
 * @file remoteconnection.h
 * Class that connects to remote machines.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup platformCsu
 * @{
 */

#ifndef REMOTECONNECTION_H
#define REMOTECONNECTION_H

#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Class that connects to remote machines.
class abstractRemoteConnection
{
    typedef abstractRemoteConnection _Myt;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~abstractRemoteConnection() {}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create connection to remote host.
    /**
     * @param[in] host  host to connect to
     * @return  @c true if connected, @c false otherwise
     */
    virtual bool connect( const QString& host ) = 0;

    /// Download file to local system.
    /**
     * Create connection to remote host.
     *
     * @param[in] host  host to connect to
     * @param[in] user  username
     * @param[in] pwd  password
     * @returns  @c true if connected, @c false otherwise
     */
    virtual bool connectAsUser( const QString& host, const QString& user, const QString& pwd ) = 0;

    /**
     * Download file to local system.
     *
     * @param[in] remote  remote file name
     * @param[in] local  local file name
     * @return  @c true if downloaded, @c false otherwise
     */
    virtual bool downloadFile( const QString& remote, const QString& local ) = 0;

    /// Download all files in remote path to local system.
    /**
     * @param[in] remote  remote file path
     * @param[in] local  local file path
     * @return  @c true if downloaded, @c false otherwise
     */
    virtual bool rsync( const QString& remote, const QString& local ) = 0;

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    abstractRemoteConnection() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Class that connects to remote machines.
/**
 * @tparam T  connection type (i.e. http, ssh)
 */
template <class T>
class remoteConnection : public abstractRemoteConnection
{
    typedef remoteConnection<T> _Myt;
    typedef abstractRemoteConnection _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    remoteConnection() {}

    /// Destructor.
    virtual ~remoteConnection() {}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create connection to remote host.
    /**
     * @param[in] host  host to connect to
     * @return  @c true if connected, @c false otherwise
     */
    virtual bool connect( const QString& host );

    /// Download file to local system.
    /**
     * Create connection to remote host.
     *
     * @param[in] host  host to connect to
     * @param[in] user  username
     * @param[in] pwd  password
     * @returns  @c true if connected, @c false otherwise
     */
    virtual bool connectAsUser( const QString& host, const QString& user, const QString& pwd );

    /**
     * Download file to local system.
     *
     * @param[in] remote  remote file name
     * @param[in] local  local file name
     * @return  @c true if downloaded, @c false otherwise
     */
    virtual bool downloadFile( const QString& remote, const QString& local );

    /// Download all files in remote path to local system.
    /**
     * @param[in] remote  remote file path
     * @param[in] local  local file path
     * @return  @c true if downloaded, @c false otherwise
     */
    virtual bool rsync( const QString& remote, const QString& local );

private:

    T c_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // REMOTECONNECTION_H
