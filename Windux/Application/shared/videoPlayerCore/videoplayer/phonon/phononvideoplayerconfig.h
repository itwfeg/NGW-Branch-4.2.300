/**
 * @file phononvideoplayerconfig.h
 * Configuration of phonon per platform.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef PHONONVIDEOPLAYERCONFIG_H
#define PHONONVIDEOPLAYERCONFIG_H

#include <QtGlobal>

// Note:
// Phonon is not supported in Qt5
#if ( QT_VERSION < 0x050000 )

#include "../videoplayer.h"

#include <QFile>
#include <QString>
#include <QStringList>

#include <platform.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Platform dependant configuration of phonon.
/**
 * This struct contains platform dependant information on phonon settings. This struct should be
 * implemented for each platform you with to support.
 *
 * Example usage:
 * @code
 *   phononVideoPlayerConfig<PLATFORM> myConfiguration;
 * @endcode
 *
 * @tparam _Platform  platform type
 */
template <platformType::type _Platform>
struct phononVideoPlayerConfig
{
    /// Audio device name for phonon audio sink configuration.
    /**
     * @return  device name string
     */
    static QString audioDeviceName() {
        return "";
    }

    /// Video device name for phonon audio sink configuration.
    /**
     * @param[in] device  device type
     * @return  device name string
     */
    static QString videoDeviceName( videoPlayerDevice::type device ) {
        Q_UNUSED( device );
        return "";
    }

    /// Environment settings to create.
    /**
     * @param[in] device  device type
     * @return  list of environment variables.
     */
    static QStringList environment( videoPlayerDevice::type device ) {
        Q_UNUSED( device );
        QStringList list;
        return list;
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Specialization for the @c platformType::EmbeddedLinux platform.
template <>
struct phononVideoPlayerConfig<platformType::EmbeddedLinux>
{
    /// Audio device name for phonon audio sink configuration.
    /**
     * @return  device name string
     */
    static QString audioDeviceName() {
        return ""; // was "hw:0,0" but we need default device in order to share audio
    }

    /// Video device name for phonon audio sink configuration.
    /**
     * @param[in] device  device type
     * @return  device name string
     */
    static QString videoDeviceName( videoPlayerDevice::type device ) {
        Q_UNUSED( device );
        return "";
    }

    /// Environment settings to create.
    /**
     * @param[in] device  device type
     * @return  list of environment variables.
     */
    static QStringList environment( videoPlayerDevice::type device ) {
        QStringList list;

        // Note:
        // There is a limitation in Phonon today that we cannot pass to the video sink any
        // arguments like you can using gst-launch. This is a big problem for us because we wish
        // to play videos on two displays at the same time and we must specify which display to
        // use when we launch the player. The Freescale V4L sink today defaults to video17, this
        // is a hard coded value. We have cobbled together a patch for their driver that will
        // accept a device name from the environment settings instead of the hard coded one. Below
        // will set this environment setting.

        // Note:
        // The Freescale i.MX6 Dual and Quad have two IPUs, the Solo only has one. This presents
        // an interesting issue when dealing with two displays since each IPU only has three
        // channels (background, foreground, and direct)...
        //
        // 1) In the Dual and Quad cases, we have the following frame buffer allocations:
        //
        //                            +--> IPU0 DP-BG : /dev/fb0 <----> /dev/video16
        // LVDS0 (operator display) --+
        //                            +--> IPU0 DP-FG : /dev/fb1 <----> /dev/video17 (overlay)
        //
        //                            +--> IPU1 DP-BG : /dev/fb2 <----> /dev/video18
        // LVDS1 (customer display) --+
        //                            +--> IPU1 DP-FG : /dev/fb3 <----> /dev/video19 (overlay)
        //
        // 2) In the Solo case, we have the following frame buffer allocations:
        //
        //                            +--> IPU0 DP-BG : /dev/fb0 <----> /dev/video16
        // LVDS0 (operator display) --+
        //                            +--> IPU0 DP-FG : /dev/fb1 <----> /dev/video17 (overlay)
        //
        // LVDS1 (customer display) -----> IPU0 DC    : /dev/fb2 <----> /dev/video18
        //
        // For the Dual and Quad we obviously want to use /dev/video19 as its an overlay and will
        // allow the gcd application and videos to be displayed simultaneously.
        //
        // For the Solo we must choose between the gcd application or videos since we have no
        // overlay device.
        //
        // At the present moment the Hobart HTi line of scales are using a Dual.

        if ( videoPlayerDevice::Customer == device )
        {
            if ( QFile::exists( "/dev/video19" ) )
            {
                list.push_back( "MFW_V4LSINK_FB=/dev/fb2" );
                list.push_back( "MFW_V4LSINK_DEVICE=/dev/video19" );
            }
            else
            {
                list.push_back( "MFW_V4LSINK_DEVICE=/dev/video18" );
            }
        }

        return list;
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // QT_VERSION < 0x050000

#endif // PHONONVIDEOPLAYERCONFIG_H
