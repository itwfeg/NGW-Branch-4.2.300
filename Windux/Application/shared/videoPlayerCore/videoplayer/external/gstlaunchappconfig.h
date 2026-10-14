/**
 * @file gstreamerappconfig.h
 * Configuration of gstlaunch application.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef GSTLAUNCHAPPCONFIG_H
#define GSTLAUNCHAPPCONFIG_H

#include <QtGlobal>

// Linux only!
#ifndef Q_OS_WIN

#include "appconfig.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Configuration of gstlaunch application.
class gstlaunchAppConfig : public appConfig
{
    typedef gstlaunchAppConfig _Myt;
    typedef appConfig _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    gstlaunchAppConfig( QObject *parent = NULL );

    /// Destructor.
    virtual ~gstlaunchAppConfig();

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
    // Properties
    // ========================================================================

    /// Retrieve audio sink details.
    /**
     * @param[in] device  device audio should play on
     * @return  gst-launch audio sink config
     */
    virtual QString audioSink( videoPlayerDevice::type device ) const;

    /// Retrieve video sink details.
    /**
     * @param[in] geometry  geometry of video output
     * @param[in] device  device video should play on
     * @return  gst-launch video sink config
     */
    virtual QString videoSink( const QRect &geometry, videoPlayerDevice::type device ) const;

private:

    /// Video sink device name.
    QString videoSinkDeviceName( videoPlayerDevice::type device ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // linux

/** @} */

#endif // GSTLAUNCHAPPCONFIG_H
