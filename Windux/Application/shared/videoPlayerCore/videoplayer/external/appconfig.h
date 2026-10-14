/**
 * @file appconfig.h
 * Abstract application configuration.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "../videoplayer.h"

#include <QObject>
#include <QRect>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Abstract application configuration.
class appConfig : public QObject
{
    typedef appConfig _Myt;
    typedef QObject _Mybase;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~appConfig();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Command to launch application.
    /**
     * @param[in] filename  filename of media to play
     * @param[in] geometry  geometry of video output
     * @param[in] device  device video should play on
     * @return  command line
     */
    virtual QString launchCommand( const QString &filename, const QRect &geometry, videoPlayerDevice::type device ) const;

    /// Retrieve the play command syntax for a process id.
    /**
     * @param[in] id  process id
     * @return  command line
     */
    virtual QString playCommand( unsigned int id ) const;

    /// Retrieve the pause command syntax for a process id.
    /**
     * @param[in] id  process id
     * @return  command line
     */
    virtual QString pauseCommand( unsigned int id ) const;

    /// Retrieve the stop command syntax for a process id.
    /**
     * @param[in] id  process id
     * @return  command line
     */
    virtual QString stopCommand( unsigned int id ) const;

    /// Retrieve the kill command syntax for a process id.
    /**
     * @param[in] id  process id to kill
     * @return  command line
     */
    virtual QString killCommand( unsigned int id ) const;

    /// Retrieve the force kill command syntax for a process id.
    /**
     * @param[in] id  process id to kill
     * @return  command line
     */
    virtual QString forceKillCommand( unsigned int id ) const;

    // ========================================================================

    /// Log file for process.
    /**
     * @param[in] device  device being logged
     * @return  log file name
     */
    virtual QString logfile( videoPlayerDevice::type device ) const;

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    appConfig( QObject *parent = NULL );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // APPCONFIG_H
