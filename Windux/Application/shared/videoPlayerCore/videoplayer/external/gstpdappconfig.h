/**
 * @file gstpdappconfig.h
 * Configuration of gstpd (gst player daemon) application.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef GSTPDAPPCONFIG_H
#define GSTPDAPPCONFIG_H

#include "appconfig.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Configuration of gstpd (gst player daemon) application.
class gstpdAppConfig : public appConfig
{
    typedef gstpdAppConfig _Myt;
    typedef appConfig _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    gstpdAppConfig( QObject *parent = NULL );

    /// Destructor.
    virtual ~gstpdAppConfig();

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

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // GSTPDAPPCONFIG_H
